program test_smartredis_mpi
  use iso_c_binding
  use mpi
  use smartredis_mpi
  implicit none

  integer :: ierr, myid, nprocs
  integer :: dims(1)
  real(C_DOUBLE), dimension(5) :: state, reward, action
  integer :: i

  ! C_CHAR 一维数组作为 key
  character(kind=C_CHAR), dimension(:), allocatable :: key_state, key_reward, key_action

  ! MPI 初始化
  call MPI_Init(ierr)
  call MPI_Comm_rank(MPI_COMM_WORLD, myid, ierr)
  call MPI_Comm_size(MPI_COMM_WORLD, nprocs, ierr)

  ! 初始化 SmartRedis MPI
  call init_smartredis_mpi(.true.)

  ! 分配 key 并初始化
  allocate(key_state(5))
  allocate(key_reward(6))
  allocate(key_action(6))

  key_state  = [ 's'C, 't'C, 'a'C, 't'C, 'e'C ]
  key_reward = [ 'r'C, 'e'C, 'w'C, 'a'C, 'r'C, 'd'C ]
  key_action = [ 'a'C, 'c'C, 't'C, 'i'C, 'o'C, 'n'C ]

  ! 初始化数据
  do i = 1, 5
     state(i) = real(myid + 1, C_DOUBLE)
     reward(i) = real(myid + 1, C_DOUBLE)
     action(i) = 0.0_C_DOUBLE
  end do
  dims(1) = 5

  ! 调用接口
  call put_state(key_state, dims, state)
  call put_reward(key_reward, dims, reward)
  call get_action(key_action, dims, action)

  print *, "Rank ", myid, " action = ", action

  call finalize_smartredis_mpi()
  call MPI_Finalize(ierr)
end program test_smartredis_mpi
