#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <string>
#include "SmartRedisMPI_CInterface.h"

namespace py = pybind11;

inline const char* str_data(const std::string &s) { return s.c_str(); }
inline int str_len(const std::string &s) { return static_cast<int>(s.size()); }

PYBIND11_MODULE(pysmartredis, m) {
    m.doc() = "Python wrapper for SmartRedisMPI via C interface";

    m.def("create_handle", [](bool clustered=false){
        SR_HANDLE h = sr_mpi_create(clustered?1:0);
        if(!h) throw std::runtime_error("Failed to create handle");
        return reinterpret_cast<uintptr_t>(h);
    }, py::arg("clustered")=false);

    m.def("destroy_handle", [](uintptr_t h){
        SR_HANDLE handle = reinterpret_cast<SR_HANDLE>(h);
        if(sr_mpi_destroy(handle)!=0)
            throw std::runtime_error("Failed to destroy handle");
    });

    m.def("init_smartredis_mpi", [](uintptr_t h, bool clustered=false, int comm=-1){
        SR_HANDLE handle = reinterpret_cast<SR_HANDLE>(h);
        if(sr_init(handle, clustered?1:0, comm)!=0)
            throw std::runtime_error("Failed to init MPI");
    }, py::arg("h"), py::arg("clustered")=false, py::arg("comm")=-1);

    m.def("finalize_smartredis_mpi", [](uintptr_t h){
        SR_HANDLE handle = reinterpret_cast<SR_HANDLE>(h);
        if(sr_finalize(handle)!=0)
            throw std::runtime_error("Failed to finalize MPI");
    });

    // put_state example
    m.def("put_state", [](uintptr_t h, const std::string &key, py::array_t<double> arr){
        SR_HANDLE handle = reinterpret_cast<SR_HANDLE>(h);
        py::buffer_info info = arr.request();
        if(sr_put_state(handle, str_data(key), str_len(key), static_cast<double*>(info.ptr), info.size)!=0)
            throw std::runtime_error("put_state failed");
    });
}
