cmake_minimum_required(VERSION 3.1)

project(Checkers LANGUAGES C CXX)

set(GLEW_USE_STATIC_LIBS TRUE)
set(GLFW_SHARED_LIBS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(checkers main.cpp)
target_include_directories(checkers PRIVATE ${glfw_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(checkers PRIVATE opengl32 glfw GLEW::glew_s)
set_target_properties(checkers PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 20
    )
target_compile_definitions(checkers PRIVATE GLEW_STATIC)
