#include "stdint.h"

struct gas_AtomCoords
{
    float x;
    float y;
    float z;
};

struct gas_Atoms
{
    const gas_AtomCoords*   coords;
    const uint64_t        n_coords;
};
