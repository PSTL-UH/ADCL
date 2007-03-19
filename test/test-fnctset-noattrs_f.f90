        program first

        implicit none
        include 'ADCL.inc'

        integer topo
        integer request
        integer funcs(3), fnctset
        integer NIT, i, rank, size, ierror
        external test_func1, test_func2, test_func3

        NIT = 200

        
        call MPI_Init ( ierror )
        call MPI_Comm_rank ( MPI_COMM_WORLD, rank, ierror)
        call MPI_Comm_size ( MPI_COMM_WORLD, size, ierror )

        call ADCL_Init ( ierror )

        call ADCL_Function_create ( test_func1, ADCL_ATTRSET_NULL, 0, &
             "test_func1", funcs(1), ierror ) 
        call ADCL_Function_create ( test_func2, ADCL_ATTRSET_NULL, 0, &
             "test_func2", funcs(2), ierror ) 
        call ADCL_Function_create ( test_func3, ADCL_ATTRSET_NULL, 0, &
             "test_func3", funcs(3), ierror ) 

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


      subroutine test_func1 ( request ) 

        implicit none
        include 'ADCL.inc'

        integer request
        integer comm, rank, size, ierror

        call ADCL_Request_get_comm ( request, comm, rank, size, ierror )
        write (*,*) rank, ": In test_func1, size = ", size
      end subroutine test_func1

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
