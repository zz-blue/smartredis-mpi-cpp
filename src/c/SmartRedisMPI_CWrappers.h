#ifndef SMARTREDISMPI_CWRAPPERS_H
#define SMARTREDISMPI_CWRAPPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

/* Opaque handle */
typedef void* SR_HANDLE;

/* Return codes */
#define SR_OK 0
#define SR_ERR 1

SR_HANDLE create_smartredis_mpi(int clustered);
int       destroy_smartredis_mpi(SR_HANDLE handle);

int init_smartredis_mpi(SR_HANDLE handle, int clustered, int comm);
int finalize_smartredis_mpi(SR_HANDLE handle);
int put_state(SR_HANDLE handle, const char* key, const double* state, size_t n);
int put_reward(SR_HANDLE handle, const char* key, const double* reward, size_t n);
int get_action(SR_HANDLE handle, const char* key, double* action, size_t n);

int put_step_type(SR_HANDLE handle, const char* key, int step_type);
int put_info(SR_HANDLE handle, const char* key, const int* info, size_t n);
int put_real_scalar(SR_HANDLE handle, const char* key, double value);

#ifdef __cplusplus
}
#endif

#endif /* SMARTREDISMPI_CWRAPPERS_H */
