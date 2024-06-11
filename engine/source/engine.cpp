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
    eng_AtomListConstructor(list, n_atoms, 1);
    eng_SetRandomPositions (list);

    atoms->coords = list->positions;
    atoms->n_coords = n_atoms;

    LOG_FUNC_END(gLogFile); 
    return ENG_ERR_NO;
}


void eng_SetMode(eng_AtomList* list, eng_Mode mode) {
    list->mode = mode;
}


eng_Error eng_Compute(eng_AtomList* list, const float deltaTime) {
    assert(list);
    LOG_FUNC_START(gLogFile);

    static int time = 0;

    eng_HandleInteractions(list);
    eng_UpdatePositions   (list, deltaTime);

    time++;
    if (time == 100) {
        float avg_speed = eng_GetAvgSpeed(list);
        float area = 3.141592 * 0.1*0.1;
        float n    = (float)list->size / (2*2*2*0.90 * 0.90 * 0.90);
        float z    = n * avg_speed / 4;
        float z2    = n * avg_speed / 6;
        float predict = z * area;
        float predict2 = z2 * area;
        printf("Hole hits:         %d\n", list->n_hole_hits);
        printf("predictCorrect:    %g\n", predict);
        printf("predictIncorrect:  %g\n\n", predict2);
        list->n_hole_hits = 0;
        time = 0;

    }

    //for (size_t atom = 0; atom < list->size; atom++) {
    //    LOGF_COLOR(gLogFile, blue, "Atom %lu\n", atom);
    //    LOGF_COLOR(gLogFile, blue, "\t x %lg\n", list->positions[atom].x);
    //    LOGF_COLOR(gLogFile, blue, "\t y %lg\n", list->positions[atom].y);
    //    LOGF_COLOR(gLogFile, blue, "\t z %lg\n", list->positions[atom].z);
    //}

    LOG_FUNC_END(gLogFile);
    return ENG_ERR_NO;
}
