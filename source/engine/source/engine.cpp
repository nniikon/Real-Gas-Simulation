#include "engine/engine.hpp"

#include <assert.h>

#include "logs/logs.hpp"

static FILE* gLogFile = nullptr;

const char* eng_GetErrorMsg(const eng_Error err) {
    #define DEF_ERR(err, msg)                                                  \
        case ENG_ERR_ ## err:                                                  \
            return msg;

    switch (err) {
        #include "engine/engine_errors_codegen.inc"
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


eng_Error eng_Compute(eng_AtomList* list, const float deltaTime, float radius, FILE* file) {
    assert(list);
    LOG_FUNC_START(gLogFile);

    list->hole_radius = radius;

    static int time = 0;

    eng_HandleInteractions(list);
    eng_UpdatePositions   (list, deltaTime);

    if (time == 100) {
        float avg_energy = eng_GetAvgSpeed2(list);
        float avg_out_energy = list->total_hole_energy / (float)list->n_hole_hits;
        printf("average gas energy: %g\n", avg_energy);
        printf("average out energy: %g\n", avg_out_energy);
        printf("coefficient: %g\n\n", avg_out_energy / avg_energy);
        fprintf(file, "%g ",   avg_energy);
        fprintf(file, "%g\n", avg_out_energy);

        list->n_hole_hits = 0;
        list->total_hole_energy = 0.0f;
        time = 0;
    }

    time++;

    //for (size_t atom = 0; atom < list->size; atom++) {
    //    LOGF_COLOR(gLogFile, blue, "Atom %lu\n", atom);
    //    LOGF_COLOR(gLogFile, blue, "\t x %lg\n", list->positions[atom].x);
    //    LOGF_COLOR(gLogFile, blue, "\t y %lg\n", list->positions[atom].y);
    //    LOGF_COLOR(gLogFile, blue, "\t z %lg\n", list->positions[atom].z);
    //}

    LOG_FUNC_END(gLogFile);
    return ENG_ERR_NO;
}
