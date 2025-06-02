cmake_minimum_required(VERSION 3.21)

project(gas)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # to generate compile_commands.json

add_executable(gas)

add_subdirectory(third_party)
add_subdirectory(flags)

add_subdirectory(graphics)
add_subdirectory(libs)
add_subdirectory(common)
add_subdirectory(engine)

target_sources(gas
    PRIVATE
        main.cpp
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
