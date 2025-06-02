add_library(engine STATIC)

add_subdirectory(atoms_list)

target_sources(engine
    PRIVATE
        source/engine.cpp
)

target_include_directories(engine
    PUBLIC
        include
)

target_link_libraries(engine
    PRIVATE
        flags
        atoms_list
        libs
        glm
)
