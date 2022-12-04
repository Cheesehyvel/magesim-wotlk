project(magesim)

cmake_minimum_required(VERSION 3.2.3)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)

add_executable(${PROJECT_NAME} "src/main.cpp" "src/unit.cpp" "src/buff.cpp" "src/simulation.cpp" "src/state.cpp" "src/player.cpp" "src/mirror_image.cpp" "src/water_elemental.cpp" "src/common.cpp")