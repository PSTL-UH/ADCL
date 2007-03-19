module Matrix
implicit none

  double precision, dimension(:,:), pointer:: colA, colB, colC, C
  
  integer :: ndims, NewComm
  integer, dimension(2)::dims
  logical :: periods(1), reorder(1)
  
  integer:: OneDirection_Row=2, MPI_CART_ERROR = 1
  integer :: nProSize, Nxx

end module Matrix


module InitMatrix
use Matrix
contains

subroutine SetMatrix(nProcs, MyRank)
use Matrix
implicit none

integer, intent(IN) :: nProcs, MyRank
integer :: i,j,dimy

! Read Problme size from Values.conf file
open(unit=101, FILE = "Values.conf", STATUS = "OLD")
88 format (i8)
read(101,88), nProSize
write(*,*), "Problem size =", nProSize

! Allocate memory to matrix
if( mod(nProSize,nProcs) .NE. 0) then
    write(*,*) "Error:Process number must be devided by Problem size"
    return
end if

  Nxx = nProSize/nProcs

  write(*, *) Nxx

  allocate(colA(nProSize, Nxx))
  allocate(colB(nProSize, Nxx))
  allocate(colC(nProSize, Nxx))
  allocate(C(nProSize, nProSize))

do i=1, nProSize, 1

  dimy = mod(MyRank, nProSize)*Nxx

  do j=1, Nxx, 1

    ColC(i,j) = 0.0D0
    ColA(i,j) = (10.0D0*i + dimy+j)/1000.0d0
    ColB(i,j) = (10.0D0*i + dimy+j)/1000.0d0
  end do
end do


end subroutine SetMatrix

end module InitMatrix


module MMultiply
contains

subroutine Multiply(C, A, B, Ny, Nx, RankNumber)
implicit none

integer, intent(IN) :: RankNumber,Ny, Nx
double precision, dimension(Ny,Nx) :: C, B
double precision, dimension(:,:),pointer :: A

integer :: i, j, k

do j=1, Nx, 1
  do k=1, Nx, 1
    do i=1, Ny, 1
      C(i,j)= C(i,j) + A(i,k) * B((RankNumber*Nx+k), j)
    end do
  end do
end do

end subroutine Multiply

end module MMultiply


        program first
        use Matrix
        use InitMatrix

        implicit none
        include 'ADCL.inc'

        integer topo
        integer request
        integer funcs(3), fnctset
        integer NIT, i, rank, size, ierror
		!integer :: NewComm
        external PMatmulSych, test_func2, test_func3

        NIT = 200

        
        call MPI_Init ( ierror )
        call MPI_Comm_rank ( MPI_COMM_WORLD, rank, ierror)
        call MPI_Comm_size ( MPI_COMM_WORLD, size, ierror )

        call ADCL_Init ( ierror )

        call ADCL_Function_create ( PMatmulSych, ADCL_ATTRSET_NULL, 0, &
             "PMatmulSych", funcs(1), ierror ) 
        call ADCL_Function_create ( test_func2, ADCL_ATTRSET_NULL, 0, &
             "test_func2", funcs(2), ierror ) 
        call ADCL_Function_create ( test_func3, ADCL_ATTRSET_NULL, 0, &
             "test_func3", funcs(3), ierror ) 

        call SetMatrix( size, rank )

        ! Set up carterion
        ndims = 2
        dims(1) = 1
        dims(2) = size

        periods(1) = .true.
        reorder(1) = .false.

        call MPI_Cart_create ( MPI_COMM_WORLD, ndims, dims, periods, reorder, NewComm, ierror )


        call ADCL_Fnctset_create ( 3, funcs, "trivial fortran funcs", fnctset, &
             ierror)

        call ADCL_Topology_create_generic ( 0, 0, 0, 0, ADCL_DIRECTION_BOTH, &
             MPI_COMM_WORLD, topo, ierror )

        call ADCL_Request_create_fnctset ( topo, fnctset, request, ierror )

        do i=0, NIT 
           call ADCL_Request_start( request, ierror )
        end do

        call ADCL_Request_free ( request, ierror )
        call ADCL_Topology_free ( topo, ierror )
        call ADCL_Fnctset_free ( fnctset, ierror )

        do i=1, 3
           call ADCL_Function_free ( funcs(i), ierror )
        end do

        call ADCL_Finalize ( ierror )
        call MPI_Finalize ( ierror )
      end program first


      subroutine PMatmulSych ( request ) 
      use Matrix
      use MMultiply
        implicit none
        include 'ADCL.inc'

        integer request
        integer comm, tag, i, rank, size, ierror, TopoType
        integer :: direction, disp, right,left
        integer :: rowreqs(2), rowstatus (MPI_STATUS_SIZE, 2)

        double precision, dimension(nProSize,Nxx),target :: ColTemp
        double precision, dimension(:,:), pointer:: pColA, pColTemp

        call ADCL_Request_get_comm ( request, comm, rank, size, ierror )
        write (*,*) rank, ": In PMatmulSych, size = ", size

        call MPI_Topo_test (comm, TopoType, ierror)

        if(TopoType .ne. MPI_CART) then
          write(*,*) "Error! Communicator topology is not set!"
          ierror = 1
        end if

        call MPI_Cart_shift ( comm, direction, disp, left, right, ierror )


        pColA => ColA
        pColTemp => ColTemp

        tag = 100
        
        do i=size, 1, -1
    
          call MPI_Irecv ( pColTemp(1,1), nProSize*Nxx, MPI_DOUBLE_PRECISION, left, tag, comm, rowreqs(1), ierror )
          call MPI_ISend ( pColA(1,1), nProSize*Nxx, MPI_DOUBLE_PRECISION, right, tag, comm, rowreqs(2),ierror )
          call MPI_Waitall(2, rowreqs, rowstatus, ierror)
          call Multiply(ColC, pColA, ColB, nProSize, Nxx, mod((i+rank),size))

          if ( mod(i,2) .eq. 0 )then 
            pColA => ColTemp
            pColTemp => ColA
          else
         
          pColA => ColA
          pColTemp => ColTemp
          endif
        enddo

        pColA => ColA
        pColTemp => ColTemp

      end subroutine PMatmulSych

      subroutine test_func2 ( request ) 

        implicit none
        include 'ADCL.inc'

        integer request
        integer comm, rank, size, ierror

        call ADCL_Request_get_comm ( request, comm, rank, size, ierror )
        write (*,*) rank, ": In test_func2, size = ", size
      end subroutine test_func2

      subroutine test_func3 ( request ) 

        implicit none
        include 'ADCL.inc'

        integer request
        integer comm, rank, size, ierror

        call ADCL_Request_get_comm ( request, comm, rank, size, ierror )
        write (*,*) rank, ": In test_func3, size = ", size
      end subroutine test_func3
