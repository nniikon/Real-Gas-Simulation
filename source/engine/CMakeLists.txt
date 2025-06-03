add_library(engine STATIC)

target_sources(engine
    PRIVATE
        source/engine.cpp
        source/eng_atoms_list.cpp
)

target_include_directories(engine
    PUBLIC
        include
)

target_link_libraries(engine
    PRIVATE
        common
        flags
        libs
        glm
)
