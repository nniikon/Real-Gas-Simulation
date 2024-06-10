// Check atoms_docpdf.pdf for a better understanding 

#include "eng_atoms_list.h"
#include <assert.h>
#include <stdlib.h>
#include <immintrin.h>
#include "../../libs/logs/logs.h"

static FILE* gLogFile = nullptr;

static float     GetRandomCoordinate();
static float     GetRandomVelocity();
static glm::vec3 GetRandomVector(float (*func)());
static bool eng_HandleAtomCollision    (eng_AtomList* atoms, size_t i, size_t j);
static void eng_HandleVanDerWaalseForce(eng_AtomList* atoms, size_t i, size_t j);
static bool eng_HandleWallCollision    (eng_AtomList* atoms, size_t pos);
static void eng_AdjustLists            (eng_AtomList* atoms);
static void eng_DumpDivisions          (eng_AtomList* atoms);


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

    return ENG_ERR_NO;
}


eng_Error eng_SetDefaultPositions(eng_AtomList* atoms) {
    assert(atoms);

    const float hole_radius = 0.1000f;
    const float angle = glm::radians(60.0f);
    const float box_size = atoms->box_size;
    const float distance_between_atoms = atoms->radius * 2.0f;

    glm::vec3 kDefaultVelocity = glm::vec3(-cos(angle) / 10, sin(angle) / 10, 0.0f);

    for (size_t i = 0; i < atoms->size; i++) {
        float z_offset = i * distance_between_atoms;

        atoms->positions[i] = glm::vec3(-box_size - (i * distance_between_atoms), hole_radius * sin(angle), z_offset);
        atoms->velocities[i] = kDefaultVelocity;
    }

    return ENG_ERR_NO;
}


eng_Error eng_AtomListConstructor(eng_AtomList* list, const size_t size,
                                                      const uint16_t divisions) {
    assert(list);
    assert(size);

    LOG_FUNC_START(gLogFile);

    // TODO: fixme
    list->radius = 0.0050f;
    list->box_size = 0.89f;
    list->axis_divisions = divisions;
    list->space_divisions = divisions * divisions * divisions;
    list->size      = size;
    list->mode = ENG_MODE_IDEAL;

    eng_Error err = ENG_ERR_NO;

    list->positions  = (glm::vec3*) calloc(size, sizeof(glm::vec3));
    if (list->positions == nullptr) {
        err = ENG_ERR_MEM_ALLOC;
        goto bad_alloc_positions;
    }

    list->velocities = (glm::vec3*) calloc(size, sizeof(glm::vec3));
    if (list->velocities == nullptr) {
        err = ENG_ERR_MEM_ALLOC;
        goto bad_alloc_velocities;
    }

    list->next = (int64_t*) calloc(size + list->space_divisions, sizeof(int64_t));
    if (list->next == nullptr) {
        err = ENG_ERR_MEM_ALLOC;
        goto bad_alloc_next;
    }

    list->prev = (int64_t*) calloc(size + list->space_divisions, sizeof(int64_t));
    if (list->prev == nullptr) {
        err = ENG_ERR_MEM_ALLOC;
        goto bad_alloc_prev;
    }

    list->is_out_of_box = (bool*) calloc(size, sizeof(bool));
    if (list->is_out_of_box == nullptr) {
        err = ENG_ERR_MEM_ALLOC;
        goto bad_alloc_out_of_box;
    }

    list->is_freezed    = (bool*) calloc(size, sizeof(bool));
    if (list->is_freezed == nullptr) {
        err = ENG_ERR_MEM_ALLOC;
        goto bad_alloc_freezed;
    }

    list->next += list->space_divisions;
    list->prev += list->space_divisions;

    LOG_FUNC_END(gLogFile);
    return ENG_ERR_NO;

    free(list->is_freezed);
    bad_alloc_freezed:
    free(list->is_out_of_box);
    bad_alloc_out_of_box:
    free(list->prev);
    bad_alloc_prev:
    free(list->next);
    bad_alloc_next:
    free(list->velocities);
    bad_alloc_velocities:
    free(list->positions);
    bad_alloc_positions:

    LOGF_ERR(gLogFile, "Ctor error");
    return err;
}


eng_Error eng_UpdatePositions(eng_AtomList* atoms, float delta_time) {
    assert(atoms);


    // TODO: SEVEROV OPTIMISATION
    for (size_t i = 0; i < atoms->size; i++) {
        if (atoms->is_freezed[i])
            continue;

        atoms->positions[i] += atoms->velocities[i] * delta_time;
    }
    
    return ENG_ERR_NO;
}


static bool eng_HandleAtomCollision(eng_AtomList* atoms, size_t i, size_t j) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    glm::vec3* pos1 = &atoms->positions[i];
    glm::vec3* pos2 = &atoms->positions[j];
    glm::vec3* vel1 = &atoms->velocities[i];
    glm::vec3* vel2 = &atoms->velocities[j];
    float radius = atoms->radius;

    glm::vec3 delta_pos = *pos2 - *pos1;
    float distance2 = glm::dot(delta_pos, delta_pos);

    if (distance2 >= 4.0f * radius * radius) {
        return false;
    }

    glm::vec3 relative_velocity = *vel2 - *vel1;

    glm::vec3 normal = glm::normalize(delta_pos);
    float dot_product = glm::dot(relative_velocity, normal);

    // If they're moving opposite directions
    if (dot_product > 0.0f) [[unlikely]] {
        return false;
    }

    float impulse_magnitude = -2.0f * dot_product;

    *vel1 += impulse_magnitude * normal;
    *vel2 -= impulse_magnitude * normal;

    LOG_FUNC_END(gLogFile);
    return true;
}


static bool eng_HandleWallCollision(eng_AtomList* atoms, size_t pos) {
    glm::vec3* position = &atoms->positions[pos];
    glm::vec3* velocity = &atoms->velocities[pos];
    float box_size = atoms->box_size;
    float radius = atoms->radius;

    if (position->x < -1.5 || position->x > 1.5 ||
        position->y < -1.5 || position->y > 1.5 ||
        position->z < -1.5 || position->z > 1.5   ) {
        atoms->is_freezed[pos] = true;
        return false;
    }

    if (atoms->is_out_of_box[pos] || atoms->is_freezed[pos])
        return false;

    const float hole_radius = 0.0100f;
    const float hole_radius_2 = hole_radius * hole_radius;

    // Check if the atom is colliding with the hole in the left wall
    //if (position->x - radius <= -box_size) {
    //    if ((position->y * position->y + position->z * position->z) <= hole_radius_2) {
    //        atoms->is_out_of_box[pos] = true;
    //        return false;
    //    }
    //}

    bool is_colliding = false;

    // Handle collisions with the walls
    if (position->x - radius <= -box_size) {
        position->x = radius - box_size;
        velocity->x = -velocity->x;
        is_colliding = true;
    } else if (position->x + radius >= box_size) {
        position->x = box_size - radius;
        velocity->x = -velocity->x;
        is_colliding = true;
    }

    if (position->y - radius <= -box_size) {
        position->y = radius - box_size;
        velocity->y = -velocity->y;
        is_colliding = true;
    } else if (position->y + radius >= box_size) {
        position->y = box_size - radius;
        velocity->y = -velocity->y;
        is_colliding = true;
    }

    if (position->z - radius <= -box_size) {
        position->z = radius - box_size;
        velocity->z = -velocity->z;
        is_colliding = true;
    } else if (position->z + radius >= box_size) {
        position->z = box_size - radius;
        velocity->z = -velocity->z;
        is_colliding = true;
    }

    return is_colliding;
}



static float PowNeg7(float num) {
    float num2 = num * num;         // num^2
    float num4 = num2 * num2;       // num^4
    return 1 / (num4 * num2 * num); // num^7
}

static float PowNeg13(float num) {
    float num2 = num * num;         // num^2
    float num4 = num2 * num2;       // num^4
    float num8 = num4 * num4;       // num^8
    return 1 / (num8 * num4 * num); // num^13
}

static void eng_HandleVanDerWaalseForce(eng_AtomList* atoms, size_t i, size_t j) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    glm::vec3 *pos1 = &atoms->positions[i];
    glm::vec3 *pos2 = &atoms->positions[j];
    glm::vec3 *vel1 = &atoms->velocities[i];
    glm::vec3 *vel2 = &atoms->velocities[j];
    float radius = atoms->radius;

    // Lennard-Jones constants
    float epsilon = 1.0f;
    float sigma = 2.0f * radius;

    glm::vec3 delta_pos = *pos2 - *pos1;
    float distance = glm::length(delta_pos);

    // Lennard-Jones potential
    float r = distance / sigma;
    float force = 24.0f * epsilon * (2.0f * PowNeg13(r) - PowNeg7(r));

    glm::vec3 force_vec= force * glm::normalize(delta_pos);

    *vel1 += force_vec;
    *vel2 -= force_vec;

    LOG_FUNC_END(gLogFile);
}


static eng_Error eng_ListPush(eng_AtomList* atoms, int64_t list_index, int64_t elem_index) {
    assert(atoms);
    assert(list_index <= atoms->space_divisions);
    assert(elem_index < atoms->size);

    LOGF(gLogFile, "eng_ListPush: %lu %lu\n", list_index, elem_index);

    int64_t last = atoms->prev[-list_index];
    atoms->next[elem_index] = -list_index;
    atoms->prev[elem_index] = last;
    atoms->next[last] = elem_index;
    atoms->prev[-list_index] = elem_index;
    
    return ENG_ERR_NO;
}


static inline int64_t iabs(int64_t num) {
    if (num >= 0) {
        return num;
    }
    else {
        return -num;
    }
}


eng_Error eng_HandleInteractions(eng_AtomList* atoms) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    size_t size = atoms->size;


    // TODO: fix O(n^2)
    for (size_t i = 0; i < size - 1; i++) {
        if (atoms->mode == ENG_MODE_REAL) {
            for (size_t j = i + 1; j < size; j++) {
                eng_HandleAtomCollision    (atoms, i, j);
                eng_HandleVanDerWaalseForce(atoms, i, j);
            }
        }
        eng_HandleWallCollision(atoms, i);
    }

    return ENG_ERR_NO;
}


void eng_DumpVelocitiesToFile(eng_AtomList* atoms, const char* file_name) {
    assert(atoms);

    FILE* file = fopen(file_name, "w");
    if (file == nullptr)
        return;

    for (size_t i = 0; i < atoms->size; i++) {
        fprintf(file, "%lg\n", glm::length(atoms->velocities[i]));
    }
}

//eng_Error eng_HandleInteractions(eng_AtomList* atoms) {
//    assert(atoms);
//    LOG_FUNC_START(gLogFile);
//
//    int64_t size = (int64_t)atoms->size;
//
//    for (int64_t i = 0; i < size; i++) {
//        eng_HandleWallCollision(atoms, (size_t)i);
//    }
//
//    switch(atoms->mode) {
//        case ENG_MODE_REAL:      break;
//        case ENG_MODE_IDEAL:                       break;
//        default:                assert(0);         break;
//    }
//
//
//    for (size_t i = 0; i < atoms->size; i++) {
//        for (size_t j = 0; j < atoms->size; j++) {
//            eng_HandleAtomCollision    (atoms, i, j);
//            eng_HandleVanDerWaalseForce(atoms, i, j);
//        }
//    }
//
//    return ENG_ERR_NO;
//
//    eng_AdjustLists(atoms);
//
//    int64_t space_divisions = (int64_t)atoms->space_divisions;
//    int64_t  axis_divisions = (int64_t)atoms-> axis_divisions;
//
//    for (int64_t main_list = -space_divisions; main_list < 0; main_list++) {
//        for (int64_t neighbour_list = -space_divisions; neighbour_list < 0; neighbour_list++) {
//
//            int64_t main_list_num = -main_list;
//            int64_t main_x = main_list_num % axis_divisions;
//            main_x = main_x / axis_divisions;
//            int64_t main_y = main_list_num % axis_divisions;
//            main_y = main_y / axis_divisions;
//            int64_t main_z = main_list_num % axis_divisions;
//
//            int64_t neighbour_list_num = -neighbour_list;
//            int64_t neighbour_x = neighbour_list_num % axis_divisions;
//            neighbour_x = neighbour_x / axis_divisions;
//            int64_t neighbour_y = neighbour_list_num % axis_divisions;
//            neighbour_y = neighbour_y / axis_divisions;
//            int64_t neighbour_z = neighbour_list_num % axis_divisions;
//
//            if (iabs(neighbour_x - main_x) > 1 ||
//                iabs(neighbour_y - main_y) > 1 ||
//                iabs(neighbour_z - main_z) > 1)
//            {
//                continue;
//            }
//
//            if (neighbour_x - main_x == 0 &&
//                neighbour_y - main_y == 0 &&
//                neighbour_z - main_z == 0)
//            {
//                continue;
//            }
//
//            int64_t main_elem = atoms->next[main_list];
//            while (main_elem >= 0) {
//                int64_t neig_elem = atoms->next[neighbour_list];
//                while (neig_elem >= 0) {
//                    eng_HandleAtomCollision    (atoms, (size_t)main_elem, (size_t)neig_elem);
//                    eng_HandleVanDerWaalseForce(atoms, (size_t)main_elem, (size_t)neig_elem);
//                    neig_elem = atoms->next[neig_elem];
//                }
//                main_elem = atoms->next[main_elem];
//            }
//
//        }
//    }
//
//    return ENG_ERR_NO;
//}


static void eng_DumpDivisions(eng_AtomList* atoms) {
    size_t* list_sizes = (size_t*) calloc(atoms->space_divisions, sizeof(size_t));
    if (list_sizes == nullptr) {
        return;
    }

    LOGF_COLOR(gLogFile, green, "ALL ELEMENTS\n");
    for (int64_t list_index = -(int64_t)atoms->space_divisions; list_index < (int64_t)atoms->size; list_index++) {
        LOGF_COLOR(gLogFile, green, "#%ld: PREV: %ld NEXT: %ld\n", list_index, atoms->prev[list_index],
                                                             atoms->next[list_index]);
    }

    LOGF_COLOR(gLogFile, green, "LISTS\n");
    for (size_t list_index = 1; list_index <= atoms->space_divisions; list_index++) {
        int64_t cur_elem = atoms->next[-list_index];
        LOGF_COLOR(gLogFile, green, "List #%ld\t\n", list_index);
        while (cur_elem >= 0) {
            LOGF_COLOR(gLogFile, orange, "\t\t elem: %ld\n", cur_elem);
            list_sizes[list_index - 1]++;
            cur_elem = atoms->next[cur_elem];
        }

        LOGF_COLOR(gLogFile, green, "List #%ld: %ld\n", list_index, list_sizes[list_index - 1]);
    }
}


static void eng_AdjustLists(eng_AtomList* atoms) {
    assert(atoms);

    // Reset the lists
    for (int64_t i = -1; i >= -atoms->space_divisions; i--) {
        atoms->next[i] = i;
        atoms->prev[i] = i;
    }

    size_t  axis_divisions = atoms-> axis_divisions;             // 3
    size_t space_divisions = atoms->space_divisions;             // 27
    float  box_size        = atoms->box_size;                    // 0.9
    float  box_length      = 2 * box_size;                       // 1.8
    float  div_length      = box_length / (float)axis_divisions; // 0.6

    assert(axis_divisions * axis_divisions * axis_divisions == space_divisions);

    for (size_t n_atom = 0; n_atom < atoms->size; n_atom++) {
        for (size_t x = 0; x < axis_divisions; x++) {
            for (size_t y = 0; y < axis_divisions; y++) {
                for (size_t z = 0; z < axis_divisions; z++) {
                    float x_low = ((float) x      * div_length) - box_size;
                    float x_top = ((float)(x + 1) * div_length) - box_size;
                    float y_low = ((float) y      * div_length) - box_size;
                    float y_top = ((float)(y + 1) * div_length) - box_size;
                    float z_low = ((float) z      * div_length) - box_size;
                    float z_top = ((float)(z + 1) * div_length) - box_size;

                    float pos_x = atoms->positions[n_atom].x;
                    float pos_y = atoms->positions[n_atom].y;
                    float pos_z = atoms->positions[n_atom].z;

                    if (x_low <= pos_x && pos_x <= x_top &&
                        y_low <= pos_y && pos_y <= y_top &&
                        z_low <= pos_z && pos_z <= z_top)
                    {
                        size_t list_index = axis_divisions * axis_divisions * x + 
                                            axis_divisions * y + z;
                        eng_ListPush(atoms, (int64_t)list_index, (int64_t)n_atom);
                    }
                }
            }
        }
    }
}
