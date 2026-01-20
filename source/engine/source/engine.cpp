#include "engine/engine.hpp"

#include <assert.h>
#include <cmath>

#include "logs/logs.hpp"

static FILE* gLogFile = nullptr;

static uint16_t eng_SelectDivisions(size_t n_atoms) {
    const size_t target_atoms_per_cell = 16;
    const double cells = (double)n_atoms / (double)target_atoms_per_cell;
    size_t axis = (size_t)std::cbrt(cells);
    if (axis < 1) {
        axis = 1;
    } else if (axis > 64) {
        axis = 64;
    }
    return (uint16_t)axis;
}

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
    eng_AtomListConstructor(list, n_atoms, eng_SelectDivisions(n_atoms));
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

    eng_HandleInteractions(list, deltaTime);
    eng_UpdatePositions   (list, deltaTime);

    if (time == 100000) {
        float avg_energy = eng_GetAvgSpeed2(list);
        float avg_out_energy = 0.0f;
        if (list->n_hole_hits > 0) {
            avg_out_energy = list->total_hole_energy / (float)list->n_hole_hits;
        }
        printf("average gas energy: %g\n", avg_energy);
        printf("average out energy: %g\n", avg_out_energy);
        if (avg_energy > 0.0f) {
            printf("coefficient: %g\n\n", avg_out_energy / avg_energy);
        } else {
            printf("coefficient: 0\n\n");
        }
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
