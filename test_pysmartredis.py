import numpy as np
import pysmartredis

def main():
    # -----------------------------
    # 1. Create handle
    # -----------------------------
    handle = pysmartredis.create_handle(clustered=False)
    print(f"Handle created: {handle}")

    # -----------------------------
    # 2. Initialize SmartRedis MPI
    # -----------------------------
    pysmartredis.init_smartredis_mpi(handle, clustered=False)
    print("SmartRedis MPI initialized")

    # -----------------------------
    # 3. Put step type
    # -----------------------------
    pysmartredis.put_step_type(handle, "step_001", 1)
    print("Step type set")

    # -----------------------------
    # 4. Put state
    # -----------------------------
    state = np.array([0.1, 0.2, 0.3, 0.4], dtype=np.float64)
    pysmartredis.put_state(handle, "state_001", state)
    print("State written:", state)

    # -----------------------------
    # 5. Put reward
    # -----------------------------
    reward = np.array([1.0, 0.0, -1.0], dtype=np.float64)
    pysmartredis.put_reward(handle, "reward_001", reward)
    print("Reward written:", reward)

    # -----------------------------
    # 6. Get action
    # -----------------------------
    action_size = 3
    action = pysmartredis.get_action(handle, "action_001", action_size)
    print("Action retrieved:", action)

    # -----------------------------
    # 7. Put info
    # -----------------------------
    info = np.array([42, 7, 13], dtype=np.int32)
    pysmartredis.put_info(handle, "info_001", info)
    print("Info written:", info)

    # -----------------------------
    # 8. Put real scalar
    # -----------------------------
    pysmartredis.put_real_scalar(handle, "scalar_001", 3.14159)
    print("Real scalar written")

    # -----------------------------
    # 9. Finalize and destroy
    # -----------------------------
    pysmartredis.finalize_smartredis_mpi(handle)
    pysmartredis.destroy_handle(handle)
    print("SmartRedis MPI finalized and handle destroyed")


if __name__ == "__main__":
    main()
