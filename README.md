# SmartRedisMPI Framework

## Overview

The **SmartRedisMPI framework** provides a high-performance interface to **SmartRedis** in an **MPI environment**, enabling seamless integration with HPC applications. The framework is built around a **C++ core** and wrapped with **C, Fortran, and Python interfaces**, allowing easy usage across multiple languages.

---

## Framework Components

* **SmartRedisMPI C++ Core (`SmartRedisMPI.cpp` / `.h`)**
  Handles all communication with SmartRedis and MPI, including data storage and retrieval.

* **C Interface (`SmartRedisMPI_CInterface.cpp` / `.h`)**
  Provides a **simple and portable API** that bridges the C++ core with **C, Fortran, and Python** wrappers, enabling multi-language interoperability.

* **Wrappers** (for multi-language support)

  * **C Wrappers (`SmartRedisMPI_CWrappers.cpp`)**
    Provides a **pure C API** that can be used directly in C programs or as a bridge for other languages requiring a standard C interface.
  * **Fortran Wrappers (`smartredis_mpi.f90`)**
    Provides Fortran bindings for HPC simulations, enabling use in legacy or high-performance Fortran codebases.

    > **Note:** Functionality is largely consistent with previous versions. The interface uses **`cchar`** data types, so arguments may require type conversion when calling the functions.
  * **Python Wrapper (`pysmartredis.cpp`)**
    Provides Python bindings using **pybind11**.

* **Test Programs**
  Example scripts in C, Fortran, and Python demonstrate initialization, data put/get operations, and cleanup.

---

## Features

* MPI-aware: Supports distributed memory parallelism.
* Multi-language support: Works natively with **C++**, **C**, **Fortran**, and **Python**.
* Data operations: Supports storing/retrieving states, actions, rewards, info arrays, and scalars.
* Easy integration: Minimal dependencies beyond MPI and SmartRedis.

---

## Build

1. **Modify Makefile**: Update environment paths (MPI, SmartRedis, Python includes/libraries) as needed.

2. **Compile**:

```bash
make
```

This builds the **C++ core**, **C Interface**, **C/Fortran/Python wrappers**, and test programs.

---

## Usage Examples

### C Example (via C Wrappers)

```c
#include "SmartRedisMPI_CWrappers.h"
#include <stdio.h>

int main() {
    SmartRedisMPI_Init(1); // verbose
    SmartRedisMPI_PutRealScalar("test_scalar", 3.14);
    SmartRedisMPI_Finalize();
    printf("C API test completed.\n");
    return 0;
}
```

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

