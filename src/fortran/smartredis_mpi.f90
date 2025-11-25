module smartredis_mpi
  use iso_c_binding
  use mpi
  implicit none

  private
  public :: init_smartredis_mpi, finalize_smartredis_mpi, &
            put_step_type, put_state, put_reward, get_action, &
            put_info, put_real_scalar, create_handle, destroy_handle

  type(c_ptr) :: global_handle = c_null_ptr
  integer :: nprocs = 1, myid = 0, ierr = 0
  integer(kind=C_INT) :: mpi_comm_local = 0

  interface
    function sr_mpi_create(clustered) bind(C, name="sr_mpi_create")
      import :: C_INT, C_PTR
      integer(C_INT), value :: clustered
      type(C_PTR) :: sr_mpi_create
    end function

    function sr_mpi_destroy(handle) bind(C, name="sr_mpi_destroy")
      import :: C_PTR, C_INT
      type(C_PTR), value :: handle
      integer(C_INT) :: sr_mpi_destroy
    end function

    function sr_init(handle, clustered, comm) bind(C, name="sr_init")
      import :: C_PTR, C_INT
      type(C_PTR), value :: handle
      integer(C_INT), value :: clustered
      integer(C_INT), value :: comm
      integer(C_INT) :: sr_init
    end function

    function sr_finalize(handle) bind(C, name="sr_finalize")
      import :: C_PTR, C_INT
      type(C_PTR), value :: handle
      integer(C_INT) :: sr_finalize
    end function

    function sr_put_step_type(handle, key, key_len, step_type) bind(C, name="sr_put_step_type")
      import :: C_PTR, C_INT, C_CHAR
      type(C_PTR), value :: handle
      character(kind=C_CHAR), dimension(*) :: key
      integer(C_INT), value :: key_len
      integer(C_INT), value :: step_type
      integer(C_INT) :: sr_put_step_type
    end function

    function sr_put_state(handle, key, key_len, state, state_size) bind(C, name="sr_put_state")
      import :: C_PTR, C_INT, C_DOUBLE, C_CHAR
      type(C_PTR), value :: handle
      character(kind=C_CHAR), dimension(*) :: key
      integer(C_INT), value :: key_len
      real(C_DOUBLE), dimension(*) :: state
      integer(C_INT), value :: state_size
      integer(C_INT) :: sr_put_state
    end function

    function sr_put_reward(handle, key, key_len, reward, reward_size) bind(C, name="sr_put_reward")
      import :: C_PTR, C_INT, C_DOUBLE, C_CHAR
      type(C_PTR), value :: handle
      character(kind=C_CHAR), dimension(*) :: key
      integer(C_INT), value :: key_len
      real(C_DOUBLE), dimension(*) :: reward
      integer(C_INT), value :: reward_size
      integer(C_INT) :: sr_put_reward
    end function

    function sr_get_action(handle, key, key_len, action, action_size) bind(C, name="sr_get_action")
      import :: C_PTR, C_INT, C_DOUBLE, C_CHAR
      type(C_PTR), value :: handle
      character(kind=C_CHAR), dimension(*) :: key
      integer(C_INT), value :: key_len
      real(C_DOUBLE), dimension(*) :: action
      integer(C_INT), value :: action_size
      integer(C_INT) :: sr_get_action
    end function

    function sr_put_info(handle, key, key_len, info, info_size) bind(C, name="sr_put_info")
      import :: C_PTR, C_INT, C_CHAR
      type(C_PTR), value :: handle
      character(kind=C_CHAR), dimension(*) :: key
      integer(C_INT), value :: key_len
      integer(C_INT), dimension(*) :: info
      integer(C_INT), value :: info_size
      integer(C_INT) :: sr_put_info
    end function

    function sr_put_real_scalar(handle, key, key_len, rscalar) bind(C, name="sr_put_real_scalar")
      import :: C_PTR, C_INT, C_DOUBLE, C_CHAR
      type(C_PTR), value :: handle
      character(kind=C_CHAR), dimension(*) :: key
      integer(C_INT), value :: key_len
      real(C_DOUBLE), value :: rscalar
      integer(C_INT) :: sr_put_real_scalar
    end function
  end interface

contains

  pure function key_length(key) result(len_key)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    integer :: len_key, i
    len_key = 0
    do i = 1, size(key)
      if (key(i) == C_NULL_CHAR) exit
      len_key = len_key + 1
    end do
  end function key_length

  function create_handle(clustered) result(res)
    integer, intent(in), optional :: clustered
    type(c_ptr) :: res
    integer(C_INT) :: ccluster
    if (present(clustered)) then
      ccluster = clustered
    else
      ccluster = 1
    end if
    res = sr_mpi_create(ccluster)
    if (c_associated(res)) then
      global_handle = res
    end if
  end function create_handle

  function destroy_handle() result(code)
    integer :: code
    if (.not. c_associated(global_handle)) then
      code = 1
      return
    end if
    code = sr_mpi_destroy(global_handle)
    if (code == 0) global_handle = c_null_ptr
  end function destroy_handle

  subroutine init_smartredis_mpi(db_clustered, comm)
    logical, intent(in), optional :: db_clustered
    integer, intent(in), optional :: comm
    integer(C_INT) :: code, ccluster, ccomm

    if (.not. c_associated(global_handle)) then
      global_handle = sr_mpi_create(1)
      if (.not. c_associated(global_handle)) then
        stop 'Failed to create SmartRedis handle'
      end if
    end if

    if (present(db_clustered)) then
      ccluster = merge(1,0,db_clustered)
    else
      ccluster = 1
    end if

    if (present(comm)) then
      ccomm = comm
    else
      ccomm = -1
    end if

    code = sr_init(global_handle, ccluster, ccomm)
    if (code /= 0) stop 'sr_init failed'

    call MPI_Comm_rank(MPI_COMM_WORLD, myid, ierr)
    call MPI_Comm_size(MPI_COMM_WORLD, nprocs, ierr)
  end subroutine init_smartredis_mpi

  subroutine finalize_smartredis_mpi()
    integer :: code
    if (.not. c_associated(global_handle)) return
    code = sr_finalize(global_handle)
    if (code /= 0) stop 'sr_finalize failed'
    code = sr_mpi_destroy(global_handle)
    global_handle = c_null_ptr
  end subroutine finalize_smartredis_mpi

  subroutine put_step_type(key, step_type)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    integer, intent(in) :: step_type
    integer(C_INT) :: code
    code = sr_put_step_type(global_handle, key, key_length(key), step_type)
    if (code /= 0) stop 'sr_put_step_type failed'
  end subroutine put_step_type

  subroutine put_state(key, dims, state)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    integer, intent(in), dimension(:) :: dims
    real(C_DOUBLE), intent(in), dimension(product(dims)) :: state
    integer(C_INT) :: code
    code = sr_put_state(global_handle, key, key_length(key), state, size(state))
    if (code /= 0) stop 'sr_put_state failed'
  end subroutine put_state

  subroutine put_reward(key, dims, reward)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    integer, intent(in), dimension(:) :: dims
    real(C_DOUBLE), intent(in), dimension(product(dims)) :: reward
    integer(C_INT) :: code
    code = sr_put_reward(global_handle, key, key_length(key), reward, size(reward))
    if (code /= 0) stop 'sr_put_reward failed'
  end subroutine put_reward

  subroutine get_action(key, dims, action)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    integer, intent(in), dimension(:) :: dims
    real(C_DOUBLE), intent(out), dimension(product(dims)) :: action
    integer(C_INT) :: code
    code = sr_get_action(global_handle, key, key_length(key), action, size(action))
    if (code /= 0) stop 'sr_get_action failed'
  end subroutine get_action

  subroutine put_info(key, dims, info)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    integer, intent(in), dimension(:) :: dims
    integer(C_INT), intent(in), dimension(product(dims)) :: info
    integer(C_INT) :: code
    code = sr_put_info(global_handle, key, key_length(key), info, size(info))
    if (code /= 0) stop 'sr_put_info failed'
  end subroutine put_info

  subroutine put_real_scalar(key, rscalar)
    character(kind=C_CHAR), intent(in), dimension(:) :: key
    real(C_DOUBLE), intent(in) :: rscalar
    integer(C_INT) :: code
    code = sr_put_real_scalar(global_handle, key, key_length(key), rscalar)
    if (code /= 0) stop 'sr_put_real_scalar failed'
  end subroutine put_real_scalar

end module smartredis_mpi
