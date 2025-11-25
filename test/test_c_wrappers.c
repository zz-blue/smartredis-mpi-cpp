/* test_c_wrappers.c
 *
 * Simple test for SmartRedis C wrappers.
 *
 * Expects these symbols provided by your wrappers lib:
 *   create_smartredis_mpi, destroy_smartredis_mpi,
 *   init_smartredis_mpi, finalize_smartredis_mpi,
 *   put_state, put_reward, get_action
 *
 * Build with mpicc and link to your wrappers/libsmartredis libs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "SmartRedisMPI_CWrappers.h" /* put this header in src/c and add -Isrc/c when compiling */

int main(int argc, char **argv)
{
    int mpi_err, rank, nprocs;
    MPI_Comm comm = MPI_COMM_WORLD;

    mpi_err = MPI_Init(&argc, &argv);
    if (mpi_err != MPI_SUCCESS) {
        fprintf(stderr, "MPI_Init failed\n");
        return 1;
    }
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &nprocs);

    if (rank == 0) printf("MPI running on %d ranks\n", nprocs);

    /* Create SmartRedis MPI handle (clustered = 1) */
    SR_HANDLE handle = create_smartredis_mpi(1);
    if (!handle) {
        fprintf(stderr, "create_smartredis_mpi failed\n");
        MPI_Finalize();
        return 2;
    }

    /* Initialize SmartRedis MPI, pass comm = -1 to use MPI_COMM_WORLD inside wrapper */
    if (init_smartredis_mpi(handle, 1, -1) != SR_OK) {
        fprintf(stderr, "init_smartredis_mpi failed\n");
        destroy_smartredis_mpi(handle);
        MPI_Finalize();
        return 3;
    }

    /* Prepare some dummy state/reward data (shape: 3 x N) flattened as 1D array */
    const size_t N = 5;
    const size_t state_rows = 3;
    size_t state_size = state_rows * N;
    double *state = (double*)malloc(sizeof(double) * state_size);
    double *reward = (double*)malloc(sizeof(double) * 2 * N);
    double *action = (double*)malloc(sizeof(double) * 1 * N); /* expect 1 x N action */

    if (!state || !reward || !action) {
        fprintf(stderr, "malloc failed\n");
        finalize_smartredis_mpi(handle);
        destroy_smartredis_mpi(handle);
        MPI_Finalize();
        return 4;
    }

    /* Fill with simple values depending on rank to test multiple ranks */
    for (size_t j = 0; j < N; ++j) {
        state[0*N + j] = 1.0 + rank;         /* hwm_plus */
        state[1*N + j] = 10.0 + j + rank;    /* velh_plus */
        state[2*N + j] = 0.1 * (double)j;    /* dveldz_plus */
        reward[0*N + j] = 0.5 + rank;        /* tauw1 */
        reward[1*N + j] = 0.25 + rank;       /* tauw1_prev */
        action[0*N + j] = 0.0;               /* initialize */
    }

    /* Keys must be null-terminated C strings */
    const char *key_state  = "test_tag.state";
    const char *key_reward = "test_tag.reward";
    const char *key_action = "test_tag.action";

    /* Put state */
    if (put_state(handle, key_state, state, state_size) != SR_OK) {
        fprintf(stderr, "[rank %d] put_state failed\n", rank);
    } else {
        if (rank == 0) printf("put_state succeeded (size=%zu)\n", state_size);
    }

    /* Put reward */
    if (put_reward(handle, key_reward, reward, 2 * N) != SR_OK) {
        fprintf(stderr, "[rank %d] put_reward failed\n", rank);
    } else {
        if (rank == 0) printf("put_reward succeeded (size=%zu)\n", 2 * N);
    }

    /* Get action (synchronous call to server; server should have logic to produce action) */
    if (get_action(handle, key_action, action, 1 * N) != SR_OK) {
        fprintf(stderr, "[rank %d] get_action failed\n", rank);
    } else {
        /* print first few action values on each rank */
        printf("[rank %d] action:", rank);
        for (size_t j = 0; j < N; ++j) {
            printf(" %.6g", action[0*N + j]);
        }
        printf("\n");
    }

    /* cleanup */
    free(state);
    free(reward);
    free(action);

    if (finalize_smartredis_mpi(handle) != SR_OK) {
        fprintf(stderr, "finalize_smartredis_mpi failed\n");
    }

    if (destroy_smartredis_mpi(handle) != SR_OK) {
        fprintf(stderr, "destroy_smartredis_mpi failed\n");
    }

    MPI_Finalize();
    if (rank == 0) printf("Test finished\n");
    return 0;
}
