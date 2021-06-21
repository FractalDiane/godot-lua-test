cmake_minimum_required(VERSION 3.20)
project(LuaTest)

set(CMAKE_CXX_STANDARD 20)

include_directories(SYSTEM
	 "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/godot-headers/" , 
	 "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/include/" , 
	 "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/include/core/" , 
	 "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/include/gen/" 
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	include_directories(SYSTEM "C:/lua-5.4.2_Win64_dllw6_lib/include/")
endif()

set(LUATEST_SRC
	cutscene.cpp
	gdnative_library.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/../../game/library/bin")
	set(LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../../game/library/bin")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../../game/library/bin")

add_library(game SHARED ${LUATEST_SRC})

option(Target "Compilation target (debug or release)" "debug")
if(Target STREQUAL "debug")
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(LUATEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/bin/libgodot-cpp.windows.debug.64.lib")
	else()
		set(LUATEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/bin/libgodot-cpp.linux.debug.64.a")
	endif()
elseif(Target STREQUAL "release")
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		set(LUATEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/bin/libgodot-cpp.windows.release.64.lib")
	else()
		set(LUATEST_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../../../godot-cpp/bin/libgodot-cpp.linux.release.64.a")
	endif()
else()
	message(FATAL_ERROR "Invalid target \"${Target}\"")
endif()

target_compile_options(game PRIVATE -Wall -Wpedantic)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(game lua)
else()
	target_link_libraries(game "C:/lua-5.4.2_Win64_dllw6_lib/liblua54.a")
endif()

target_link_libraries(game ${LUATEST_LIB})
