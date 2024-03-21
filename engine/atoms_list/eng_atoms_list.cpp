#include "eng_atoms_list.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static FILE* kLogFile = nullptr;

static float GetRandomCoordinate() {
    // Returns a random float value from [-1.0; 1.0]
    return (float)rand() / (float)(RAND_MAX / 2) - 1.0;
}

static float GetRandomVelocity() {
    // Returns a random float value from [-1.0; 1.0]
    return (float)rand() / (float)(RAND_MAX / 2) - 1.0;
}

static glm::vec3 GetRandomVector(float (*func)())
{
    return glm::vec3(func(), func(), func());
}

eng_Error eng_SetRandomPositions(eng_AtomList* atoms, size_t size)
{
    assert(atoms);
    assert(size);

    for (size_t i = 0; i < size; i++)
    {
        atoms->positions [i] = GetRandomVector(GetRandomCoordinate);
        atoms->velocities[i] = GetRandomVector(GetRandomVelocity);
    }

    return ENG_ERR_NO;
}

eng_Error eng_AtomListConstructor(eng_AtomList* list, size_t* size,
                                                          uint16_t *divisions) {
    assert(list);
    assert(size);

    // Prev / Next
    //
    //  +--- fictional elements (division times)
    //  |   +--- real elements (size times)
    //  V   V
    // +--+------
    // |  |
    // +--+------
    //

    void* temp = calloc((*size)              * sizeof(glm::vec3) * 2  +
                        (*size + *divisions) * sizeof(size_t*  ) * 2, 1);
    if (temp == nullptr) {
        return ENG_ERR_MEM_ALLOC;
    }

    list->size      = *size;
    list->divisions = *divisions;

    list->positions  = (glm::vec3*)temp;
    temp = (glm::vec3*)temp + *size;

    list->velocities = (glm::vec3*)temp;
    temp = (glm::vec3*)temp + *size;

    list->next = (size_t*)temp;
    temp = (size_t*)temp + *size + *divisions;

    list->prev = (size_t*)temp;

    return ENG_ERR_NO;
}


eng_Error eng_UpdatePositions(eng_AtomList* atoms, float delta_time) {
    assert(atoms);

    // TODO: SEVEROV OPTIMISATION
    for (size_t i = 0; i < atoms->size; i++) {
        atoms->positions[i] += atoms->velocities[i] * delta_time;
    }
    
    return ENG_ERR_NO;
}


static bool eng_HandleAtomBounce(eng_AtomList* atoms, size_t i, size_t j) {

    float radius = atoms->radius;
    glm::vec3 delta_pos = atoms->positions[i] - atoms->positions[j];

    // TODO: don't calculate 4R^2 each time
    // Engenius optimization
    float dist_squared = glm::dot(delta_pos, delta_pos);
    if (dist_squared > 4.0f * radius * radius) {
        return false;
    }

    glm::vec3 collision_normal = glm::normalize(delta_pos);
    glm::vec3 relative_velocity = atoms->velocities[j] - atoms->velocities[i];
    float dot_product = glm::dot(relative_velocity, collision_normal);

    // If they're moving away each other
    if (dot_product >= 0.0f) {
        return false;
    }

    glm::vec3 impulse = -2.0f * dot_product * collision_normal;
    atoms->velocities[i] -= impulse;
    atoms->velocities[j] += impulse;

    return true;

}


static bool eng_HandleWallBounce(eng_AtomList* atoms, size_t pos) {
    
}


eng_Error eng_HandleCollisions(eng_AtomList* atoms) {
    assert(atoms);

    size_t size  = atoms->size;

    // TODO: fix O(n^2)
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = i + 1; j < size; j++) {

            eng_HandleAtomBounce(atoms, i, j);

        }
    }

    return ENG_ERR_NO;
}
