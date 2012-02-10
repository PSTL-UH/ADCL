!
! Copyright (c) 2009          HLRS. All rights reserved.
! $COPYRIGHT$
!
! Additional copyrights may follow
!
! $HEADER$
!
program test_timerobject_tworeq

!   Test program for timer object
!   ==============================================
!   2D neighborhood communication, two requests with two different emethods
!

   implicit none
   include 'ADCL.inc'


   integer rank, size, ierror, it, maxit
   integer nc, hwidth
   integer vmap, vec1, vec2, topo, req1, req2
   integer, allocatable :: reqs(:)
   integer cart_comm
   integer, dimension(2) :: dims, cdims, periods
   integer, dimension(4) :: neighbors
   integer, parameter :: DIM0=10, DIM1=6
   double precision :: data1(DIM0,DIM1)
   integer          :: data2(DIM0,DIM1)
   ! Timer 
   integer timer;

   cdims(1)   = 0
   cdims(2)   = 0
   periods(1) = 0
   periods(2) = 0

   ! Initiate the MPI environment
   call MPI_Init ( ierror )
   call MPI_Comm_rank ( MPI_COMM_WORLD, rank, ierror)
   call MPI_Comm_size ( MPI_COMM_WORLD, size, ierror )
   if (size .eq. 1) then 
      print *, "This is a MPI program, start with more than one processor"
      stop
   end if

   ! Describe the neighborhood relations
   call MPI_Dims_create ( size, 2, cdims, ierror)
   call MPI_Cart_create ( MPI_COMM_WORLD, 2, cdims, periods, 0,  &
                          cart_comm, ierror )
   call MPI_Cart_shift ( cart_comm, 0, 1, neighbors(1), neighbors(2), ierror )
   call MPI_Cart_shift ( cart_comm, 1, 1, neighbors(3), neighbors(4), ierror )

   ! Initiate the ADCL library and register a topology object with ADCL
   call ADCL_Init ( ierror )
   call ADCL_Topology_create ( cart_comm, topo, ierror )
   

   dims(1) = DIM0 
   dims(2) = DIM1 
   nc     = 0
   hwidth = 1
   call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
   if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"

   ! allocate vector and request no. 1
   call adcl_vector_register_generic ( 2,  dims, nc, vmap, MPI_DOUBLE_PRECISION, & 
                               data1, vec1, ierror )
   if ( ADCL_SUCCESS .ne. ierror) print *, "vector_register not successful"
   call ADCL_Request_create ( vec1, topo, ADCL_FNCTSET_NEIGHBORHOOD, &
        req1, ierror )

   ! allocate vector and request no. 2
   call adcl_vector_register_generic ( 2,  dims, nc, vmap, MPI_INTEGER, & 
                               data2, vec2, ierror )
   if ( ADCL_SUCCESS .ne. ierror) print *, "vector_register not successful"
   call ADCL_Request_create ( vec2, topo, ADCL_FNCTSET_NEIGHBORHOOD, &
        req2, ierror )


   ! define timer object
   allocate ( reqs(2) )
   reqs(1) = req1
   reqs(2) = req2
   call ADCL_Timer_create ( 2, reqs, timer, ierror )
   deallocate ( reqs )

   maxit = 400
   do it = 1, maxit
      call set_data_2D_dp  ( data1, rank, dims, hwidth ) 
      call set_data_2D_int ( data2, rank, dims, hwidth ) 
#ifdef VERBOSE
      call dump_vector_2D_dp  ( data1, rank, dims )
      call dump_vector_2D_int ( data2, rank, dims )
#endif

      call ADCL_Timer_start ( timer, ierror )
      call ADCL_Request_start ( req1, ierror )
      call ADCL_Request_start ( req2, ierror )
      call ADCL_Timer_stop ( timer, ierror );

      call check_data_2D_dp  ( data1, rank, dims, hwidth, neighbors )
      call check_data_2D_int ( data2, rank, dims, hwidth, neighbors )
   end do

   call ADCL_Timer_free ( timer, ierror )
   call ADCL_Request_free ( req1, ierror )
   call ADCL_Request_free ( req2, ierror )
   call ADCL_Vector_deregister ( vec1, ierror )
   call ADCL_Vector_deregister ( vec2, ierror )
   call ADCL_Vmap_free ( vmap, ierror )

   ! done
   call ADCL_Topology_free ( topo, ierror )
   call MPI_Comm_free ( cart_comm, ierror )

   call ADCL_Finalize ( ierror )
   call MPI_Finalize ( ierror )

end program test_timerobject_tworeq
