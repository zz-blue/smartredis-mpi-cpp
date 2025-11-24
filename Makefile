# Compiler type: GNU or NVIDIA
COMPILER = NVIDIA
# Set SINGLE_PRECISION to 1 to use single precision, otherwise use double precision
SINGLE_PRECISION = 0
# SmartRedis installation directory
SMARTREDIS_INSTALL_DIR = /data/home/scvi558/run/zzy/SmartRedis/install

# Set module flag based on compiler type
ifeq ($(COMPILER),GNU)
  MPIFORT = mpifort
  MPICXX = mpicxx
  MODULE_FLAG = -J
else ifeq ($(COMPILER),NVIDIA)
  # FC = mpifort
  MPIFORT = nvfortran
  MPICXX = mpicxx
  MODULE_FLAG = -module
else
  $(error Invalid COMPILER value. Must be GNU or NVIDIA)
endif

CXXFLAGS := -O2 -fPIC -I./src -I$(SMARTREDIS_INSTALL_DIR)/include \
            -I/data/home/scvi558/.conda/envs/smartflow/include/python3.10 \
            -I/data/home/scvi558/.conda/envs/smartflow/lib/python3.10/site-packages/pybind11/include
CXXFLAGS += --diag_suppress inline_gnu_noinline_conflict
FCFLAGS := -O2 -fPIC -I$(SMARTREDIS_INSTALL_DIR)/include -cpp \
          -I/data/apps/nvhpc/25.3_cuda12.8/Linux_x86_64/25.3/comm_libs/12.8/hpcx/hpcx-2.22.1/ompi/lib

ifeq ($(SINGLE_PRECISION),1)
  FCFLAGS += -D_SINGLE_PRECISION
endif

LDFLAGS := -L$(SMARTREDIS_INSTALL_DIR)/lib \
          -L/data/apps/nvhpc/25.3_cuda12.8/Linux_x86_64/25.3/comm_libs/12.8/hpcx/hpcx-2.22.1/ompi/lib
LIBS := -lsmartredis -lsmartredis-fortran \
        -lmpi_usempif08 -lmpi_usempi_ignore_tkr -lmpi_mpifh -lmpi

# -----------------------
# Directories and files
# -----------------------
BUILD_DIR := build
LIB_DIR := lib
SRC_DIR := src

CPP_SRCS := $(SRC_DIR)/SmartRedisMPI.cpp $(SRC_DIR)/SmartRedisMPI_CInterface.cpp
CPP_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CPP_SRCS))

PY_SRC := $(SRC_DIR)/pysmartredis.cpp
PY_OBJ := $(BUILD_DIR)/pysmartredis.o

F90_SRCS := $(SRC_DIR)/smartredis_mpi.f90
F90_OBJS := $(patsubst $(SRC_DIR)/%.f90,$(BUILD_DIR)/%.o,$(F90_SRCS))
F90_MODS := $(BUILD_DIR)/smartredis_mpi.mod

SHARED_LIB := $(LIB_DIR)/libsmartredis_mpi.so
FORTRAN_LIB := $(LIB_DIR)/libsmartredis_mpi_fortran.a
PY_MODULE := $(LIB_DIR)/pysmartredis.so

# -----------------------
# Default target
# -----------------------
.PHONY: all
all: dirs $(SHARED_LIB) $(FORTRAN_LIB) $(PY_MODULE)

# -----------------------
# Create directories
# -----------------------
.PHONY: dirs
dirs:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(LIB_DIR)

# -----------------------
# Build C++ objects
# -----------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "CXX -> $<"
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

# Build pybind11 object
$(BUILD_DIR)/pysmartredis.o: $(PY_SRC)
	@echo "CXX(py) -> $<"
	$(MPICXX) $(CXXFLAGS) -c $< -o $@

# -----------------------
# Build Fortran objects (module)
# -----------------------
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.f90
	@echo "FORTRAN -> $<"
	$(MPIFORT) $(FCFLAGS) $(MODULE_FLAG) $(BUILD_DIR) -c $< -o $@

# -----------------------
# Link shared C++ library
# -----------------------
$(SHARED_LIB): $(CPP_OBJS)
	@echo "Linking shared library -> $@"
	$(MPICXX) -shared -o $@ $^ $(LDFLAGS) $(LIBS)

# -----------------------
# Create Fortran static library from module objects
# -----------------------
$(FORTRAN_LIB): $(F90_OBJS)
	@echo "Archiving Fortran static lib -> $@"
	ar rcs $@ $^

# -----------------------
# Build Python module
# -----------------------
$(PY_MODULE): $(PY_OBJ) $(BUILD_DIR)/SmartRedisMPI.o $(BUILD_DIR)/SmartRedisMPI_CInterface.o
	@echo "Linking Python module -> $@"
	$(MPICXX) -shared -o $@ $^ $(LDFLAGS) $(LIBS) $(shell $(PYTHON) -m pybind11 --includes)

# -----------------------
# Clean
# -----------------------
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.mod $(LIB_DIR)/*.so $(LIB_DIR)/*.a
