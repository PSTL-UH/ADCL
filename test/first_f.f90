        program first

        implicit none
        include 'ADCL.inc'

        integer i, rank, size, ierror, NIT, allocstat
        integer dims(3), cdims(3), periods(3)
        integer vec;
        integer request;
        integer cart_comm;
        integer, allocatable, dimension (:,:,:) :: data
        
        NIT = 100

        dims(1) = 66
        dims(2) = 34
        dims(3) = 34
        cdims(1) = 0
        cdims(2) = 0
        cdims(3) = 0
        periods(1) = 0
        periods(2) = 0
        periods(3) = 0
        
        call MPI_Init ( ierror )
        call MPI_Comm_rank ( MPI_COMM_WORLD, rank, ierror)
        call MPI_Comm_size ( MPI_COMM_WORLD, size, ierror )

        allocate ( data (0:dims(1)-1, 0:dims(2)-1, 0:dims(3)-1), &
             stat = allocstat ) 
        if ( allocstat.gt.0 ) then 
           write (*,*) rank, ' : Error allocating memory'
           call MPI_Abort ( MPI_COMM_WORLD, 1, ierror )
        end if

        call ADCL_Init ( ierror )
        call ADCL_Vector_register ( 3, dims, 0, 1, MPI_DOUBLE_PRECISION,&
                                    data, vec, ierror)

        call MPI_Dims_create ( size, 3, cdims, ierror)
        call MPI_Cart_create ( MPI_COMM_WORLD, 3, cdims, periods, 0,  &
                               cart_comm, ierror )

        call ADCL_Request_create ( vec, cart_comm, request, ierror )

        do i=0, NIT 
           call ADCL_Request_start( request, ierror )
        end do

        call ADCL_Request_free ( request, ierror );
        call ADCL_Vector_deregister ( vec, ierror );
        call MPI_Comm_free ( cart_comm, ierror );
    
        deallocate ( data, stat=allocstat)

        call ADCL_Finalize ( ierror );
        call MPI_Finalize ( ierror );
      end program first
