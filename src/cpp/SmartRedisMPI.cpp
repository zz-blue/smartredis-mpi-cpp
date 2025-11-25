#include "SmartRedisMPI.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>

SmartRedisMPI::SmartRedisMPI(bool clustered, MPI_Comm comm)
: mpi_comm_local(comm), client(nullptr)
{
    MPI_Comm_rank(mpi_comm_local, &myid);
    MPI_Comm_size(mpi_comm_local, &nprocs);

    if (myid == 0) {
        try {
            client = new SmartRedis::Client(clustered, "default");
        } catch (const std::exception &e) {
            std::cerr << "SmartRedis client creation failed: " << e.what() << std::endl;
            throw;
        }
    }
}

SmartRedisMPI::~SmartRedisMPI() {
    if (myid == 0 && client) {
        delete client;
        client = nullptr;
    }
}

void SmartRedisMPI::init_smartredis_mpi(bool clustered, MPI_Comm comm) {
    mpi_comm_local = (comm != MPI_COMM_NULL) ? comm : MPI_COMM_WORLD;
    MPI_Comm_rank(mpi_comm_local, &myid);
    MPI_Comm_size(mpi_comm_local, &nprocs);

    if (myid == 0 && !client) {
        client = new SmartRedis::Client(clustered, "default");
    }
}

void SmartRedisMPI::finalize_smartredis_mpi() {
    if (myid == 0 && client) {
        delete client;
        client = nullptr;
    }
}

// === Helper for rank0-only operations ===
#define RANK0_ONLY if(myid != 0) return;

// === Data operations ===

void SmartRedisMPI::put_step_type(const std::string &key, int step_type) {
    RANK0_ONLY
    int32_t v = step_type;
    client->put_tensor(key, &v, {1}, SRTensorTypeInt32, SRMemLayoutContiguous);
}

void SmartRedisMPI::put_state(const std::string &key, const std::vector<double> &state) {
    int size_local = static_cast<int>(state.size());
    std::vector<int> sizes(nprocs,0);
    MPI_Gather(&size_local, 1, MPI_INT, sizes.data(), 1, MPI_INT, 0, mpi_comm_local);

    std::vector<int> displs(nprocs,0);
    int total_size = 0;
    if(myid==0){
        for(int i=0;i<nprocs;i++){
            displs[i] = total_size;
            total_size += sizes[i];
        }
    }

    std::vector<double> global(total_size,0.0);
    MPI_Gatherv(state.data(), size_local, MPI_DOUBLE,
                myid==0 ? global.data() : nullptr,
                myid==0 ? sizes.data() : nullptr,
                myid==0 ? displs.data() : nullptr,
                MPI_DOUBLE, 0, mpi_comm_local);

    RANK0_ONLY
    client->put_tensor(key, global.data(), {static_cast<size_t>(total_size)}, SRTensorTypeDouble, SRMemLayoutContiguous);
}

void SmartRedisMPI::put_reward(const std::string &key, const std::vector<double> &reward) {
    put_state(key, reward);  // same pattern
}

void SmartRedisMPI::get_action(const std::string &key, std::vector<double> &action) {
    int size_local = static_cast<int>(action.size());
    std::vector<int> sizes(nprocs,0);
    MPI_Gather(&size_local,1,MPI_INT,sizes.data(),1,MPI_INT,0,mpi_comm_local);

    std::vector<int> displs(nprocs,0);
    int total_size=0;
    if(myid==0){
        for(int i=0;i<nprocs;i++){
            displs[i] = total_size;
            total_size += sizes[i];
        }
    }

    std::vector<double> global(total_size,0.0);
    RANK0_ONLY
    client->unpack_tensor(key, global.data(), {static_cast<size_t>(total_size)}, SRTensorTypeDouble, SRMemLayoutContiguous);
    client->delete_tensor(key);

    MPI_Scatterv(myid==0 ? global.data() : nullptr,
                 myid==0 ? sizes.data() : nullptr,
                 myid==0 ? displs.data() : nullptr,
                 MPI_DOUBLE,
                 action.data(),
                 size_local,
                 MPI_DOUBLE,
                 0,
                 mpi_comm_local);
}

void SmartRedisMPI::put_info(const std::string &key, const std::vector<int> &info) {
    int size_local = static_cast<int>(info.size());
    std::vector<int> sizes(nprocs,0);
    MPI_Gather(&size_local,1,MPI_INT,sizes.data(),1,MPI_INT,0,mpi_comm_local);

    std::vector<int> displs(nprocs,0);
    int total_size=0;
    if(myid==0){
        for(int i=0;i<nprocs;i++){
            displs[i] = total_size;
            total_size += sizes[i];
        }
    }

    std::vector<int> global(total_size,0);
    MPI_Gatherv(info.data(), size_local, MPI_INT,
                myid==0 ? global.data() : nullptr,
                myid==0 ? sizes.data() : nullptr,
                myid==0 ? displs.data() : nullptr,
                MPI_INT,0,mpi_comm_local);

    RANK0_ONLY
    client->put_tensor(key, global.data(), {static_cast<size_t>(total_size)}, SRTensorTypeInt32, SRMemLayoutContiguous);
}

void SmartRedisMPI::put_real_scalar(const std::string &key, const double &rscalar) {
    RANK0_ONLY
    client->put_tensor(key, &rscalar, {1}, SRTensorTypeDouble, SRMemLayoutContiguous);
}
