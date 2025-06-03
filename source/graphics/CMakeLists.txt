add_library(graphics STATIC)

target_sources(graphics
    PRIVATE
        source/graphics.cpp
        source/graphics_log.cpp
        source/graphics_shaders.cpp
)

target_include_directories(graphics
    PUBLIC
        include
)

target_link_libraries(graphics
    PRIVATE
        flags
        engine
        common
        libs
        glad
        glm
)
