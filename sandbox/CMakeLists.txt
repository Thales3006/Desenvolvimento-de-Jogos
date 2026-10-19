# Sandbox build — sfml-tilemap-test branch only.
#
# Visual test harness for the TileMap / Dungeon2D pipeline. It links SFML 3
# against the (SFML-agnostic) engine sources it reuses from ../source.
#
# Build:
#   cmake -S sandbox -B sandbox/build
#   cmake --build sandbox/build
#   ./sandbox/build/tilemap_sandbox
#
# On this machine SFML 3 lives in the nix store; CMake finds it through
# CMAKE_PREFIX_PATH (see configure note in CLAUDE.md).

cmake_minimum_required(VERSION 3.16)
project(tilemap_sandbox CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../source)

add_executable(tilemap_sandbox
    tilemap_sandbox.cpp
    ${ENGINE_DIR}/TileMap.cpp
    ${ENGINE_DIR}/Dungeon2D.cpp
    ${ENGINE_DIR}/Vector2D.cpp
)

target_include_directories(tilemap_sandbox PRIVATE ${ENGINE_DIR})
target_link_libraries(tilemap_sandbox PRIVATE SFML::Graphics SFML::Window SFML::System)
