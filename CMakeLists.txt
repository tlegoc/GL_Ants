cmake_minimum_required(VERSION 3.25)

project(AntsGL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)

configure_file(shaders/ants.comp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ants.comp  COPYONLY)
configure_file(shaders/anthill_food_render.comp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/anthill_food_render.comp  COPYONLY)
configure_file(shaders/input.comp ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/input.comp  COPYONLY)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

add_subdirectory(src)