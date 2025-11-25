#ifndef SMARTREDISMPI_CINTERFACE_H
#define SMARTREDISMPI_CINTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle type */
typedef void* SR_HANDLE;

/* Return codes: 0 success, non-zero failure */
#define SR_OK 0
#define SR_ERR 1

/* Create / Destroy */
SR_HANDLE sr_mpi_create(int clustered);
int sr_mpi_destroy(SR_HANDLE handle);

/* Initialize / Finalize MPI */
int sr_init(SR_HANDLE handle, int clustered, int comm);
int sr_finalize(SR_HANDLE handle);

/* Data operations */
int sr_put_step_type(SR_HANDLE handle, const char* key, int key_len, int step_type);
int sr_put_state(SR_HANDLE handle, const char* key, int key_len, const double* state, int state_size);
int sr_put_reward(SR_HANDLE handle, const char* key, int key_len, const double* reward, int reward_size);
int sr_get_action(SR_HANDLE handle, const char* key, int key_len, double* action, int action_size);
int sr_put_info(SR_HANDLE handle, const char* key, int key_len, const int* info, int info_size);
int sr_put_real_scalar(SR_HANDLE handle, const char* key, int key_len, double rscalar);

#ifdef __cplusplus
}
#endif

#endif // SMARTREDISMPI_CINTERFACE_H
