add_library(atoms_list STATIC)

target_sources(atoms_list
    PRIVATE
        eng_atoms_list.cpp
)

target_include_directories(atoms_list
    PUBLIC
        .
)

target_link_libraries(atoms_list
    PRIVATE
        flags
        libs
        glm
)
