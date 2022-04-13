cmake_minimum_required(VERSION 3.16)
project(example)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(LIBIGL_GLFW "Build target igl::glfw" ON)
include(libigl)
include(glew)

file(GLOB SRC_FILES *.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PUBLIC igl::glfw libglew_static)