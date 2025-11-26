# SmartRedisMPI-CPP

## Overview

**SmartRedisMPI-CPP** is a high-performance C++ framework providing **MPI-aware access** to **SmartRedis**, designed for integration in HPC applications.  
It features a **C++ core**, a **C interface**, and wrappers for **Fortran** and **Python**, allowing seamless multi-language usage.

---

## Framework Structure

```

SmartRedisMPI-CPP
│
├─ C++ Core: SmartRedisMPI.cpp / SmartRedisMPI.h
│    Handles communication with SmartRedis and MPI.
│    Builds: libsmartredis_core.a
│
├─ C Interface: SmartRedisMPI_CInterface.cpp / SmartRedisMPI_CInterface.h
│    Bridges C++ core to C API.
│    Builds: libsmartredis_cinterface.a
│
├─ Fortran Wrapper: smartredis_mpi.f90
│    Provides Fortran bindings.
│    Builds: libsmartredis_mpi.a
│
├─ Python Wrapper: pysmartredis.cpp
│    Provides Python bindings using pybind11.
│    Builds: pysmartredis.so
│
├─ C Wrappers: SmartRedisMPI_CWrappers.cpp
│    Additional C wrappers (optional).
│    Builds: libsmartredis_cwrappers.a
│
└─ Test Programs
Example Fortran and Python scripts for verification.

````

## Build Instructions

1. **Set paths in Makefile** 
2. **Build all components**:
```bash
make
````

* Produces static libraries:

  * `libsmartredis_core.a`
  * `libsmartredis_cinterface.a`
  * `libsmartredis_mpi.a` (Fortran)
  * `libsmartredis_cwrappers.a` (C)
* Produces Python shared object:

  * `pysmartredis.so`

---

## Usage

### Fortran Example

```fortran
program test_smartredis
  use smartredis_mpi
  implicit none

  call init_smartredis_mpi(.true.)
  call put_real_scalar("test_scalar", 3.14)
  call finalize_smartredis_mpi()
end program test_smartredis
```

### Python Example

```python
import pysmartredis

handle = pysmartredis.init_smartredis_mpi(True)
pysmartredis.put_real_scalar("test_scalar", 3.14)
pysmartredis.finalize_smartredis_mpi()
```

### C++ Example

```c++
#include "SmartRedisMPI.h"

int main() {
    SmartRedisMPI::init(true);
    SmartRedisMPI::put_real_scalar("test_scalar", 3.14);
    SmartRedisMPI::finalize();
    return 0;
}
```

---

## Integration in HPC Projects

When using in a project like **CaLES-smartflow**, link the following libraries in order:

```text
-lsmartredis_mpi -lsmartredis_cinterface -lsmartredis_core
```

* Ensure proper **C++ compiler ABI compatibility**.
* For Fortran interoperability, pass strings as `cchar` using `iso_c_binding`.
* Python bindings require `pysmartredis.so` in `PYTHONPATH`.

