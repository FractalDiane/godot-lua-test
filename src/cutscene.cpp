#include "cutscene.h"

#include <ProjectSettings.hpp>
#include <SceneTree.hpp>
#include <Node2D.hpp>

#include <thread>
#include <semaphore>
#include <string>
#include <initializer_list>

using namespace godot;

Cutscene* Cutscene::current_cutscene = nullptr;

namespace {
	Timer* timer_wait;

	std::thread lua_thread;
	std::binary_semaphore lua_semaphore_wait{0};
	std::binary_semaphore lua_semaphore_choice{0};

	std::string speaker;

	int selected_choice;

	struct LuaNode {
		Node* ptr;
	};

	int luanode_get(lua_State* L) {
		String path = luaL_checkstring(L, 1);
		LuaNode* node = static_cast<LuaNode*>(lua_newuserdata(L, sizeof(LuaNode)));
		node->ptr = Cutscene::get_current_cutscene()->get_tree()->get_current_scene()->get_node(NodePath(path));
		luaL_setmetatable(L, "Node");
		return 1;
	}

	int luanode_getposition(lua_State* L) {
		LuaNode* node = static_cast<LuaNode*>(lua_touserdata(L, 1));
		luaL_argcheck(L, node != nullptr, 1, "'node' expected");
		Node2D* target = Object::cast_to<Node2D>(node->ptr);
		String pos = String("({0}, {1})").format(Array::make(target->get_position().x, target->get_position().y));
		lua_pushstring(L, pos.alloc_c_string());
		return 1;
	}

	int register_lua_class(lua_State* L, const char* name, lua_CFunction constructor_function, std::initializer_list<std::pair<lua_CFunction, const char*>> functions) {
		lua_register(L, name, constructor_function);
		luaL_newmetatable(L, name);
		lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");

		for (auto& pair : functions) {
			lua_pushcfunction(L, pair.first);
			lua_setfield(L, -2, pair.second);
		}

		lua_pop(L, 1);
		return 1;
	}

	// ==================================================

	int lua_printsomething(lua_State* L) {
		Godot::print(lua_tostring(L, 1));
		return 0;
	}

	int lua_wait(lua_State* L) {
		float time = lua_tonumber(L, 1);
		timer_wait->set_wait_time(time);
		timer_wait->start();
		lua_semaphore_wait.acquire();
		return 0;
	}

	int lua_setspeaker(lua_State* L) {
		speaker = lua_tostring(L, 1);
		return 0;
	}

	int lua_say(lua_State* L) {
		std::string text = lua_tostring(L, 1);
		Godot::print("{0}: {1}", speaker.c_str(), text.c_str());
		return 0;
	}

	int lua_choice(lua_State* L) {
		std::string text = lua_tostring(L, 1);
		Array choices;
		lua_pushnil(L);
		while (lua_next(L, 2) != 0) {
			choices.push_back(lua_tostring(L, -1));
			lua_pop(L, 1);
		}

		Godot::print("{0}: {1}", speaker.c_str(), text.c_str());
		String choices_str{};
		for (int i = 0; i < choices.size(); i++) {
			String str = choices[i];
			choices_str += str;
		}

		Godot::print(choices_str);
		return 0;
	}

	/*int lua_animation(lua_State* L) {

	}*/

}

void Cutscene::_register_methods() {
	register_method("_ready", &Cutscene::_ready);
	register_method("_exit_tree", &Cutscene::_exit_tree);

	register_method("end_wait", &Cutscene::end_wait);
	//register_method("select_choice", &Cutscene::select_choice);

	register_property("lua_file", &Cutscene::lua_file, String(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_FILE, "*.lua");
}

void Cutscene::_init() {}


void Cutscene::_ready() {
	timer_wait = get_node<Timer>("TimerWait");

	lua = luaL_newstate();
	luaL_openlibs(lua);
	register_lua_class(lua, "Node", luanode_get, {{luanode_getposition, "position"}});

	lua_register(lua, "printsomething", lua_printsomething);
	lua_register(lua, "wait", lua_wait);
	lua_register(lua, "setspeaker", lua_setspeaker);
	lua_register(lua, "say", lua_say);
	lua_register(lua, "choice", lua_choice);

	current_cutscene = this;

	run_cutscene();
}


void Cutscene::_exit_tree() {
	lua_thread.join();
	lua_close(lua);
	current_cutscene = nullptr;
}


void Cutscene::run_cutscene() {
	lua_thread = std::thread{&Cutscene::run_lua_file, this};
}


void Cutscene::run_lua_file() {
	if (luaL_dofile(lua, ProjectSettings::get_singleton()->globalize_path(lua_file).alloc_c_string()) != LUA_OK) {
		Godot::print_error(lua_tostring(lua, -1), __func__, __FILE__, __LINE__);
	}
}


void Cutscene::end_wait() {
	lua_semaphore_wait.release();
}
