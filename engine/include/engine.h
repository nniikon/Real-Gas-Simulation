#ifndef ENGINE_H_
#define ENGINE_H_

#include "../../common/gas_structs.h"

#include "../atoms_list/eng_atoms_list.h"

const char* eng_GetErrorMsg(const eng_Error err);

eng_Error eng_Ctor(gas_Atoms* atoms, eng_AtomList* list, const size_t n_atoms);

eng_Error eng_Compute(eng_AtomList* list, const float deltaTime);

#endif // ENGINE_H_
