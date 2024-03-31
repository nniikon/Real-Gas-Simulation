#ifndef GAS_STRUCTS_H_
#define GAS_STRUCTS_H_

#include "stdint.h"
#include "../glm/glm/glm.hpp"

struct gas_Atoms {
    glm::vec3*   coords;
    size_t     n_coords;
    float        radius;
};

#endif // GAS_STRUCTS_H_ 
