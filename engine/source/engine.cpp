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

eng_Error eng_Ctor(Engine* engine, const size_t n_atoms)
{
    assert(engine);

    engine->n_atoms = n_atoms;

    return ENG_ERR_NO;
}
