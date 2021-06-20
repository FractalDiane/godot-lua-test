cmake_minimum_required(VERSION 3.20)
project(LuaTest)

set(CMAKE_CXX_STANDARD 20)

include_directories(SYSTEM
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/godot-headers/",
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/include/",
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/include/core/",
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/include/gen/"
)

set(LUATEST_SRC
	cutscene.cpp
	gdnative_library.cpp
)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../../game/library/bin")

add_library(game SHARED ${LUATEST_SRC})

option(Target "Compilation target (debug or release)" "debug")
if(Target STREQUAL "debug")
	set(LUATEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/bin/libgodot-cpp.linux.debug.64.a")
elseif(Target STREQUAL "release")
	set(LUATEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/bin/libgodot-cpp.linux.release.64.a")
else()
	message(FATAL_ERROR "Invalid target \"${Target}\"")
endif()

target_compile_options(game PRIVATE -Wall -Wpedantic)

target_link_libraries(game lua)
target_link_libraries(game ${LUATEST_LIB})
