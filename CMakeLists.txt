cmake_minimum_required(VERSION 3.28)
project(boundary C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

include(FetchContent)

FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
)
FetchContent_MakeAvailable(raylib)

FetchContent_Declare(
        cimgui
        GIT_REPOSITORY https://github.com/cimgui/cimgui.git
        GIT_TAG docking_inter
)
set(IMGUI_STATIC ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(cimgui)

add_executable(
        boundary
        main.c
        gui.c
        prim.c
        rlimgui/rlcimgui.c
        euler.c
        scan.c
        shape.c
        transform.c
        linalg.c
        render.c
        tesselate.c
        tesselate.h
        parse.c
        parse.h
)

set_target_properties(boundary PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(boundary PRIVATE raylib cimgui)

add_custom_command(TARGET boundary POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        ${CMAKE_BINARY_DIR}/shaders
)

# Unit tests

FetchContent_Declare(
        unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG v2.5.2
)
FetchContent_MakeAvailable(unity)

add_executable(
        unit_tests
        tests/euler_tests.c
        euler.c
        prim.c
        scan.c
        linalg.c
        tesselate.c
        tesselate.h
)
target_link_libraries(unit_tests PRIVATE unity)