#ifndef GAS_STRUCTS_H_
#define GAS_STRUCTS_H_

#include "stdint.h"
#include "../glm/glm/glm.hpp"

struct gas_Atoms
{
    const glm::vec3* coords;
    const size_t     n_coords;
    const float      radius;
};

#endif // GAS_STRUCTS_H_ 
