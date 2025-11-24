#pragma once
#include <mpi.h>
#include <vector>
#include <string>
#include "client.h"  // SmartRedis::Client

class SmartRedisMPI {
public:
    SmartRedisMPI(bool clustered, MPI_Comm comm = MPI_COMM_WORLD);
    ~SmartRedisMPI();

    void init_smartredis_mpi(bool db_clustered, MPI_Comm comm = MPI_COMM_NULL);
    void finalize_smartredis_mpi();

    void put_step_type(const std::string &key, int step_type);
    void put_state(const std::string &key, const std::vector<double> &state);
    void put_reward(const std::string &key, const std::vector<double> &reward);
    void get_action(const std::string &key, std::vector<double> &action);
    void put_info(const std::string &key, const std::vector<int> &info);
    void put_real_scalar(const std::string &key, const double &rscalar);

private:
    MPI_Comm mpi_comm;
    MPI_Comm mpi_comm_local;
    int myid, nprocs;
    SmartRedis::Client *client = nullptr;
};
