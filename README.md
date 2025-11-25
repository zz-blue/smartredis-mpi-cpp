# SmartRedisMPI Framework

## Overview

The **SmartRedisMPI framework** provides a high-performance interface to **SmartRedis** in an **MPI environment**, enabling seamless integration with HPC applications. The framework is designed with a **C++ core**, wrapped with a **C interface** to allow easy usage from **Fortran** and **Python**.

### Framework Components

* **SmartRedisMPI C++ Core** (`SmartRedisMPI.cpp` / `.h`)
  Handles all communication with SmartRedis and MPI, including data storage and retrieval operations.

* **C Interface** (`SmartRedisMPI_CInterface.cpp` / `.h`)
  Provides a simple, portable API to bridge the C++ core with Fortran and Python.

* **Fortran Interface** (`smartredis_mpi.f90`)
  Fortran bindings for HPC simulations, allowing the framework to be used in legacy or high-performance Fortran codebases.

* **Python Wrapper** (`pysmartredis.cpp`)
  Provides Python bindings using `pybind11` to access SmartRedisMPI from Python scripts.

* **Test Programs**
  Example scripts in Fortran and Python demonstrate initialization, data put/get operations, and cleanup.

---

## Features

* MPI-aware: Supports distributed memory parallelism.
* Multi-language support: Works natively with **C++**, **Fortran**, and **Python**.
* Data operations: Supports storing/retrieving states, actions, rewards, info arrays, and scalars.
* Easy to integrate: Minimal dependencies beyond MPI and SmartRedis.

---

## Installation

1. **Prerequisites**:

   * MPI (e.g., OpenMPI, MVAPICH2, or HPC-X MPI)
   * SmartRedis library
   * C++ compiler with C++11 support
   * For Python: `pybind11`, compatible Python version

2. **Build**:

   ```bash
   make
   ```

   This compiles the C++ core, C interface, Fortran module, and Python wrapper.

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

---

## Notes

* The C interface is primarily for interoperability; direct usage in C++ is also supported.
* MPI handles must be initialized before calling any put/get operations.
* Python scripts require the compiled `pysmartredis` module in the Python path.
