!
! Copyright (c) 2009           HLRS. All rights reserved.
! $COPYRIGHT$
!
! Additional copyrights may follow
!
! $HEADER$
!
#undef VERBOSE

program testfnctsetextneigh3df

        implicit none
        include 'ADCL.inc'

        integer rank, size, ierror
        integer nc, hwidth
        integer vmap, vec, topo, request
        integer cart_comm
        integer, dimension(3) :: dims, cdims, periods
        integer, dimension(6) :: neighbors
        !double precision :: data(10,6), data1(12,8), data2(10,6,1), data3(12,8,1), data4(12,8,2)
        double precision, dimension(:,:,:), allocatable :: data1
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
        !dims(1) = 10
        !dims(2) = 6
        hwidth = 1
        nc = 0
        dims(1) = DIM0 + 2*hwidth;
        dims(2) = DIM1 + 2*hwidth;
        dims(3) = DIM2 + 2*hwidth;

        allocate ( data1(dims(1),dims(2), dims(3)) )
        call adcl_vmap_halo_allocate( hwidth, vmap, ierror )
        if ( ADCL_SUCCESS .ne. ierror) print *, "vmap_halo_allocate not successful"
        call adcl_vector_register_generic ( 3,  dims, nc, vmap, MPI_DOUBLE_PRECISION, data1, vec, ierror )
        call ADCL_Request_create ( vec, topo, ADCL_FNCTSET_NEIGHBORHOOD, request, ierror )

        call set_data_3D( data1, rank, dims, hwidth, cart_comm )
#ifdef VERBOSE
        call dump_vector_3D_mpi_dp ( data1, dims, cart_comm )
#endif
        call ADCL_Request_start( request, ierror )

#ifdef VERBOSE
        call dump_vector_3D_mpi_dp ( data1, dims, cart_comm )
#endif
        call check_data_3D ( data1, rank, dims, hwidth, neighbors, cart_comm )

        deallocate ( data1 )

        call ADCL_Request_free ( request, ierror )
        call ADCL_Vector_deregister ( vec, ierror )
        call ADCL_Vmap_free ( vmap, ierror )

        ! **********************************************************************
        ! Test 2 to 5 missing
        ! **********************************************************************

!!.......done
        call ADCL_Topology_free ( topo, ierror )
        call MPI_Comm_free ( cart_comm, ierror )

        call ADCL_Finalize ( ierror )
        call MPI_Finalize ( ierror )


contains

! ****************************************************************************

subroutine set_data_3D ( data, rank, dims, hwidth, cart_comm)

    implicit none
    include 'ADCL.inc'

    integer, intent(in) :: rank, hwidth, cart_comm
    integer :: i, j, k, ierr
    integer, dimension(3), intent(in) :: dims
    integer, dimension(3) :: coords, cart_dims, period, dims_wo_halos
    double precision, intent(inout) :: data(dims(1),dims(2),dims(3))

    call MPI_Cart_get(cart_comm, 3, cart_dims, period, coords, ierr)
    ! cube: cart_dims with index coords
    ! inside cube: dims with index (i,j,k)

    do i=1, dims(1)
        do j=1, dims(2)
            do k=1, hwidth
                data(i,j,k) = -1
            end do
            do k=dims(3)-hwidth+1, dims(3)
                data(i,j,k) = -1
           end do
       end do
   end do

    do i=1, dims(1)
        do k=1, dims(3)
            do j=1, hwidth
                data(i,j,k) = -1
           end do
            do j=dims(2)-hwidth+1, dims(2)
                data(i,j,k) = -1
           end do
       end do
   end do

    do j=1, dims(2)
        do k=1, dims(3)
            do i=1, hwidth
                data(i,j,k) = -1
           end do
            do i=dims(1)-hwidth+1, dims(1)
                data(i,j,k) = -1
           end do
       end do
   end do

    !be aware of the change in i and j in the equation compare to the c code. (i-1) and (j-1)
    ! without hwidth: 
    !    in x direction offset dims(1)*coords(1) + i
    !    in y direction offset dims(1)*cart_dims(1) * ( dims(2)*coords(2) + j-1 )
    !    in z direction offset dims(1)*cart_dims(1) * dims(2)*cart_dims(2) * ( dims(3)*coords(3) + z-1 )
    dims_wo_halos = dims - 2*hwidth 

    do  i=hwidth+1, dims(1)-hwidth
        do j=hwidth+1, dims(2)-hwidth
            do k=hwidth+1, dims(3)-hwidth
                !print *, "rank =", rank, ", coords = ( ", coords, " )"
                data(i,j,k) = i-hwidth + dims_wo_halos(1)*coords(1) +                                                     & 
                              dims_wo_halos(1)*cart_dims(1) * ( ( dims_wo_halos(2)*coords(2) + j-hwidth-1 ) +             & 
                                             dims_wo_halos(2)*cart_dims(2) * ( dims_wo_halos(3)*coords(3) + k-hwidth-1 ) )
           end do  
       end do
    end do

    return
end subroutine set_data_3D

!****************************************************************************
      subroutine dump_vector_3D ( data, rank, dims, nc )

        implicit none
        include 'ADCL.inc'

        integer rank, dims(2), nc
        double precision data(dims(1), dims(2), nc)
        integer i, j

        if ( nc .le. 1 ) then
           do j = 1, dims(1)
              write (*,*) rank, (data(j,i,1), i=1,dims(2))
           end do
        else if ( nc .eq. 2 ) then 
           do j = 1, dims(1)
              write (*,*) rank, (data(j,i,1), data(j,i,2),  i=1,dims(2))
           end do
        endif

        return
      end subroutine dump_vector_3D


subroutine check_data_3D ( data, rank, dims, hwidth, neighbors, cart_comm )

   implicit none
   include 'ADCL.inc'

   integer, intent(in) :: rank, hwidth, cart_comm
   integer, dimension(3), intent(in) :: dims
   integer, dimension(6), intent(in) :: neighbors
   double precision, intent(in) :: data(dims(1),dims(2), dims(3))

   integer :: i, j, k, ierr, lres=1, gres
   integer, dimension(3) :: coords, n_coords, c_coords, cart_size, period
   double precision should_be

   integer control_x, control_y, control_z

   ! check for each of the 27 possible locations
   do control_x = -1, 1
        do control_y = -1, 1
            do control_z = -1, 1
#ifdef INCCORNER
                if ( control_x * control_y * control_z .ne. 0 ) then
                    ! corner
                    lres = calc_entry3D ( control_x, control_y, control_z, data, rank, cart_comm, dims, hwidth, neighbors)
                endif 
#endif
                if ( control_x * control_y * control_z == 0) then
                    ! edge, face or inside
                    lres = calc_entry3D ( control_x, control_y, control_z, data, rank, cart_comm, dims, hwidth, neighbors )
                end if
            end do
        end do
    end do

    call MPI_Allreduce ( lres, gres, 1, MPI_INTEGER, MPI_MIN, MPI_COMM_WORLD, ierror )

    if ( gres .eq. 1 ) then
        if ( rank == 0 )  then
            print *, "3-D C testsuite: hwidth = ", hwidth, ", nc = 0 passed"
        end if
    else 
        if ( rank == 0 ) then
            print *, "3-D C testsuite: hwidth = ", hwidth, ", nc = 0 failed"
        end if
        call dump_vector_3D_dp ( data, rank, dims );
    endif

    return

end subroutine check_data_3D


function calc_entry3D ( control_x, control_y, control_z, data, rank, cart_comm, dims, hwidth, neighbors) result(lres)

    integer :: control_x, control_y, control_z, cart_comm
    integer :: dims(3)                          ! size of one cube
    integer :: hwidth, neighbors(6), rank
    double precision :: data (dims(1), dims(2), dims(3) )
    integer :: lres

    integer :: i, j, k, ierr
    double precision :: should_be
    integer :: cart_dims(3), dims_wo_halos(3)
    integer :: coords(3), n_coords(3), c_coords(3) ! coords, coords of neighbor and corrected coords of MPI process
    integer :: period(3)
    integer :: compensate(3)  ! what do I have to add / substract to my coordinate on the 
                              ! neighboring process to compare the values in the halo cell and 
                              ! in the domain of the neighboring process
    integer :: loopstart(3), loopend(3)  ! defines part of array to check
    logical :: neighbor_cond(3)            ! is there a neighbor in x,y,z-direction?

    lres = 1

    call MPI_Cart_get (cart_comm, 3, cart_dims, period, coords, ierr)
    neighbor_cond = .false.

    select case (control_x)
       case (0)
            loopstart(1)     = hwidth+1
            loopend(1)       = dims(1)-hwidth
            compensate(1)    = -hwidth
            c_coords(1)      = coords(1)
            neighbor_cond(1) = .true.
       case (-1)
            loopstart(1) = 1
            loopend(1) = hwidth
            compensate(1) = dims(1) - 2*hwidth -1 
            if (neighbors(1) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(1), 3, n_coords)
                c_coords(1) = n_coords(1)
                neighbor_cond(1) = .true.
            endif
       case (1)
            loopstart(1)     = dims(1) - hwidth + 1
            loopend(1)       = dims(1) 
            compensate(1)    = - dims(1) + 2*hwidth -1
            if (neighbors(2) .ne. MPI_PROC_NULL ) then 
                call MPI_Cart_coords (cart_comm, neighbors(2), 3, n_coords)
                c_coords(1) = n_coords(1)
                neighbor_cond(1) = .true.
            endif
    end select

    select case (control_y)
        case (0)
            loopstart(2) = hwidth+1
            loopend(2) = dims(2) - hwidth
            compensate(2) = -hwidth 
            c_coords(2) = coords(2)
            neighbor_cond(2) = .true.
        case (-1)
            loopstart(2) = 1
            loopend(2)   = hwidth
            compensate(2) = dims(2) - 2*hwidth -1 
            if (neighbors(3) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(3), 3, n_coords)
                c_coords(2) = n_coords(2)
                neighbor_cond(2) = .true.
            endif
        case (1)
            loopstart(2)  = dims(2) - hwidth + 1 
            loopend(2)    = dims(2)
            compensate(2) = - dims(2) + 2*hwidth -1 
            if (neighbors(4) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(4), 3, n_coords)
                c_coords(2) = n_coords(2)
                neighbor_cond(2) = .true.
            endif
    end select

    select case (control_z)
        case (0)
            loopstart(3) = hwidth + 1
            loopend(3)   = dims(3) - hwidth
            compensate(3) = -hwidth 
            c_coords(3) = coords(3)
            neighbor_cond(3) = .true.
        case (-1)
            loopstart(3) = 1
            loopend(3)   = hwidth
            compensate(3) = dims(3) - 2*hwidth -1  
            if (neighbors(5) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(5), 3, n_coords)
                c_coords(3) = n_coords(3)
                neighbor_cond(3) = .true.
            endif
        case (1)
            loopstart(3)  = dims(3) - hwidth + 1 
            loopend(3)    = dims(3) 
            compensate(3) = - dims(3) + 2*hwidth -1
            if (neighbors(6) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(6), 3, n_coords)
                c_coords(3) = n_coords(3)
                neighbor_cond(3) = .true.
            endif
    end select


   dims_wo_halos = dims - 2*hwidth 

    do k = loopstart(3), loopend(3)
       do  j= loopstart(2), loopend(2)
           do  i = loopstart(1), loopend(1)
               if (neighbor_cond(1) .and. neighbor_cond(2) .and. neighbor_cond(3)) then 
                  should_be = i + compensate(1) + dims_wo_halos(1)*c_coords(1) +               & 
                       dims_wo_halos(1)*cart_dims(1) *  & 
                                ( ( dims_wo_halos(2)*c_coords(2) + j-1 + compensate(2) ) +                      & 
                                    dims_wo_halos(2)*cart_dims(2) * ( dims_wo_halos(3)*c_coords(3) + k-1 + compensate(3) ) )
               else
                   should_be = -1
               end if 
               if ( data(i,j,k) .ne. should_be ) then
                   lres = 0
                   write(*,'(i4,a,3I4,a,f12.4,a,f12.4,a,3i3)') rank, ": data(",i,j,k,") = ", data(i,j,k), & 
                      ", should_be, ", should_be, ", control =",  control_x, control_y, control_z
               end if
            end do
        end do
    end do
    return

end function calc_entry3D



end program testfnctsetextneigh3df
