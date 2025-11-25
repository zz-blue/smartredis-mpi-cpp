#include "SmartRedisMPI_CInterface.h"
#include "SmartRedisMPI.h"
#include <cstring>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <mpi.h>

extern "C" {

/* Opaque handle type */
typedef void* SR_HANDLE;

/* Return codes: 0 success, non-zero failure */
#define SR_OK 0
#define SR_ERR 1

/* Create/destroy */
SR_HANDLE sr_mpi_create(int clustered) {
    try {
        SmartRedisMPI* obj = new SmartRedisMPI(); 
        return static_cast<SR_HANDLE>(obj);
    } catch (...) {
        return nullptr;
    }
}


int sr_mpi_destroy(SR_HANDLE handle) {
    if (!handle) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        delete obj;
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

/* init: pass clustered (0/1) and an integer MPI_Comm (or -1 for MPI_COMM_WORLD) */
int sr_init(SR_HANDLE handle, int clustered, int comm) {
    if (!handle) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        MPI_Comm mpi_comm = MPI_COMM_WORLD;
        if (comm != -1) mpi_comm = (MPI_Comm)comm;
        obj->init_smartredis_mpi(static_cast<bool>(clustered != 0), mpi_comm);
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

int sr_finalize(SR_HANDLE handle) {
    if (!handle) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        obj->finalize_smartredis_mpi();
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

/* Utility to convert string + length from Fortran to std::string */
static std::string fortran_str_to_cpp(const char* s, int len) {
    if (!s || len <= 0) return std::string();
    return std::string(s, static_cast<size_t>(len));
}

/* put_step_type: (key, key_len, step_type) */
int sr_put_step_type(SR_HANDLE handle, const char* key, int key_len, int step_type) {
    if (!handle) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        std::string k = fortran_str_to_cpp(key, key_len);
        obj->put_step_type(k, step_type);
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

/* put_state: (key, key_len, state_ptr, state_size) -- state is double* */
int sr_put_state(SR_HANDLE handle, const char* key, int key_len, const double* state, int state_size) {
    if (!handle) return SR_ERR;
    if (state_size < 0) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        std::string k = fortran_str_to_cpp(key, key_len);
        std::vector<double> vec;
        vec.assign(state, state + state_size);
        obj->put_state(k, vec);
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

/* put_reward: same as put_state */
int sr_put_reward(SR_HANDLE handle, const char* key, int key_len, const double* reward, int reward_size) {
    return sr_put_state(handle, key, key_len, reward, reward_size);
}

/* get_action: (key, key_len, action_ptr, action_size) -- action_ptr is double* writable */
int sr_get_action(SR_HANDLE handle, const char* key, int key_len, double* action, int action_size) {
    if (!handle) return SR_ERR;
    if (action_size < 0) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        std::string k = fortran_str_to_cpp(key, key_len);
        std::vector<double> vec(action_size);
        obj->get_action(k, vec);
        /* copy back */
        if (static_cast<int>(vec.size()) != action_size) {
            /* If sizes mismatch we still copy min */
            int n = std::min<int>(static_cast<int>(vec.size()), action_size);
            for (int i=0;i<n;++i) action[i] = vec[i];
            /* zero remainder */
            for (int i=n;i<action_size;++i) action[i] = 0.0;
        } else {
            std::memcpy(action, vec.data(), sizeof(double)*action_size);
        }
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

/* put_info: integer array */
int sr_put_info(SR_HANDLE handle, const char* key, int key_len, const int* info, int info_size) {
    if (!handle) return SR_ERR;
    if (info_size < 0) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        std::string k = fortran_str_to_cpp(key, key_len);
        std::vector<int> v;
        v.assign(info, info + info_size);
        obj->put_info(k, v);
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

/* put_real_scalar: writes a single double */
int sr_put_real_scalar(SR_HANDLE handle, const char* key, int key_len, double rscalar) {
    if (!handle) return SR_ERR;
    SmartRedisMPI* obj = static_cast<SmartRedisMPI*>(handle);
    try {
        std::string k = fortran_str_to_cpp(key, key_len);
        obj->put_real_scalar(k, rscalar);
        return SR_OK;
    } catch (...) {
        return SR_ERR;
    }
}

} // extern "C"
