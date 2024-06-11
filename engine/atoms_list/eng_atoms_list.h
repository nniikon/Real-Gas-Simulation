#ifndef ENG_ATOMS_LIST_
#define ENG_ATOMS_LIST_

#include "../../glm/glm/glm.hpp"
#include <stdio.h>
   
enum eng_Error {
    #define DEF_ERR(err, msg) ENG_ERR_ ## err,
    #include "../include/engine_errors_codegen.inc"
    #undef  DEF_ERR
};

enum eng_Mode {
    ENG_MODE_REAL,
    ENG_MODE_IDEAL,
};

struct eng_AtomList {
    glm::vec3* positions;
    glm::vec3* velocities;
    int64_t*   prev;
    int64_t*   next;
    bool*      is_out_of_box;
    bool*      is_freezed;

    size_t size;
    size_t axis_divisions;
    size_t space_divisions;
    float  radius;

    float  box_size;

    eng_Mode mode;
    int n_hole_hits;
    glm::vec3 total_hole_energy;
};

eng_Error eng_AtomListConstructor(eng_AtomList* atoms, size_t size, uint16_t divisions);
eng_Error eng_UpdatePositions    (eng_AtomList* atoms, float delta_time);
eng_Error eng_HandleInteractions (eng_AtomList* atoms);
eng_Error eng_SetRandomPositions (eng_AtomList* atoms);
eng_Error eng_SetDefaultPositions(eng_AtomList* atoms);
float     eng_GetAvgSpeed        (eng_AtomList* atoms);
void      eng_ListSetLogFile(FILE* file);

#endif // ENG_ATOMS_LIST_
