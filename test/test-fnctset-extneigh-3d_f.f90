!
! Copyright (c) 2009           HLRS. All rights reserved.
! $COPYRIGHT$
!
! Additional copyrights may follow
!
! $HEADER$
!
#define VERBOSE

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

        print *, "Sorry, still under development."
        stop

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
        !call dump_vector_3D_mpi_dp ( data1, dims, cart_comm )
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

! subroutine set_data_3D ( data, rank, dims, hwidth, cart_comm, nc)
! 
!    implicit none
!    include 'ADCL.inc'
! 
!    integer, intent(in) :: rank, hwidth, cart_comm, nc
!    integer :: i, j, k, ierr
!    integer, dimension(2), intent(in) :: dims
!    integer, dimension(2) :: coords, dimsize, period
!    double precision :: data(dims(1),dims(2), nc)
! 
!    call MPI_Cart_get(cart_comm, 2, dimsize, period, coords, ierr)
! !  write (*,*) 'coords(1)=', coords(1)
! !  write (*,*) 'coords(2)=', coords(2)
! 
!    do i=1,hwidth
!       do j = 1, dims(2)
!          do k = 1, nc
!              data(i, j, k) = -1
!          end do
!       end do
!    end do
! 
!    do i = dims(1)-hwidth+1, dims(1)
!       do j = 1, dims(2)
!          do k= 1, nc
!             data(i, j, k) = -1
!          end do
!       end do
!    end do
! 
!    do i=1,dims(1)
!       do j = 1, hwidth
!          do k = 1, nc 
!             data(i, j, k) = -1
!          end do
!        end do
!    end do
! 
!    do i = 1, dims(1)
!       do j = dims(2)-hwidth+1, dims(2)
!          do k = 1, nc
!             data(i, j, k) = -1
!          end do
!        end do
!    end do
! 
!    !beaware of the change in i and j in the equation compare to the c code. (i-1) and (j-1)
!    do i = hwidth+1, dims(1)-hwidth
!       do j = hwidth+1, dims(2)-hwidth
!          do k = 1, nc
!             data(i, j, k) =  (coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
!                 + (dimsize(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * coords(2) + ((j-1)-hwidth))
!          end do
!       end do
!    end do
! 
!    return
! end subroutine set_data_3D


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
   integer :: i, j, k, ierr, lres=1, gres
   integer, dimension(3), intent(in) :: dims
   integer, dimension(3) :: coords, n_coords, c_coords, cart_size, period
   integer, dimension(6), intent(in) :: neighbors
   double precision should_be
   double precision :: data(dims(1),dims(2), dims(3))

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
    integer :: coords(3), n_coords(0:2), c_coords(0:2) ! coords, coords of neighbor and corrected coords of MPI process
    integer :: period(3)
    integer :: compensate(3)  ! what do I have to add / substract to my coordinate on the 
                              ! neighboring process to compare the values in the halo cell and 
                              ! in the domain of the neighboring process
    integer :: loopstart(0:2), loopend(0:2)  ! defines part of array to check
    logical :: neighbor_cond(3)            ! is there a neighbor in x,y,z-direction?

    lres = 1

    call MPI_Cart_get (cart_comm, 3, cart_dims, period, coords, ierr)
    neighbor_cond = .false.

    select case (control_x)
       case (0)
            loopstart(0)     = hwidth
            loopend(0)       = dims(1) - hwidth
            compensate(1)    = 0
            c_coords(0)      = coords(1)
            neighbor_cond(1) = .true.
       case (1)
            loopstart(0)     = dims(1) - hwidth *2
            loopend(0)       = dims(1) - hwidth
            compensate(1)    = - (dims(1) - hwidth*2)
            if (neighbors(1) .ne. MPI_PROC_NULL ) then 
                call MPI_Cart_coords (cart_comm, neighbors(1), 3, n_coords)
                c_coords(0) = n_coords(0)
                neighbor_cond(1) = .true.
            endif
       case (-1)
            loopstart(0) = hwidth
            loopend(0) = hwidth * 2
            compensate(1) = (dims(1) - hwidth*2) 
            if (neighbors(2) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(2), 3, n_coords)
                c_coords(0) = n_coords(0)
                neighbor_cond(1) = .true.
            endif
    end select

    select case (control_y)
        case (0)
            loopstart(1) = hwidth
            loopend(1) = dims(2) - hwidth
            compensate(2) = 0
            c_coords(1) = coords(2)
            neighbor_cond(2) = .true.
        case (1)
            loopstart(1) = dims(2) - hwidth *2
            loopend(1) = dims(2)-hwidth ; 
            compensate(1) = - (dims(2) - hwidth*2) 
            if (neighbors(3) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(3), 3, n_coords)
                c_coords(1) = n_coords(1)
                neighbor_cond(2) = .true.
            endif
        case (-1)
            loopstart(1) = hwidth
            loopend(1) = hwidth * 2
            compensate(2) = (dims(2) - hwidth*2) 
            if (neighbors(4) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(4), 3, n_coords)
                c_coords(1) = n_coords(1)
                neighbor_cond(2) = .true.
            endif
    end select

    select case (control_z)
        case (0)
            loopstart(2) = hwidth
            loopend(2) = dims(3) - hwidth
            compensate(3) = 0
            c_coords(2) = coords(3)
            neighbor_cond(3) = .true.
        case (1)
            loopstart(2) = dims(3) - hwidth *2
            loopend(2) = dims(3)-hwidth
            compensate(3) = - (dims(3) - hwidth*2) 
            if (neighbors(5) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(5), 3, n_coords)
                c_coords(2) = n_coords(2)
                neighbor_cond(3) = .true.
            endif
        case (-1)
            loopstart(2) = hwidth
            loopend(2) = hwidth * 2
            compensate(3) = (dims(3) - hwidth*2) 
            if (neighbors(6) .ne. MPI_PROC_NULL ) then
                call MPI_Cart_coords (cart_comm, neighbors(6), 3, n_coords)
                c_coords(2) = n_coords(2)
                neighbor_cond(3) = .true.
            endif
    end select


   dims_wo_halos = dims - 2*hwidth 

   do  i = loopstart(0), loopend(0)
        do  j= loopstart(1), loopend(1)
            do k = loopstart(2), loopend(2)
                    if (neighbor_cond(1) .and. neighbor_cond(2) .and. neighbor_cond(3)) then 
                       should_be = i + compensate(1) + dims_wo_halos(1)*coords(1) +                                     & 
                            dims(1)*cart_dims(1) *  & 
                                     ( ( dims(2)*coords(2) + j-1 + compensate(2) ) +                      & 
                                         dims(2)*cart_dims(2) * ( dims(3)*coords(3) + k-1 + compensate(3) ) )
                    else
                        should_be = -1
                    end if 
                    if ( data(i,j,k) .ne. should_be ) then
                        lres = 0
                        print *, "4D shouldbe = ", should_be, ", data= ", data(i,j,k), & 
                           " control_x= ", control_x, ", control_y=",  control_y, ", control_z= ", control_z, ", rank = ", rank
                    end if
            end do
        end do
    end do
    return

end function


!****************************************************************************

subroutine check_data_3D_old ( data, rank, dims, hwidth, neighbors, cart_comm, nc) 

   implicit none
   include 'ADCL.inc'

   integer, intent(in) :: rank, hwidth, cart_comm, nc
   integer :: i, j, k, ierr, lres, gres
   integer, dimension(2), intent(in) :: dims
   integer, dimension(2) :: coords, n_coords, c_coords, cart_size, period
   integer, dimension(4), intent(in) :: neighbors
   double precision should_be
   double precision :: data(dims(1),dims(2), nc)

   call MPI_Cart_get(cart_comm, 3, cart_size, period, coords, ierr)

!   write (*,*) 'coords(1)=', coords(1)
!   write (*,*) 'coords(2)=', coords(2)
        
    lres = 1


!.......Up HALO Cell
    if( neighbors(1) .ne. MPI_PROC_NULL ) then 
       call MPI_Cart_coords (cart_comm, neighbors(1), 2, n_coords, ierr)
    endif
    do j = hwidth+1, dims(2)-hwidth
       do i= dims(1) - hwidth * 2 + 1, dims(1) - hwidth
          do k = 1, nc
          if( neighbors(1) .ne. MPI_PROC_NULL ) then
    should_be = (n_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * n_coords(2) + ((j-1)-hwidth))
    else
    should_be = -1
    endif

    if( data( i - (dims(1) - hwidth * 2), j, k) .ne. should_be ) then
    lres = 0
    write (*, 99) rank, ':', 'Up', i - (dims(1) - hwidth * 2),j, k, 'is', &
    data(i - (dims(1) - hwidth * 2), j, k), 'should be', should_be
    endif
    end do
    end do
    end do

!.......Down HALO Cell
		if( neighbors(2) .ne. MPI_PROC_NULL ) then 
			call MPI_Cart_coords (cart_comm, neighbors(2), 2, n_coords, ierr)
		endif
	    do j = hwidth+1, dims(2)-hwidth	
			do i= hwidth + 1, hwidth * 2 
				do k = 1, nc
					if( neighbors(2) .ne. MPI_PROC_NULL ) then
						should_be = (n_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * n_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i + (dims(1) - hwidth * 2), j, k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Down', i + (dims(1) - hwidth * 2), j, k, 'is',& 
						 data(i + (dims(1) - hwidth * 2), j, k), 'should be', should_be
					endif
				end do				
			end do
		end do

!.......Left HALO Cell
		if( neighbors(3) .ne. MPI_PROC_NULL ) then 
			call MPI_Cart_coords (cart_comm, neighbors(3), 2, n_coords, ierr)
		endif
	    do i = hwidth+1, dims(1)-hwidth	
			do j= dims(2) - hwidth * 2 + 1, dims(2) - hwidth
				do k = 1, nc
					if( neighbors(3) .ne. MPI_PROC_NULL ) then
						should_be = (n_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * n_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i, j - (dims(2) - hwidth * 2), k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Left', i, j - (dims(2) - hwidth * 2), k, 'is',&
						 data(i, j - (dims(2) - hwidth * 2), k), 'should be', should_be
					endif
				end do				
			end do
		end do

!.......Right HALO Cell
		if( neighbors(4) .ne. MPI_PROC_NULL ) then 
			call MPI_Cart_coords (cart_comm, neighbors(4), 2, n_coords, ierr)
		endif
	    do i = hwidth + 1, dims(1)-hwidth	
			do j= hwidth + 1, hwidth * 2 
				do k = 1, nc
					if( neighbors(4) .ne. MPI_PROC_NULL ) then
						should_be = (n_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * n_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i, j + (dims(2) - hwidth * 2), k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Right', i, j + (dims(2) - hwidth * 2), k, 'is', &
						 data(i, j+(dims(2) - hwidth * 2), k), 'should be', should_be
					endif	
				end do			
			end do
		end do

!.......Inside 
	    do i = hwidth + 1, dims(1)-hwidth	
			do j= hwidth + 1, dims(2)-hwidth 
				do k = 1, nc 
					should_be = (coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             + (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * coords(2) + ((j-1)-hwidth))
			
					if( data( i, j, k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Inside', i,j,k, 'is', data(i, j, k), 'should be', should_be
					endif		
				end do 		
			end do
		end do


!.......Up-Left Corner
		if( (neighbors(1) .ne. MPI_PROC_NULL) .AND. (neighbors(3) .ne. MPI_PROC_NULL)) then 
			call MPI_Cart_coords (cart_comm, neighbors(1), 2, n_coords, ierr)
			c_coords(1) = n_coords(1)
			call MPI_Cart_coords (cart_comm, neighbors(3), 2, n_coords, ierr)
			c_coords(2) = n_coords(2)
		endif
	    do i = dims(1) - hwidth * 2 + 1, dims(1) - hwidth	
			do j= dims(2) - hwidth * 2 + 1, dims(2) - hwidth
				do k = 1, nc
					if( (neighbors(1) .ne. MPI_PROC_NULL) .AND. (neighbors(3) .ne. MPI_PROC_NULL)) then 
						should_be = (c_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * c_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i - (dims(1) - hwidth *2), j - (dims(2) - hwidth * 2), k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Up-Left', i - (dims(1) - hwidth *2),j - (dims(2) - hwidth * 2), k, 'is',&
						 data(i - (dims(1) - hwidth * 2), j - (dims(2) - hwidth * 2), k), 'should be', should_be
					endif
				end do				
			end do
		end do


!.......Up-Right Corner
		if( (neighbors(1) .ne. MPI_PROC_NULL) .AND. (neighbors(4) .ne. MPI_PROC_NULL)) then 
			call MPI_Cart_coords (cart_comm, neighbors(1), 2, n_coords, ierr)
			c_coords(1) = n_coords(1)
			call MPI_Cart_coords (cart_comm, neighbors(4), 2, n_coords, ierr)
			c_coords(2) = n_coords(2)
		endif
	    do i = dims(1) - hwidth * 2 + 1, dims(1) - hwidth	
			do j= hwidth + 1, hwidth * 2
				do k=1, nc
					if( (neighbors(1) .ne. MPI_PROC_NULL) .AND. (neighbors(4) .ne. MPI_PROC_NULL)) then 
						should_be = (c_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * c_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i - (dims(1) - hwidth *2), j + (dims(2) - hwidth * 2), k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Up-Right', i - (dims(1) - hwidth *2),j + (dims(2) - hwidth * 2), k, 'is',&
						 data(i - (dims(1) - hwidth * 2), j + (dims(2) - hwidth * 2), k), 'should be', should_be
					endif		
				end do		
			end do
		end do


!.......Down-Left Corner
		if( (neighbors(2) .ne. MPI_PROC_NULL) .AND. (neighbors(3) .ne. MPI_PROC_NULL)) then 
			call MPI_Cart_coords (cart_comm, neighbors(2), 2, n_coords, ierr)
			c_coords(1) = n_coords(1)
			call MPI_Cart_coords (cart_comm, neighbors(3), 2, n_coords, ierr)
			c_coords(2) = n_coords(2)
		endif
	    do i = hwidth + 1, hwidth * 2	
			do j= dims(2) - hwidth * 2 + 1, dims(2) - hwidth
				do k=1, nc
					if( (neighbors(2) .ne. MPI_PROC_NULL) .AND. (neighbors(3) .ne. MPI_PROC_NULL)) then 
						should_be = (c_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * c_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i + (dims(1) - hwidth *2), j - (dims(2) - hwidth * 2), k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Down-Left', i + (dims(1) - hwidth *2),j - (dims(2) - hwidth * 2), k, 'is',&
						 data(i + (dims(1) - hwidth * 2), j - (dims(2) - hwidth * 2), k), 'should be', should_be
					endif			
				end do	
			end do
		end do


!.......Down-Right Corner
		if( (neighbors(2) .ne. MPI_PROC_NULL) .AND. (neighbors(4) .ne. MPI_PROC_NULL)) then 
			call MPI_Cart_coords (cart_comm, neighbors(2), 2, n_coords, ierr)
			c_coords(1) = n_coords(1)
			call MPI_Cart_coords (cart_comm, neighbors(4), 2, n_coords, ierr)
			c_coords(2) = n_coords(2)
		endif
	    do i = hwidth + 1, hwidth * 2	
			do j= hwidth + 1, hwidth * 2
				do k= 1, nc
					if( (neighbors(2) .ne. MPI_PROC_NULL) .AND. (neighbors(4) .ne. MPI_PROC_NULL)) then 
						should_be = (c_coords(1) * (dims(1)-hwidth*2) + ((i-1)-hwidth) ) & 
		             	+ (cart_size(1) * (dims(1) - hwidth*2)) * ((dims(2) - hwidth*2) * c_coords(2) + ((j-1)-hwidth))
					else
						should_be = -1
					endif
			
					if( data( i + (dims(1) - hwidth *2), j + (dims(2) - hwidth * 2), k) .ne. should_be ) then
						lres = 0
						write (*, 99) rank, ':', 'Down-Right', i + (dims(1) - hwidth *2),j + (dims(2) - hwidth * 2),k, 'is',&
						 data(i + (dims(1) - hwidth * 2), j + (dims(2) - hwidth * 2), k), 'should be', should_be
					endif
				end do				
			end do
		end do

99      format (i1,a3,a10,3i3,a4,f12.5,a11,f12.5)

        call MPI_Allreduce ( lres, gres, 1, MPI_INTEGER, MPI_MIN, MPI_COMM_WORLD, ierr)
        if ( gres .eq. 0 ) then
!          call dump_vector_3D( data, rank, dims )
           if ( rank .eq. 0 ) then
              write (*,*) '2-D Fortran testsuite hwidth =', hwidth, &
                   'nc = ', nc, ' failed'  
           end if
        else
           if ( rank .eq. 0 ) then
              write (*,*) '2-D Fortran testsuite hwidth =', hwidth, &
                   'nc = ', nc, ' passed'  
           end if
        end if


        return
      end subroutine check_data_3D_old

!****************************************************************************

! !**************************************************************************************************
! function calc_entry ( i, j, cond, dim, dims, n_coords, hwidth )
! !**************************************************************************************************
!     integer, intent(in) :: i, j, hwidth
!     logical, intent(in) :: cond
!     integer, dimension(2), intent(in) :: dim, dims, n_coords
!     integer :: entry = -1
!     double precision :: calc_entry
! 
!     if ( cond ) then
!         !printf("coords = %d %d, n_coords = %d, %d \n", coords(0), coords(1), n_coords(0), n_coords(1));
!         entry = (n_coords(1) * (dim(1)-hwidth*2) + (i-hwidth) ) + & 
!             (dims(1) * (dim(1)-hwidth*2)) * ((dim(2) - hwidth*2) * n_coords(2) + (j-hwidth))
!     end if
! 
!     calc_entry = entry;
! end function calc_entry
! 
! !**************************************************************************************************
! subroutine check_entry3D ( data, i, j, k, should_be, error, lres ) 
! !**************************************************************************************************
!    double precision, dimension(:,:,:), intent(in) :: data 
!    integer, intent(in) :: i, j, k
!    double precision, intent(in) :: should_be
!    character(*), intent(in) :: error
!    integer, intent(out) :: lres
! 
! 
!     if ( data(i,j,k) .ne. should_be ) then
!         lres = 0;
!         print *, "3D ", error, "shouldbe = ", should_be, " data= ", data(i,j,k), " nc= ", k
!     end if
! end subroutine check_entry3D
! 
! !**************************************************************************************************
! subroutine check_entry2D ( data, i, j, should_be, error, lres ) 
! !**************************************************************************************************
!    double precision, dimension(:,:), intent(in) :: data 
!    integer, intent(in) :: i, j
!    double precision, intent(in) :: should_be
!    character(*), intent(in) :: error
!    integer, intent(out) :: lres
! 
!    if ( data(i,j) .ne. should_be ) then
!       lres = 0;
!        print *, "2D ", error, "shouldbe = ", should_be, " data= ", data(i,j)
!    end if
! 
! end subroutine check_entry2D



end program testfnctsetextneigh3df
