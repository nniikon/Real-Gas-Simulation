#ifndef ENG_ATOMS_LIST_
#define ENG_ATOMS_LIST_

#include "../../glm/glm/glm.hpp"
   
enum eng_Error {
    #define DEF_ERR(err, msg) ENG_ERR_ ## err,
    #include "../include/engine_errors_codegen.inc"
    #undef  DEF_ERR
};

struct eng_AtomList {
    glm::vec3* positions;
    glm::vec3* velocities;
    size_t*    prev;
    size_t*    next;

    size_t size;
    size_t divisions;
    float  radius;
};

eng_Error eng_AtomListConstructor(eng_AtomList* list, size_t* size,
                                                          uint16_t *divisions);
 

#endif // ENG_ATOMS_LIST_
