#include "SmartRedisMPI.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring> 

SmartRedisMPI::SmartRedisMPI(bool clustered, MPI_Comm comm)
{
    mpi_comm = comm;
    MPI_Comm_rank(mpi_comm, &myid);
    MPI_Comm_size(mpi_comm, &nprocs);

    if (myid == 0) {
        client = new SmartRedis::Client(clustered, "default");
    } else {
        client = nullptr;
    }
}

SmartRedisMPI::~SmartRedisMPI()
{
    if (myid == 0 && client) {
        delete client;
        client = nullptr;
    }
}

void SmartRedisMPI::init_smartredis_mpi(bool db_clustered, MPI_Comm comm)
{
    mpi_comm_local = (comm != MPI_COMM_NULL) ? comm : MPI_COMM_WORLD;
    MPI_Comm_rank(mpi_comm_local, &myid);
    MPI_Comm_size(mpi_comm_local, &nprocs);

    if (myid == 0) {
        client = new SmartRedis::Client(db_clustered, "default");
    }
}

void SmartRedisMPI::finalize_smartredis_mpi()
{
    if (myid == 0 && client) {
        delete client;
        client = nullptr;
    }
}

void SmartRedisMPI::put_step_type(const std::string &key, int step_type)
{
    if (myid != 0) return;
    if (!client) throw std::runtime_error("SmartRedis client not initialized on rank 0");

    int32_t v = static_cast<int32_t>(step_type);
    std::vector<size_t> dims = {1};
    client->put_tensor(key, (const void*)&v, dims,
                       SRTensorTypeInt32,
                       SRMemLayoutContiguous);
}

void SmartRedisMPI::put_state(const std::string &key, const std::vector<double> &state)
{
    int state_size = static_cast<int>(state.size());
    std::vector<int> state_sizes(nprocs);

    MPI_Gather(&state_size, 1, MPI_INT, state_sizes.data(), 1, MPI_INT, 0, mpi_comm_local);

    std::vector<int> state_displs(nprocs, 0);
    int state_global_size = 0;
    if (myid == 0) {
        for (int i = 0; i < nprocs; ++i) {
            state_displs[i] = state_global_size;
            state_global_size += state_sizes[i];
        }
    }

    std::vector<double> state_global;
    if (myid == 0) state_global.resize(state_global_size);

    MPI_Gatherv(state.data(), state_size, MPI_DOUBLE,
                (myid == 0 ? state_global.data() : nullptr),
                (myid == 0 ? state_sizes.data() : nullptr),
                (myid == 0 ? state_displs.data() : nullptr),
                MPI_DOUBLE, 0, mpi_comm_local);

    if (myid == 0) {
        std::vector<size_t> dims = { static_cast<size_t>(state_global_size) };
        client->put_tensor(key, (const void*)state_global.data(), dims,
                           SRTensorTypeDouble,
                           SRMemLayoutContiguous);
    }
}

void SmartRedisMPI::put_reward(const std::string &key, const std::vector<double> &reward)
{
    int reward_size = static_cast<int>(reward.size());
    std::vector<int> reward_sizes(nprocs);

    MPI_Gather(&reward_size, 1, MPI_INT, reward_sizes.data(), 1, MPI_INT, 0, mpi_comm_local);

    std::vector<int> reward_displs(nprocs, 0);
    int reward_global_size = 0;
    if (myid == 0) {
        for (int i = 0; i < nprocs; ++i) {
            reward_displs[i] = reward_global_size;
            reward_global_size += reward_sizes[i];
        }
    }

    std::vector<double> reward_global;
    if (myid == 0) reward_global.resize(reward_global_size);

    MPI_Gatherv(reward.data(), reward_size, MPI_DOUBLE,
                (myid == 0 ? reward_global.data() : nullptr),
                (myid == 0 ? reward_sizes.data() : nullptr),
                (myid == 0 ? reward_displs.data() : nullptr),
                MPI_DOUBLE, 0, mpi_comm_local);

    if (myid == 0) {
        std::vector<size_t> dims = { static_cast<size_t>(reward_global_size) };
        client->put_tensor(key, (const void*)reward_global.data(), dims,
                           SRTensorTypeDouble,
                           SRMemLayoutContiguous);
    }
}

void SmartRedisMPI::get_action(const std::string &key, std::vector<double> &action)
{
    int action_size = static_cast<int>(action.size());
    std::vector<int> action_sizes(nprocs);

    MPI_Gather(&action_size, 1, MPI_INT, action_sizes.data(), 1, MPI_INT, 0, mpi_comm_local);

    std::vector<int> action_displs(nprocs, 0);
    int action_global_size = 0;
    if (myid == 0) {
        for (int i = 0; i < nprocs; ++i) {
            action_displs[i] = action_global_size;
            action_global_size += action_sizes[i];
        }
    }

    std::vector<double> action_global;
    if (myid == 0) action_global.resize(action_global_size);

    if (myid == 0) {
        client->unpack_tensor(key, (void*)action_global.data(), {static_cast<size_t>(action_global_size)},
                              SRTensorTypeDouble,
                              SRMemLayoutContiguous);

        client->delete_tensor(key);
    }

    MPI_Scatterv((myid == 0 ? action_global.data() : nullptr),
                 (myid == 0 ? action_sizes.data() : nullptr),
                 (myid == 0 ? action_displs.data() : nullptr),
                 MPI_DOUBLE,
                 action.data(), action_size, MPI_DOUBLE,
                 0, mpi_comm_local);
}

void SmartRedisMPI::put_info(const std::string &key, const std::vector<int> &info)
{
    int info_size = static_cast<int>(info.size());
    std::vector<int> info_sizes(nprocs);

    MPI_Gather(&info_size, 1, MPI_INT, info_sizes.data(), 1, MPI_INT, 0, mpi_comm_local);

    std::vector<int> info_displs(nprocs, 0);
    int info_global_size = 0;
    if (myid == 0) {
        for (int i = 0; i < nprocs; ++i) {
            info_displs[i] = info_global_size;
            info_global_size += info_sizes[i];
        }
    }

    std::vector<int> info_global;
    if (myid == 0) info_global.resize(info_global_size);

    MPI_Gatherv(info.data(), info_size, MPI_INT,
                (myid == 0 ? info_global.data() : nullptr),
                (myid == 0 ? info_sizes.data() : nullptr),
                (myid == 0 ? info_displs.data() : nullptr),
                MPI_INT, 0, mpi_comm_local);

    if (myid == 0) {
        client->put_tensor(key, (const void*)info_global.data(), {static_cast<size_t>(info_global_size)},
                           SRTensorTypeInt32,
                           SRMemLayoutContiguous);
    }
}

void SmartRedisMPI::put_real_scalar(const std::string &key, const double &rscalar)
{
    if (myid != 0) return;

    client->put_tensor(key, (const void*)&rscalar, {1},
                       SRTensorTypeDouble,
                       SRMemLayoutContiguous);
}
