#ifndef ENGINE_H_
#define ENGINE_H_

#include "../../common/gas_structs.h"

#include "../atoms_list/eng_atoms_list.h"

struct Engine
{
    eng_Atom*   atoms;
    size_t    n_atoms;
};

const char* eng_GetErrorMsg(const eng_Error err);

eng_Error eng_Ctor(const size_t n_atoms);

#endif // ENGINE_H_
