#ifndef GRAPHICS_BOX_H_
#define GRAPHICS_BOX_H_

#include <stddef.h>

#include "fwd.hpp"

static const float kScale = 0.9f;

static const glm::vec3 kBox[] = {
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

static const size_t kNOfVertInBox = sizeof(kBox) / sizeof(*kBox);

#endif // GRAPHICS_BOX_H_