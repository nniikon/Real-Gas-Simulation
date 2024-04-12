// Check atoms_docpdf.pdf for a better understanding 

#include "eng_atoms_list.h"
#include <assert.h>
#include <stdlib.h>
#include "../../libs/logs/logs.h"


static FILE* gLogFile = nullptr;


void eng_ListSetLogFile(FILE* file) {
    gLogFile = file;
}


static float GetRandomCoordinate() {
    // Returns a random float value from [-1.0; 1.0]
    return (float)rand() / (float)(RAND_MAX / 2) - 1.0f;
}


static float GetRandomVelocity() {
    // Returns a random float value from [-1.0; 1.0]
    return (float)rand() / (float)(RAND_MAX / 2) - 1.0f;
}


static glm::vec3 GetRandomVector(float (*func)()) {
    return glm::vec3(func(), func(), func());
}


eng_Error eng_SetRandomPositions(eng_AtomList* atoms) {
    assert(atoms);

    for (size_t i = 0; i < atoms->size; i++) {
        atoms->positions [i] = GetRandomVector(GetRandomCoordinate);
        atoms->velocities[i] = GetRandomVector(GetRandomVelocity);
    }

    LOGF_COLOR(gLogFile, orange, "Set position: (%lg %lg %lg)\n", atoms->positions->x,
                                                                  atoms->positions->y,
                                                                  atoms->positions->z);

    LOGF_COLOR(gLogFile, orange, "Set velocity: (%lg %lg %lg)\n", atoms->velocities->x,
                                                                  atoms->velocities->y,
                                                                  atoms->velocities->z);
    return ENG_ERR_NO;
}


eng_Error eng_AtomListConstructor(eng_AtomList* list, const size_t size,
                                                      const uint16_t divisions) {
    assert(list);
    assert(size);

    void* temp = calloc((size)             * sizeof(glm::vec3) * 2  +
                        (size + divisions) * sizeof(size_t*  ) * 2, 1);
    if (temp == nullptr) {
        return ENG_ERR_MEM_ALLOC;
    }

    list->size      = size;
    list->divisions = divisions;

    list->positions  = (glm::vec3*)temp;
    temp = (glm::vec3*)temp + size;

    list->velocities = (glm::vec3*)temp;
    temp = (glm::vec3*)temp + size;

    list->next = (size_t*)temp;
    temp = (size_t*)temp + size + divisions;

    list->prev = (size_t*)temp;

    // TODO: fixme
    list->radius = 1.0f;

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


static bool eng_HandleAtomCollision(eng_AtomList* atoms, size_t i, size_t j) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    glm::vec3& pos1 = atoms->positions[i];
    glm::vec3& pos2 = atoms->positions[j];
    glm::vec3& vel1 = atoms->velocities[i];
    glm::vec3& vel2 = atoms->velocities[j];
    float radius = atoms->radius;

    glm::vec3 delta_pos = pos2 - pos1;
    float distance = glm::length(delta_pos);

    if (distance >= 2.0f * radius) {
        return false;
    }

    glm::vec3 relative_velocity = vel2 - vel1;

    glm::vec3 normal = glm::normalize(delta_pos);
    float dot_product = glm::dot(relative_velocity, normal);

    // If they're moving opposite directions
    if (dot_product > 0.0f) [[unlikely]] {
        return false;
    }

    float impulseMagnitude = -2.0f * dot_product;

    vel1 += impulseMagnitude * normal;
    vel2 -= impulseMagnitude * normal;

    LOG_FUNC_END(gLogFile);
    return true;
}


static bool eng_HandleWallCollision(eng_AtomList* atoms, size_t pos) {
    glm::vec3& position = atoms->positions[pos];
    glm::vec3& velocity = atoms->velocities[pos];
    float box_size = atoms->box_size;
    float radius = atoms->radius;

    bool is_colliding = false;

    // TODO: fix copypaste
    // Check collision with each wall and handle the bounce
    if (position.x - radius <= 0.0f) {
        position.x = radius;
        velocity.x = -velocity.x;
        is_colliding = true;
    }
    else if (position.x + radius >= box_size) {
        position.x = box_size - radius;
        velocity.x = -velocity.x;
        is_colliding = true;
    }

    if (position.y - radius <= 0.0f) {
        position.y = radius;
        velocity.y = -velocity.y;
        is_colliding = true;
    }
    else if (position.y + radius >= box_size) {
        position.y = box_size - radius;
        velocity.y = -velocity.y;
        is_colliding = true;
    }

    if (position.z - radius <= 0.0f) {
        position.z = radius;
        velocity.z = -velocity.z;
        is_colliding = true;
    }
    else if (position.z + radius >= box_size) {
        position.z = box_size - radius;
        velocity.z = -velocity.z;
        is_colliding = true;
    }

    return is_colliding;
}


static void eng_HandleVanDerWaalseForce(eng_AtomList* atoms, size_t i, size_t j) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    glm::vec3& pos1 = atoms->positions[i];
    glm::vec3& pos2 = atoms->positions[j];
    glm::vec3& vel1 = atoms->velocities[i];
    glm::vec3& vel2 = atoms->velocities[j];
    float radius = atoms->radius;

    // Lennard-Jones constants
    float epsilon = 1.0f;
    float sigma = 2.0f * radius;

    glm::vec3 delta_pos = pos2 - pos1;
    float distance = glm::length(delta_pos);

    // Calculate the force magnitude using the Lennard-Jones potential
    // TODO: write own pow13 and pow7 functions 
    float r = distance / sigma;
    float force = 24.0f * epsilon * (2.0f * (float)pow(r, -13.0f) -
                                            (float)pow(r, -7.0f));

    // Calculate the force vector
    glm::vec3 force_vec= force * glm::normalize(delta_pos);

    // Apply the force to the velocities
    vel1 += force_vec;
    vel2 -= force_vec;

    LOG_FUNC_END(gLogFile);
}


eng_Error eng_HandleInteractions(eng_AtomList* atoms) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    size_t size = atoms->size;

    // TODO: fix O(n^2)
    for (size_t i = 0; i < size - 1; i++) {
        for (size_t j = i + 1; j < size; j++) {
            eng_HandleAtomCollision    (atoms, i, j);
            eng_HandleVanDerWaalseForce(atoms, i, j);

            for (size_t atom = 0; atom < atoms->size; atom++) {
                LOGF_COLOR(gLogFile, blue, "Atom %lu\n", atom);
                LOGF_COLOR(gLogFile, blue, "\t x %lg\n", atoms->positions[atom].x);
                LOGF_COLOR(gLogFile, blue, "\t y %lg\n", atoms->positions[atom].y);
                LOGF_COLOR(gLogFile, blue, "\t z %lg\n", atoms->positions[atom].z);
            }
        }
        eng_HandleWallCollision(atoms, i);
    }

    return ENG_ERR_NO;
}
