#ifndef GRAPHICS_BOX_HPP_
#define GRAPHICS_BOX_HPP_

#include <cstddef>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

namespace gas {
namespace grx {

constexpr inline float kScale = 0.9f;

constexpr inline const glm::vec3 kBox[] = {
    // Front face
    {-kScale, -kScale,  kScale},
    { kScale, -kScale,  kScale},

    {-kScale, -kScale,  kScale},
    {-kScale,  kScale,  kScale},

    { kScale,  kScale,  kScale},
    { kScale, -kScale,  kScale},

    { kScale,  kScale,  kScale},
    {-kScale,  kScale,  kScale},

    // Right face
    { kScale, -kScale,  kScale},
    { kScale,  kScale,  kScale},

    { kScale,  kScale,  kScale},
    { kScale,  kScale, -kScale},

    { kScale,  kScale, -kScale},
    { kScale, -kScale, -kScale},

    { kScale, -kScale, -kScale},
    { kScale, -kScale,  kScale},

    // Left face
    {-kScale, -kScale,  kScale},
    {-kScale,  kScale,  kScale},

    {-kScale,  kScale,  kScale},
    {-kScale,  kScale, -kScale},

    {-kScale,  kScale, -kScale},
    {-kScale, -kScale, -kScale},

    {-kScale, -kScale, -kScale},
    {-kScale, -kScale,  kScale},
    // Back face
    {-kScale,  kScale, -kScale},
    { kScale,  kScale, -kScale},

    { kScale,  kScale, -kScale},
    { kScale, -kScale, -kScale},

    { kScale, -kScale, -kScale},
    {-kScale, -kScale, -kScale},

    {-kScale, -kScale, -kScale},
    {-kScale,  kScale, -kScale},
    // // Top face
    // {-1, 1, 1},
    // {1, 1, 1},

    // {1, 1, 1},
    // {1, 1, -1},

    // {1, 1, -1},
    // {-1, 1, -1},

    // {-1, 1, -1},
    // {-1, 1, 1},
    // // Bottom face
    // {-1, -1, 1},
    // {-1, -1, -1},

    // {-1, -1, -1},
    // {1, -1, -1},

    // {1, -1, -1},
    // {1, -1, 1},

    // {1, -1, 1},
    // {-1, -1, 1},
};

constexpr inline size_t kNOfVertInBox = sizeof(kBox) / sizeof(*kBox);

} // namespace grx
} // namespace gas

#endif // GRAPHICS_BOX_HPP_
