// Check atoms_docpdf.pdf for a better understanding

#include "engine/eng_atoms_list.hpp"

#include <assert.h>
#include <stdlib.h>
#include <immintrin.h>
#include <cmath>
#include <limits>

#include "logs/logs.hpp"

static FILE* gLogFile = nullptr;

static float     GetRandomCoordinate();
static float     GetRandomVelocity();
static glm::vec3 GetRandomVector(float (*func)());
static inline bool eng_HandleAtomCollision(eng_AtomList* atoms, size_t i, size_t j,
                                           const glm::vec3& delta_pos, float distance2,
                                           float collision_dist2);
static inline void eng_HandleVanDerWaalseForce(eng_AtomList* atoms, size_t i, size_t j,
                                               const glm::vec3& delta_pos, float distance2,
                                               float delta_time, double epsilon_sigma6,
                                               double epsilon_sigma12, float force_shift);
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

static inline int64_t eng_ListHeadIndex(size_t list_index) {
    return -((int64_t)list_index + 1);
}

static inline size_t eng_GetCellIndex(const eng_AtomList* atoms, const glm::vec3& pos) {
    const float box_size = atoms->box_size;
    const float box_length = box_size * 2.0f;
    const float inv_div_length = (float)atoms->axis_divisions / box_length;

    int x = (int)((pos.x + box_size) * inv_div_length);
    int y = (int)((pos.y + box_size) * inv_div_length);
    int z = (int)((pos.z + box_size) * inv_div_length);

    const int max_index = (int)atoms->axis_divisions - 1;
    if (x < 0) {
        x = 0;
    } else if (x > max_index) {
        x = max_index;
    }
    if (y < 0) {
        y = 0;
    } else if (y > max_index) {
        y = max_index;
    }
    if (z < 0) {
        z = 0;
    } else if (z > max_index) {
        z = max_index;
    }

    return (size_t)((x * (int)atoms->axis_divisions + y) * (int)atoms->axis_divisions + z);
}


// Function to generate normally distributed random numbers using Box-Muller transform
static float generateNormalRandom(float mean, float stdDev) {
    static bool hasSpare = false;
    static double spare;

    if (hasSpare) {
        hasSpare = false;
        return mean + stdDev * spare;
    }

    hasSpare = true;
    static const double twoPi = 2.0 * M_PI;
    double u1 = 0.0;
    double u2 = 0.0;
    do {
        u1 = (double)rand() / RAND_MAX;
        u2 = (double)rand() / RAND_MAX;
    } while (u1 <= std::numeric_limits<double>::min());

    double sqrtMinus2LogU1 = sqrt(-2.0 * log(u1));
    spare = sqrtMinus2LogU1 * sin(twoPi * u2);
    return mean + stdDev * (sqrtMinus2LogU1 * cos(twoPi * u2));
}


static glm::vec3 GetRandomVelocityVector() {
    const float mean = 0.0f;
    const float stdDev = 1.0f;

    float x = generateNormalRandom(mean, stdDev);
    float y = generateNormalRandom(mean, stdDev);
    float z = generateNormalRandom(mean, stdDev);

    return {x, y, z};
}


float eng_GetAvgSpeed(eng_AtomList* atoms) {
    float avg_speed = 0.0f;
    for (size_t i = 0; i < atoms->size; i++) {
        avg_speed += glm::length(atoms->velocities[i]);
    }
    avg_speed /= (float)(atoms->size);
    return avg_speed;
}


float eng_GetAvgSpeed2(eng_AtomList* atoms) {
    float avg_speed = 0.0f;
    float len = 0.0f;
    size_t n_atoms = 0;
    for (size_t i = 0; i < atoms->size; i++) {
        if (!atoms->is_out_of_box[i]) {
            len = glm::length(atoms->velocities[i]);
            avg_speed += len * len;
            n_atoms++;
        }
    }
    if (n_atoms == 0) {
        return 0.0f;
    }
    avg_speed /= (float)(n_atoms);
    return avg_speed;
}


eng_Error eng_SetRandomPositions(eng_AtomList* atoms) {
    assert(atoms);

    for (size_t i = 0; i < atoms->size; i++) {
        atoms->positions [i] = GetRandomVector(GetRandomCoordinate);
        atoms->velocities[i] = GetRandomVelocityVector();
    }

    return ENG_ERR_NO;
}


eng_Error eng_AtomListConstructor(eng_AtomList* list, const size_t size,
                                                      const uint16_t divisions) {
    assert(list);
    assert(size);

    LOG_FUNC_START(gLogFile);

    // TODO: fixme
    list->n_hole_hits = 0;
    list->total_hole_energy = 0.0f;
    list->radius = 0.00005f;
    list->box_size = 0.89f;
    list->axis_divisions = divisions;
    list->space_divisions = divisions * divisions * divisions;
    list->size      = size;
    // list->mode = ENG_MODE_IDEAL;
    list->mode = ENG_MODE_REAL;

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

    for (size_t i = 0; i < atoms->size; i++) {
        if (atoms->is_freezed[i])
            continue;

        atoms->positions[i] += atoms->velocities[i] * delta_time;
    }

    return ENG_ERR_NO;
}


static inline bool eng_HandleAtomCollision(eng_AtomList* atoms, size_t i, size_t j,
                                           const glm::vec3& delta_pos, float distance2,
                                           float collision_dist2) {
    assert(atoms);

    if (distance2 <= 0.0f || distance2 >= collision_dist2) {
        return false;
    }

    const float dist = sqrt(distance2);
    const glm::vec3 normal = delta_pos / dist;
    const float collision_dist = sqrt(collision_dist2);
    const float overlap = collision_dist - dist;
    if (overlap > 0.0f) {
        const glm::vec3 correction = 0.5f * overlap * normal;
        atoms->positions[i] -= correction;
        atoms->positions[j] += correction;
    }

    const glm::vec3 relative_velocity = atoms->velocities[j] - atoms->velocities[i];
    const float dot_product = glm::dot(relative_velocity, normal);

    if (dot_product >= 0.0f) [[unlikely]] {
        return false;
    }

    const glm::vec3 impulse = dot_product * normal;
    atoms->velocities[i] += impulse;
    atoms->velocities[j] -= impulse;

    return true;
}


static bool eng_HandleWallCollision(eng_AtomList* atoms, size_t pos) {
    glm::vec3* position = &atoms->positions[pos];
    glm::vec3* velocity = &atoms->velocities[pos];
    float box_size = atoms->box_size;
    float radius = atoms->radius;

    if (atoms->is_out_of_box[pos] || atoms->is_freezed[pos])
        return false;

    const float hole_radius = atoms->hole_radius;
    const float hole_radius_2 = hole_radius * hole_radius;

    // Check if the atom is colliding with the hole in the left wall
    if (position->x - radius <= -box_size) {
        if ((position->y * position->y + position->z * position->z) <= hole_radius_2) {
            atoms->is_out_of_box[pos] = true;
            float len = glm::length(atoms->velocities[pos]);
            atoms->total_hole_energy += len * len;
            atoms->n_hole_hits++;
            return false;
        }
    }

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



static inline void eng_HandleVanDerWaalseForce(eng_AtomList* atoms, size_t i, size_t j,
                                               const glm::vec3& delta_pos, float distance2,
                                               float delta_time, double epsilon_sigma6,
                                               double epsilon_sigma12, float force_shift) {
    assert(atoms);

    const double inv_r2 = 1.0 / (double)distance2;
    const double inv_r6 = inv_r2 * inv_r2 * inv_r2;
    const double inv_r12 = inv_r6 * inv_r6;
    const float force_mag =
        (float)(24.0 * (2.0 * epsilon_sigma12 * inv_r12 - epsilon_sigma6 * inv_r6) * inv_r2)
                            - force_shift;
    const glm::vec3 force_vec = force_mag * delta_pos;

    atoms->velocities[i] += force_vec * delta_time;
    atoms->velocities[j] -= force_vec * delta_time;
}


static eng_Error eng_ListPush(eng_AtomList* atoms, int64_t list_index, int64_t elem_index) {
    assert(atoms);
    assert(list_index >= 0);
    assert((size_t)list_index < atoms->space_divisions);
    assert(elem_index < atoms->size);

    const int64_t head = eng_ListHeadIndex((size_t)list_index);
    const int64_t last = atoms->prev[head];
    atoms->next[elem_index] = head;
    atoms->prev[elem_index] = last;
    atoms->next[last] = elem_index;
    atoms->prev[head] = elem_index;

    return ENG_ERR_NO;
}


eng_Error eng_HandleInteractions(eng_AtomList* atoms, float delta_time) {
    assert(atoms);
    LOG_FUNC_START(gLogFile);

    const size_t size = atoms->size;
    if (size == 0) {
        LOG_FUNC_END(gLogFile);
        return ENG_ERR_NO;
    }

    const float radius = atoms->radius;
    const float collision_dist2 = 4.0f * radius * radius;

    const bool apply_interactions = atoms->mode == ENG_MODE_REAL;
    const double sigma = 2.0 * (double)radius;
    const double sigma2 = sigma * sigma;
    const double sigma6 = sigma2 * sigma2 * sigma2;
    const double sigma12 = sigma6 * sigma6;
    const double epsilon = (double)atoms->radius; // Limit LJ stiffness for very small radii.
    const double epsilon_sigma6 = epsilon * sigma6;
    const double epsilon_sigma12 = epsilon * sigma12;
    const float lj_cutoff = (float)(2.5 * sigma);
    const float lj_cutoff2 = lj_cutoff * lj_cutoff;
    const float min_r2 = 1e-12f;
    const float lj_min_r2 = collision_dist2 > min_r2 ? collision_dist2 : min_r2;
    float force_shift = 0.0f;

    if (lj_cutoff2 > 0.0f) {
        const double inv_rc2 = 1.0 / (double)lj_cutoff2;
        const double inv_rc6 = inv_rc2 * inv_rc2 * inv_rc2;
        const double inv_rc12 = inv_rc6 * inv_rc6;
        force_shift =
            (float)(24.0 * (2.0 * epsilon_sigma12 * inv_rc12 - epsilon_sigma6 * inv_rc6) * inv_rc2);
    }

    float max_cutoff2 = collision_dist2;
    if (apply_interactions && lj_cutoff2 > max_cutoff2) {
        max_cutoff2 = lj_cutoff2;
    }

    if (apply_interactions) {
        eng_AdjustLists(atoms);

        const size_t axis = atoms->axis_divisions;
        if (axis > 0 && max_cutoff2 > 0.0f) {
            // Traverse only neighboring cells to keep pair checks local.
            for (size_t x = 0; x < axis; x++) {
                for (size_t y = 0; y < axis; y++) {
                    for (size_t z = 0; z < axis; z++) {
                        const size_t cell_index = (x * axis + y) * axis + z;
                        const int64_t cell_head = eng_ListHeadIndex(cell_index);

                        for (int nx = (int)x - 1; nx <= (int)x + 1; nx++) {
                            if (nx < 0 || nx >= (int)axis) {
                                continue;
                            }
                            for (int ny = (int)y - 1; ny <= (int)y + 1; ny++) {
                                if (ny < 0 || ny >= (int)axis) {
                                    continue;
                                }
                                for (int nz = (int)z - 1; nz <= (int)z + 1; nz++) {
                                    if (nz < 0 || nz >= (int)axis) {
                                        continue;
                                    }

                                    const size_t neigh_index =
                                        ((size_t)nx * axis + (size_t)ny) * axis + (size_t)nz;
                                    if (neigh_index < cell_index) {
                                        continue;
                                    }

                                    const int64_t neigh_head = eng_ListHeadIndex(neigh_index);

                                    if (neigh_index == cell_index) {
                                        for (int64_t i = atoms->next[cell_head]; i >= 0; i = atoms->next[i]) {
                                            for (int64_t j = atoms->next[i]; j >= 0; j = atoms->next[j]) {
                                                const glm::vec3 delta_pos =
                                                    atoms->positions[(size_t)j] - atoms->positions[(size_t)i];
                                                const float distance2 = glm::dot(delta_pos, delta_pos);
                                                if (distance2 > max_cutoff2) {
                                                    continue;
                                                }

                                            eng_HandleAtomCollision(atoms, (size_t)i, (size_t)j,
                                                                    delta_pos, distance2, collision_dist2);
                                                if (distance2 >= lj_min_r2 && distance2 <= lj_cutoff2) {
                                                    eng_HandleVanDerWaalseForce(atoms, (size_t)i, (size_t)j,
                                                                                delta_pos, distance2, delta_time,
                                                                                epsilon_sigma6, epsilon_sigma12,
                                                                                force_shift);
                                                }
                                            }
                                        }
                                    } else {
                                        for (int64_t i = atoms->next[cell_head]; i >= 0; i = atoms->next[i]) {
                                            for (int64_t j = atoms->next[neigh_head];
                                                 j >= 0;
                                                 j = atoms->next[j]) {
                                                const glm::vec3 delta_pos =
                                                    atoms->positions[(size_t)j] - atoms->positions[(size_t)i];
                                                const float distance2 = glm::dot(delta_pos, delta_pos);
                                                if (distance2 > max_cutoff2) {
                                                    continue;
                                                }

                                            eng_HandleAtomCollision(atoms, (size_t)i, (size_t)j,
                                                                    delta_pos, distance2, collision_dist2);
                                                if (distance2 >= lj_min_r2 && distance2 <= lj_cutoff2) {
                                                    eng_HandleVanDerWaalseForce(atoms, (size_t)i, (size_t)j,
                                                                                delta_pos, distance2, delta_time,
                                                                                epsilon_sigma6, epsilon_sigma12,
                                                                                force_shift);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < size; i++) {
        eng_HandleWallCollision(atoms, i);
    }

    LOG_FUNC_END(gLogFile);
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
    for (int64_t i = -1; i >= -(int64_t)atoms->space_divisions; i--) {
        atoms->next[i] = i;
        atoms->prev[i] = i;
    }

    const size_t axis_divisions = atoms->axis_divisions;
    if (axis_divisions == 0) {
        return;
    }

    assert(axis_divisions * axis_divisions * axis_divisions == atoms->space_divisions);

    for (size_t n_atom = 0; n_atom < atoms->size; n_atom++) {
        if (atoms->is_out_of_box[n_atom]) {
            continue;
        }

        const size_t list_index = eng_GetCellIndex(atoms, atoms->positions[n_atom]);
        eng_ListPush(atoms, (int64_t)list_index, (int64_t)n_atom);
    }
}
