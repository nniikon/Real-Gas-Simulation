#include "../include/engine.h"
#include "../../libs/logs/logs.h"

#include <assert.h>
#include <stdio.h>

static FILE* kLogFile = nullptr;

const char* eng_GetErrorMsg(const eng_Error err)
{
    #define DEF_ERR(err, msg)                                                  \
        case ENG_ERR_ ## err:                                                  \
            return msg;

    switch (err)
    {
        #include "../include/engine_errors_codegen.inc"
        default:
            return "No such error was found";
    }
    #undef DEF_ERR
}

eng_Error eng_Ctor(gas_Atoms* atoms, eng_AtomList* list, const size_t n_atoms)
{
    assert(list);

    // TODO: cringe
    // TODO: error check
    eng_AtomListConstructor(list, n_atoms, 9);
    eng_SetRandomPositions (list);

    atoms->coords = list->positions;
    atoms->n_coords = n_atoms;

    return ENG_ERR_NO;
}


eng_Error eng_Compute(eng_AtomList* list, const float deltaTime) {
    assert(list);

    eng_HandleInteractions(list);
    eng_UpdatePositions   (list, deltaTime);

    return ENG_ERR_NO;
}
