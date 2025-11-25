#ifndef SMARTREDIS_MPI_H
#define SMARTREDIS_MPI_H

#include <vector>
#include <string>
#include <mpi.h>
#include "client.h"

class SmartRedisMPI {
public:
    SmartRedisMPI(bool clustered=false, MPI_Comm comm=MPI_COMM_WORLD);
    ~SmartRedisMPI();

    void init_smartredis_mpi(bool clustered=false, MPI_Comm comm=MPI_COMM_WORLD);
    void finalize_smartredis_mpi();

    void put_step_type(const std::string &key, int step_type);
    void put_state(const std::string &key, const std::vector<double> &state);
    void put_reward(const std::string &key, const std::vector<double> &reward);
    void get_action(const std::string &key, std::vector<double> &action);
    void put_info(const std::string &key, const std::vector<int> &info);
    void put_real_scalar(const std::string &key, const double &rscalar);

    int get_rank() const { return myid; }
    int get_nprocs() const { return nprocs; }

private:
    MPI_Comm mpi_comm_local;
    int myid;
    int nprocs;
    SmartRedis::Client* client;
};

#endif
