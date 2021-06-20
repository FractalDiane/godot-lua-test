#pragma once

#include <Godot.hpp>
#include <Node.hpp>
#include <Timer.hpp>

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

namespace godot {

class Cutscene : public Node {
	GODOT_CLASS(Cutscene, Node)

private:
	String lua_file;
	lua_State* lua;

	static Cutscene* current_cutscene;

public:
	static void _register_methods();
	void _init();

	void _ready();
	void _exit_tree();

	void run_cutscene();

	static inline Cutscene* get_current_cutscene() { return current_cutscene; }

private:
	void run_lua_file();
	void end_wait();
	//void select_choice(int index);
};

}
