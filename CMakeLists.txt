cmake_minimum_required(VERSION 3.10)

project(Checkers LANGUAGES C CXX)

set(GLEW_USE_STATIC_LIBS TRUE)
set(GLFW_SHARED_LIBS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

set(PROJ_SRC "main.cpp" "geometry.cpp")
add_executable(checkers ${PROJ_SRC})
target_include_directories(checkers PRIVATE ${glfw_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})
target_link_libraries(checkers PRIVATE glfw GLEW::glew_s)
if (APPLE)
    target_link_libraries(checkers PRIVATE OpenGL::GL)
endif(APPLE)
if (WIN32)
    target_link_libraries(checkers PRIVATE opengl32)
endif(WIN32)
set_target_properties(checkers PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 17
    )
target_compile_definitions(checkers PRIVATE GLEW_STATIC)
