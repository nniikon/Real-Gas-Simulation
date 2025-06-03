cmake_minimum_required(VERSION 3.21)

project(gas)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

add_executable(gas)

add_subdirectory(source/third_party)
add_subdirectory(source/flags)

add_subdirectory(source/graphics)
add_subdirectory(source/libs)
add_subdirectory(source/common)
add_subdirectory(source/engine)

target_sources(gas
    PRIVATE
        source/main.cpp
)

target_link_libraries(gas
    PRIVATE
        flags
        common
        engine
        graphics
        libs

        glm
        glad
        glfw
        GL
        X11
        pthread
        Xrandr
        Xi
)
