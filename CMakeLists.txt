project(magesim)

cmake_minimum_required(VERSION 3.2.3)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

find_package(Python COMPONENTS Interpreter)

if (NOT PYTHON_FOUND)
	message(WARNING "Python not found.  Will not regenerate WoW data header")
endif()

add_executable(${PROJECT_NAME}
	"src/main.cpp"
	"src/unit.cpp"
	"src/buff.cpp"
	"src/simulation.cpp"
	"src/state.cpp"
	"src/player.cpp"
	"src/mirror_image.cpp"
	"src/water_elemental.cpp"
	"src/common.cpp"
    "src/config.cpp"
	"src/talents.cpp")

if (PYTHON_FOUND)
	add_custom_target(wow_data_header
		COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/wow_data_code_gen.py
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		BYPRODUCTS "src/wow_data.h"
		COMMENT "Build WoW data header")

	add_dependencies(${PROJECT_NAME} wow_data_header)
endif()