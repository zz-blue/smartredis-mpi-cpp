#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <string>
#include "SmartRedisMPI_CInterface.h"

namespace py = pybind11;

// Helper: convert std::string to char* and length
inline const char* str_data(const std::string& s) { return s.c_str(); }
inline int str_len(const std::string& s) { return static_cast<int>(s.size()); }

PYBIND11_MODULE(pysmartredis, m) {
    m.doc() = "Python wrapper for SmartRedisMPI via C interface";

    // Handle creation/destruction
    m.def("create_handle", [](bool clustered=false) -> uintptr_t {
        SR_HANDLE h = sr_mpi_create(clustered ? 1 : 0);
        if (!h) throw std::runtime_error("Failed to create SmartRedis handle");
        return reinterpret_cast<uintptr_t>(h); // pass as integer
    });

    m.def("destroy_handle", [](uintptr_t handle) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        int code = sr_mpi_destroy(h);
        if (code != 0) throw std::runtime_error("Failed to destroy SmartRedis handle");
    });

    // Initialization / Finalization
    m.def("init_smartredis_mpi", [](uintptr_t handle, bool clustered=false, int comm=-1) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        int code = sr_init(h, clustered ? 1 : 0, comm);
        if (code != 0) throw std::runtime_error("Failed to initialize SmartRedis MPI");
    });

    m.def("finalize_smartredis_mpi", [](uintptr_t handle) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        int code = sr_finalize(h);
        if (code != 0) throw std::runtime_error("Failed to finalize SmartRedis MPI");
    });

    // Data operations
    m.def("put_step_type", [](uintptr_t handle, const std::string& key, int step_type) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        int code = sr_put_step_type(h, str_data(key), str_len(key), step_type);
        if (code != 0) throw std::runtime_error("sr_put_step_type failed");
    });

    m.def("put_state", [](uintptr_t handle, const std::string& key, py::array_t<double> arr) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        py::buffer_info info = arr.request();
        int code = sr_put_state(h, str_data(key), str_len(key),
                                static_cast<double*>(info.ptr), info.size);
        if (code != 0) throw std::runtime_error("sr_put_state failed");
    });

    m.def("put_reward", [](uintptr_t handle, const std::string& key, py::array_t<double> arr) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        py::buffer_info info = arr.request();
        int code = sr_put_reward(h, str_data(key), str_len(key),
                                 static_cast<double*>(info.ptr), info.size);
        if (code != 0) throw std::runtime_error("sr_put_reward failed");
    });

    m.def("get_action", [](uintptr_t handle, const std::string& key, int size) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        std::vector<double> vec(size);
        int code = sr_get_action(h, str_data(key), str_len(key), vec.data(), size);
        if (code != 0) throw std::runtime_error("sr_get_action failed");
        return py::array(vec.size(), vec.data()); // return NumPy array
    });

    m.def("put_info", [](uintptr_t handle, const std::string& key, py::array_t<int> arr) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        py::buffer_info info = arr.request();
        int code = sr_put_info(h, str_data(key), str_len(key),
                               static_cast<int*>(info.ptr), info.size);
        if (code != 0) throw std::runtime_error("sr_put_info failed");
    });

    m.def("put_real_scalar", [](uintptr_t handle, const std::string& key, double r) {
        SR_HANDLE h = reinterpret_cast<SR_HANDLE>(handle);
        int code = sr_put_real_scalar(h, str_data(key), str_len(key), r);
        if (code != 0) throw std::runtime_error("sr_put_real_scalar failed");
    });
}
