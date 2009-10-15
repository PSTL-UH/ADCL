!
! Copyright (c) 2009           HLRS. All rights reserved.
! $COPYRIGHT$
!
! Additional copyrights may follow
!
! $HEADER$
!
#undef VERBOSE

! unit test for 3D Fortran extended neighborhood communication (for Lattice Boltzmann)
!
! set_data: over all processes, all compute cells are numbered continously in x, y and z direction 
! check_data: determines the "direction" in which to check data
! calc_entry: depending on direction and presence of a neighbor, sets loop bounds and offsets and checks entries
!
! ToDo: for nc > 1, program a special set_data and check_data function
!       at the moment all data(x,y,z,1:nc) are set to the number
 
program testfnctsetextneigh3df
        use auxdata3df
        implicit none

        integer rank, size, ierror
        integer nc, hwidth
        integer vmap, vec, topo, request
        integer cart_comm
        integer, dimension(3) :: dims1, cdims, periods
        integer, dimension(4) :: dims2
        integer, dimension(6) :: neighbors
        !double precision :: data(10,6), data1(12,8), data2(10,6,1), data3(12,8,1), data4(12,8,2)
        double precision, allocatable :: data1(:,:,:), data2(:,:,:,:)
 
        ! integer, parameter :: DIM0=4, DIM1=5, DIM2=6
        integer, parameter :: DIM0=4, DIM1=3, DIM2=2

        cdims = 0
        periods = 0

        ! Initiate the MPI environment
        call MPI_Init ( ierror )
        call MPI_Comm_rank ( MPI_COMM_WORLD, rank, ierror)
        call MPI_Comm_size ( MPI_COMM_WORLD, size, ierror )

        ! Describe the neighborhood relations
        call MPI_Dims_create ( size, 3, cdims, ierror)
        call MPI_Cart_create ( MPI_COMM_WORLD, 3, cdims, periods, 0, cart_comm, ierror )
        call MPI_Cart_shift ( cart_comm, 0, 1, neighbors(1), neighbors(2), ierror )
        call MPI_Cart_shift ( cart_comm, 1, 1, neighbors(3), neighbors(4), ierror )
        call MPI_Cart_shift ( cart_comm, 2, 1, neighbors(5), neighbors(6), ierror );

        ! Initiate the ADCL library and register a topology object with ADCL
        call ADCL_Init ( ierror )
        call ADCL_Topology_create_extended ( cart_comm, topo, ierror )

        ! **********************************************************************
        ! Test 1: hwidth=1, nc=0
        !dims1(1) = 10
        !dims1(2) = 6
        hwidth = 1
        nc = 0
        dims1(1) = DIM0 + 2*hwidth;
        dims1(2) = DIM1 + 2*hwidth;
        dims1(3) = DIM2 + 2*hwidth;

        allocate ( data1(dims1(1),dims1(2), dims1(3)) )
        call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
        if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"
        call adcl_vector_register_generic ( 3,  dims1, nc, vmap, MPI_DOUBLE_PRECISION, data1, vec, ierror )
        call ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, request, ierror )

        call set_data_3D_cont( data1, rank, dims1, hwidth, cart_comm )
#ifdef VERBOSE
        call dump_vector_3D_mpi_dp ( data1, dims1, cart_comm )
#endif
        call ADCL_Request_start( request, ierror )

#ifdef VERBOSE
        call dump_vector_3D_mpi_dp ( data1, dims1, cart_comm )
#endif
        call check_data_3D_cont ( data1, rank, dims1, hwidth, neighbors, cart_comm )

        deallocate ( data1 )

        call ADCL_Request_free ( request, ierror )
        call ADCL_Vector_deregister ( vec, ierror )
        call ADCL_Vmap_free ( vmap, ierror )

        ! **********************************************************************
        ! Test 2: hwidth=2, nc=0
        hwidth = 2
        nc = 0
        dims1(1) = DIM0 + 2*hwidth;
        dims1(2) = DIM1 + 2*hwidth;
        dims1(3) = DIM2 + 2*hwidth;

        allocate ( data1(dims1(1),dims1(2), dims1(3)) )
        call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
        if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"
        call adcl_vector_register_generic ( 3,  dims1, nc, vmap, MPI_DOUBLE_PRECISION, data1, vec, ierror )
        call ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, request, ierror )

        call set_data_3D_cont( data1, rank, dims1, hwidth, cart_comm )

#ifdef VERBOSE
        call dump_vector_3D_mpi_dp ( data1, dims1, cart_comm )
#endif
        call ADCL_Request_start( request, ierror )

#ifdef VERBOSE
        call dump_vector_3D_mpi_dp ( data1, dims1, cart_comm )
#endif
        call check_data_3D_cont ( data1, rank, dims1, hwidth, neighbors, cart_comm )

        deallocate ( data1 )

        call ADCL_Request_free ( request, ierror )
        call ADCL_Vector_deregister ( vec, ierror )
        call ADCL_Vmap_free ( vmap, ierror )

        ! **********************************************************************
        ! Test 3: hwidth=1, nc=1
        hwidth = 1
        nc = 1
        dims2(1) = DIM0 + 2*hwidth;
        dims2(2) = DIM1 + 2*hwidth;
        dims2(3) = DIM2 + 2*hwidth;
        dims2(4) = nc

        allocate ( data2(dims2(1),dims2(2), dims2(3), dims2(4)) )
        call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
        if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"
        call adcl_vector_register_generic ( 3,  dims2(1:3), nc, vmap, MPI_DOUBLE_PRECISION, data2, vec, ierror )
        call ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, request, ierror )

        call set_data_4D( data2, rank, dims2(1:3), hwidth, nc, cart_comm )

#ifdef VERBOSE
        call dump_vector_4D_mpi_dp ( data2, dims2, cart_comm )
#endif
        call ADCL_Request_start( request, ierror )

#ifdef VERBOSE
        call dump_vector_4D_mpi_dp ( data2, dims2, cart_comm )
#endif
        call check_data_4D ( data2, rank, dims2(1:3), hwidth, nc, neighbors, cart_comm )

        deallocate ( data2 )

        call ADCL_Request_free ( request, ierror )
        call ADCL_Vector_deregister ( vec, ierror )
        call ADCL_Vmap_free ( vmap, ierror )

        ! **********************************************************************
        ! Test 4: hwidth=2, nc=1
        hwidth = 2
        nc = 1
        dims2(1) = DIM0 + 2*hwidth;
        dims2(2) = DIM1 + 2*hwidth;
        dims2(3) = DIM2 + 2*hwidth;
        dims2(4) = nc

        allocate ( data2(dims2(1),dims2(2), dims2(3), dims2(4)) )
        call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
        if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"
        call adcl_vector_register_generic ( 3,  dims2(1:3), nc, vmap, MPI_DOUBLE_PRECISION, data2, vec, ierror )
        call ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, request, ierror )

        call set_data_4D( data2, rank, dims2(1:3), hwidth, nc, cart_comm )

#ifdef VERBOSE
        call dump_vector_4D_mpi_dp ( data2, dims2, cart_comm )
#endif
        call ADCL_Request_start( request, ierror )

#ifdef VERBOSE
        call dump_vector_4D_mpi_dp ( data2, dims2, cart_comm )
#endif
        call check_data_4D ( data2, rank, dims2(1:3), hwidth, nc, neighbors, cart_comm )

        deallocate ( data2 )

        call ADCL_Request_free ( request, ierror )
        call ADCL_Vector_deregister ( vec, ierror )
        call ADCL_Vmap_free ( vmap, ierror )


        ! **********************************************************************
        ! Test 5: hwidth=2, nc=2
        hwidth = 2
        nc = 1
        dims2(1) = DIM0 + 2*hwidth;
        dims2(2) = DIM1 + 2*hwidth;
        dims2(3) = DIM2 + 2*hwidth;
        dims2(4) = nc

        allocate ( data2(dims2(1),dims2(2), dims2(3), dims2(4)) )
        call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
        if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"
        call adcl_vector_register_generic ( 3,  dims2(1:3), nc, vmap, MPI_DOUBLE_PRECISION, data2, vec, ierror )
        call ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, request, ierror )

        call set_data_4D( data2, rank, dims2(1:3), hwidth, nc, cart_comm )

#ifdef VERBOSE
        call dump_vector_4D_mpi_dp ( data2, dims2, cart_comm )
#endif
        call ADCL_Request_start( request, ierror )

#ifdef VERBOSE
        call dump_vector_4D_mpi_dp ( data2, dims2, cart_comm )
#endif
        call check_data_4D ( data2, rank, dims2(1:3), hwidth, nc, neighbors, cart_comm )

        deallocate ( data2 )

        call ADCL_Request_free ( request, ierror )
        call ADCL_Vector_deregister ( vec, ierror )
        call ADCL_Vmap_free ( vmap, ierror )

        ! **********************************************************************

!!.......done
        call ADCL_Topology_free ( topo, ierror )
        call MPI_Comm_free ( cart_comm, ierror )

        call ADCL_Finalize ( ierror )
        call MPI_Finalize ( ierror )


contains

! ****************************************************************************

subroutine set_data_4D ( data, rank, dims, hwidth, nc, cart_comm)

    implicit none
    include 'ADCL.inc'

    integer, intent(in) :: rank, hwidth, nc, cart_comm
    integer, dimension(3), intent(in) :: dims
    double precision, intent(inout) :: data(dims(1),dims(2),dims(3),nc)

    integer :: i, j, k, l, ierr
    integer, dimension(3) :: coords, cart_dims, period, dims_wo_halos

    call MPI_Cart_get(cart_comm, 3, cart_dims, period, coords, ierr)
    ! cube: cart_dims with index coords
    ! inside cube: dims with index (i,j,k)

    do l = 1, nc
        do j=1, dims(2)
           do i=1, dims(1)
              do k=1, hwidth
                 data(i,j,k,l) = -1
              end do 
              do k=dims(3)-hwidth+1, dims(3)
                 data(i,j,k,l) = -1
              end do 
           end do
       end do
   end do

   do l = 1, nc
      do k=1, dims(3)
         do i=1, dims(1)
            do j=1, hwidth
               data(i,j,k,l) = -1
           end do
           do j=dims(2)-hwidth+1, dims(2)
               data(i,j,k,l) = -1
            end do
         end do
      end do
   end do

   do l = 1, nc
      do k=1, dims(3)
         do j=1, dims(2)
            do i=1, hwidth
               data(i,j,k,l) = -1
            end do
            do i=dims(1)-hwidth+1, dims(1)
               data(i,j,k,l) = -1
            end do
         end do
      end do
   end do

    !be aware of the change in i and j in the equation compare to the c code. (i-1) and (j-1)
    ! without hwidth: 
    !    in x direction offset dims(1)*coords(1) + i
    !    in y direction offset dims(1)*cart_dims(1) * ( dims(2)*coords(2) + j-1 )
    !    in z direction offset dims(1)*cart_dims(1) * dims(2)*cart_dims(2) * ( dims(3)*coords(3) + z-1 )
    dims_wo_halos = dims - 2*hwidth 

    do l = 1, nc
       do k=hwidth+1, dims(3)-hwidth
          do j=hwidth+1, dims(2)-hwidth
             do  i=hwidth+1, dims(1)-hwidth
                 !print *, "rank =", rank, ", coords = ( ", coords, " )"
                 data(i,j,k,l) = i-hwidth + dims_wo_halos(1)*coords(1) +                                                   & 
                               dims_wo_halos(1)*cart_dims(1) * ( ( dims_wo_halos(2)*coords(2) + j-hwidth-1 ) +             & 
                                           dims_wo_halos(2)*cart_dims(2) * ( dims_wo_halos(3)*coords(3) + k-hwidth-1 ) )
            end do 
         end do  
      end do
   end do

   return
end subroutine set_data_4D

!****************************************************************************
subroutine check_data_4D ( data, rank, dims, hwidth, nc, neighbors, cart_comm )
   use  auxdata3df
   implicit none

   integer, intent(in) :: rank, hwidth, nc, cart_comm
   integer, dimension(3), intent(in) :: dims
   integer, dimension(6), intent(in) :: neighbors
   double precision, intent(in) :: data(dims(1),dims(2), dims(3), nc)

   integer :: i, j, k, l, ierr, lres=1, gres, prod
   integer, dimension(3) :: coords, n_coords, c_coords, cart_size, period
   double precision should_be

   integer :: x_direction, y_direction, z_direction

   ! check for each of the 27 possible locations
   do z_direction = -1, 1
        do y_direction = -1, 1
            do x_direction = -1, 1
                do l = 1, nc
                   prod = x_direction * y_direction * z_direction
#ifdef INCCORNER
                   if ( prod .ne. 0 ) then
                       ! corner
                       lres = check_region_3D (x_direction, y_direction, z_direction, data(:,:,:,l), rank, & 
                              cart_comm, dims, hwidth, neighbors)
                   endif 
#endif
                   if ( prod == 0) then
                       ! edge, face or inside
                       lres = check_region_3D (x_direction, y_direction, z_direction, data(:,:,:,l), rank, & 
                              cart_comm, dims, hwidth, neighbors )
                   end if
                end do
            end do
        end do
    end do

    call MPI_Allreduce ( lres, gres, 1, MPI_INTEGER, MPI_MIN, MPI_COMM_WORLD, ierror )

    if ( gres .eq. 1 ) then
        if ( rank == 0 )  then
            write(*,'(1x,a,i0,a,i0,a)') "3-D C testsuite: hwidth = ", hwidth, ", nc = ", nc, " passed"
        end if
    else 
        if ( rank == 0 ) then
            write(*,'(1x,a,i0,a,i0,a)') "3-D C testsuite: hwidth = ", hwidth, ", nc = ", nc, " failed"
        end if
        call dump_vector_4D_dp ( data, rank, dims );
    endif

    return

end subroutine check_data_4D

end program testfnctsetextneigh3df
