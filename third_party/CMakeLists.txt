include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # v1.0.1
    GIT_SHALLOW true
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
)
FetchContent_MakeAvailable(glad)

set(GLAD_SOURCES_DIR "${glad_SOURCE_DIR}")
set(GLAD_GEN_DIR "${CMAKE_BINARY_DIR}/gladgen")
add_subdirectory("${GLAD_SOURCES_DIR}/cmake" glad_cmake)
glad_add_library(glad STATIC REPRODUCIBLE EXCLUDE_FROM_ALL QUIET LOCATION ${GLAD_GEN_DIR} LOADER API gl:core=3.3)
include_directories("${GLAD_GEN_DIR}/include")
