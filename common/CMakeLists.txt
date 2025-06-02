add_library(common INTERFACE)

target_include_directories(common
    INTERFACE
        .
)

target_link_libraries(common
    INTERFACE
        flags
        glm
)
