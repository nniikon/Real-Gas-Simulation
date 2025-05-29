add_library(libs STATIC)

target_sources(libs
    PRIVATE
        logs/logs.cpp
)

target_include_directories(libs
    PUBLIC
        .        
)

target_link_libraries(libs
    PRIVATE
        flags
)
