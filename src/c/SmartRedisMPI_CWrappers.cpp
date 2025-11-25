#include "SmartRedisMPI_CWrappers.h"
#include "SmartRedisMPI_CInterface.h"
#include <cstring>   // strlen

extern "C" {

/* Create/destroy wrap sr_mpi_create / sr_mpi_destroy */
SR_HANDLE create_smartredis_mpi(int clustered) {
    return sr_mpi_create(clustered);
}

int destroy_smartredis_mpi(SR_HANDLE handle) {
    return sr_mpi_destroy(handle);
}

/* init/finalize wrap sr_init / sr_finalize
   comm: -1 => MPI_COMM_WORLD forwarded as -1 in sr_init */
int init_smartredis_mpi(SR_HANDLE handle, int clustered, int comm) {
    return sr_init(handle, clustered, comm);
}

int finalize_smartredis_mpi(SR_HANDLE handle) {
    return sr_finalize(handle);
}

/* put_state / put_reward / get_action wrappers that forward to sr_* */
int put_state(SR_HANDLE handle, const char* key, const double* state, size_t n) {
    if (!key) return SR_ERR;
    return sr_put_state(handle, key, (int)std::strlen(key), state, (int)n);
}

int put_reward(SR_HANDLE handle, const char* key, const double* reward, size_t n) {
    if (!key) return SR_ERR;
    return sr_put_reward(handle, key, (int)std::strlen(key), reward, (int)n);
}

int get_action(SR_HANDLE handle, const char* key, double* action, size_t n) {
    if (!key) return SR_ERR;
    return sr_get_action(handle, key, (int)std::strlen(key), action, (int)n);
}

/* optional forwards for other helpers */
int put_step_type(SR_HANDLE handle, const char* key, int step_type) {
    if (!key) return SR_ERR;
    return sr_put_step_type(handle, key, (int)std::strlen(key), step_type);
}

int put_info(SR_HANDLE handle, const char* key, const int* info, size_t n) {
    if (!key) return SR_ERR;
    return sr_put_info(handle, key, (int)std::strlen(key), info, (int)n);
}

int put_real_scalar(SR_HANDLE handle, const char* key, double value) {
    if (!key) return SR_ERR;
    return sr_put_real_scalar(handle, key, (int)std::strlen(key), value);
}

} // extern "C"
