#include "../include/engine.h"
#include "../../libs/logs/logs.h"

#include <assert.h>

static FILE* gLogFile = nullptr;

const char* eng_GetErrorMsg(const eng_Error err) {
    #define DEF_ERR(err, msg)                                                  \
        case ENG_ERR_ ## err:                                                  \
            return msg;

    switch (err) {
        #include "../include/engine_errors_codegen.inc"
        default:
            return "No such error was found";
    }
    #undef DEF_ERR
}


void eng_SetLogFile(FILE* file) {
    gLogFile = file;
    eng_ListSetLogFile(file);
}


eng_Error eng_Ctor(gas_Atoms* atoms, eng_AtomList* list, const size_t n_atoms) {
    assert(list);
    LOG_FUNC_START(gLogFile);

    // TODO: cringe
    // TODO: error check
    eng_AtomListConstructor(list, n_atoms, 20);
    eng_SetRandomPositions (list);

    atoms->coords = list->positions;
    atoms->n_coords = n_atoms;

    LOG_FUNC_END(gLogFile); 
    return ENG_ERR_NO;
}


eng_Error eng_Compute(eng_AtomList* list, const float deltaTime) {
    assert(list);
    LOG_FUNC_START(gLogFile);

    eng_HandleInteractions(list);
    eng_UpdatePositions   (list, deltaTime);

    //for (size_t atom = 0; atom < list->size; atom++) {
    //    LOGF_COLOR(gLogFile, blue, "Atom %lu\n", atom);
    //    LOGF_COLOR(gLogFile, blue, "\t x %lg\n", list->positions[atom].x);
    //    LOGF_COLOR(gLogFile, blue, "\t y %lg\n", list->positions[atom].y);
    //    LOGF_COLOR(gLogFile, blue, "\t z %lg\n", list->positions[atom].z);
    //}

    LOG_FUNC_END(gLogFile);
    return ENG_ERR_NO;
}
