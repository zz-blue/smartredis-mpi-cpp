# mpirun -n 4 python test_pysmartredis.py

import numpy as np
from mpi4py import MPI
import sys
import os

os.environ["SSDB"] = "tcp://127.0.0.1:6379"

sys.path.append(os.path.abspath('../lib'))

import pysmartredis

def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    nprocs = comm.Get_size()

    handle = pysmartredis.create_handle(clustered=False)
    print(f"[Rank {rank}] Handle created: {handle}")

    pysmartredis.init_smartredis_mpi(handle, clustered=False)
    print(f"[Rank {rank}] Initialized SmartRedisMPI")

    # Put a state array
    state = np.full(5, rank*1.0)
    pysmartredis.put_state(handle, f"state_{rank}", state)
    print(f"[Rank {rank}] put_state done: {state}")

    pysmartredis.finalize_smartredis_mpi(handle)
    print(f"[Rank {rank}] Finalized SmartRedisMPI")

    pysmartredis.destroy_handle(handle)
    print(f"[Rank {rank}] Handle destroyed")

if __name__ == "__main__":
    main()
