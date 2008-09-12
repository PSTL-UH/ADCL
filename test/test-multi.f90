      program test_prometheus
      ! tests of functionality required for PROMETHEUS/VERTEX
      ! uses two requests (one for allreduce, one for allgatherv)
      ! simultaneously
      implicit none
      include 'ADCL.inc'
      integer topo
      integer cart_comm
   
      
      integer, parameter :: nymom = 4
      
      ! rsweep
      integer, parameter :: qx = 2, qz = 3
      integer :: vecdim, loopcnt 
   
      integer :: ierror
      integer, dimension(2) :: dims
      integer :: cdims = 0
      integer :: periods = 0
      integer :: nprocs, rank, i, j

      call MPI_Init ( ierror )
      call MPI_Comm_rank ( MPI_COMM_WORLD, rank, ierror )
      call MPI_Comm_size ( MPI_COMM_WORLD, nprocs, ierror )

      call adcl_init ( ierror )
      call MPI_Dims_create ( nprocs, 1, cdims, ierror )
      call MPI_Cart_create ( MPI_COMM_WORLD, 1, cdims, periods, 0, &
        cart_comm, ierror)
   
      call adcl_topology_create ( cart_comm, topo, ierror )
 
      loopcnt = 50

!!! run one test at a time as long as historic learning does not work 
!!! properly with new function sets

      ! running two request with different function sets simultaneously
      call test_two_requests(loopcnt, nymom, rank, nprocs, topo)
        
      ! buffer is of length x*4 and defined as ADCL_VECTOR_HALO
      ! needed e.g. for rsweep
      vecdim = qx*4*qz
      !call test_halo(loopcnt, vecdim, rank, nprocs, topo)


      call adcl_topology_free ( topo, ierror )
      call MPI_Comm_free ( cart_comm, ierror )
      call adcl_finalize ( ierror )

      call MPI_Finalize ( ierror )


      end program test_prometheus

!-----------------------------------------------------------------------
      subroutine test_two_requests(loopcnt, nymom, rank, nprocs, topo)
!-----------------------------------------------------------------------
!     tests if running two requests simultaneously with different 
!     function sets (here ADCL_FNCTSET_ALLREDUCE and
!     ADCL_FNCTSET_ALLGATHERV) works
!-----------------------------------------------------------------------
      implicit none
      include 'ADCL.inc' 
      integer, intent(in) :: loopcnt, nymom, rank, nprocs, topo
  
      integer svmap_inplace, svec_inplace
   
      ! HTO: compute hydro timestep: dtmin (rady/rady.F)
      double precision  :: adcl_dtmin
      integer rvmap_dtmin, rvec_dtmin, req_dtmin

      ! comm_qterms
      integer, dimension(:), allocatable :: displ, rcnts
      double precision, dimension(:), allocatable :: adcl_hlp
      integer rvmap_qterms, rvec_qterms, req_qterms

      integer :: i, ierror

      ! register in_place dummy send vector
      call adcl_vmap_inplace_allocate( ADCL_VECTOR_INPLACE, &
         svmap_inplace, ierror )
      if ( ADCL_SUCCESS .ne. ierror) &
         print *, "vmap_inplace_allocate not successful"  
      call adcl_vector_register_generic ( 0, 0, 0, svmap_inplace, & 
         MPI_DATATYPE_NULL, MPI_IN_PLACE, svec_inplace, ierror )
      if ( ADCL_SUCCESS .ne. ierror) &
         print *, "vector_register for svec_inplace not successful"
   
   
      !-----------------------------------------------------------------
      ! prepare communications
      !-----------------------------------------------------------------
      ! comm_dtmin:
      call adcl_vmap_allreduce_allocate( ADCL_VECTOR_ALLREDUCE, & 
         MPI_MIN, rvmap_dtmin, ierror )
      if ( ADCL_SUCCESS .ne. ierror) & 
         print *, "vmap_allreduce_allocate for rvmap_dtmin not ", &
         "successful"
      call adcl_vector_register_generic ( 1,  1, 0, rvmap_dtmin, & 
         MPI_DOUBLE_PRECISION, adcl_dtmin, rvec_dtmin, ierror )
      if ( ADCL_SUCCESS .ne. ierror) & 
         print *, "vector_register for rvmap_dtmin not successful"
   
      call adcl_request_create_generic ( svec_inplace, rvec_dtmin, & 
         topo, ADCL_FNCTSET_ALLREDUCE, req_dtmin, ierror )
      if ( ADCL_SUCCESS .ne. ierror) & 
         print *, "request_create for req_dtmin not successful"
   
   
! comm_qterms: AllGatherV to verify convergence on all rays
      ! register in_place dummy send vector
      allocate(displ(0:nprocs-1), rcnts(0:nprocs-1))
      call set_displ(nprocs, nymom, rcnts, displ)
      !if (my_proc .eq. 0)
      write(*,*) "displ", displ, "rcnts", rcnts
      allocate (adcl_hlp(nprocs*nymom))

      call adcl_vmap_list_allocate( ADCL_VECTOR_LIST, nprocs, rcnts, & 
         displ, rvmap_qterms, ierror )
      if ( ADCL_SUCCESS .ne. ierror) & 
         print *, "vmap_list_allocate for rvmap_qterms not successful"
      call adcl_vector_register_generic ( 1,  nprocs*nymom, 0, & 
         rvmap_qterms, MPI_DOUBLE_PRECISION, adcl_hlp, rvec_qterms, ierror )
      if ( ADCL_SUCCESS .ne. ierror) & 
          print *, "vmap_vector_register for rvec_qterms not successful"
   
      call adcl_request_create_generic ( svec_inplace, rvec_qterms, & 
         topo, ADCL_FNCTSET_ALLGATHERV, req_qterms, ierror )
      if ( ADCL_SUCCESS .ne. ierror) & 
         print *, "request_create not successful"
 
  
      do i = 1, 50
          call adcl_request_start(req_dtmin, ierror)
          call adcl_request_start(req_qterms, ierror)
      end do

      deallocate(displ, rcnts)

      call adcl_vector_deregister( svec_inplace,  ierror )
      call adcl_vmap_free        ( svmap_inplace, ierror )

      call adcl_request_free      ( req_dtmin,   ierror )
      call adcl_vector_deregister ( rvec_dtmin,  ierror )
      call adcl_vmap_free         ( rvmap_dtmin, ierror )

      call adcl_request_free      ( req_qterms,   ierror )
      call adcl_vector_deregister ( rvec_qterms,  ierror )
      call adcl_vmap_free         ( rvmap_qterms, ierror )

      end subroutine test_two_requests


!-----------------------------------------------------------------------
      subroutine test_halo(loopcnt, vecdim, rank, nprocs, topo)
!-----------------------------------------------------------------------
!     tests if next-neighbor communiction with array of size 
!     vecdim=4*scnt defined as ADCL_VECTOR_HALO is successful     
!-----------------------------------------------------------------------
      implicit none
      include 'ADCL.inc'
      integer, intent(in) :: loopcnt, vecdim, rank, nprocs, topo
      integer :: i, j, scnt, ierror
      double precision, dimension(:), allocatable :: adcl_rsweep
      integer :: vmap_rsweep, vec_rsweep, req_rsweep
      
      scnt = vecdim / 4

      allocate ( adcl_rsweep(vecdim) )
      call adcl_vmap_halo_allocate ( ADCL_VECTOR_HALO, scnt, &
         vmap_rsweep, ierror)
      call check_success( ierror, "vmap_halo_allocate for vmap_rsweep")
      call adcl_vector_register_generic ( 1, vecdim, 0, vmap_rsweep, &
         MPI_DOUBLE_PRECISION, adcl_rsweep, vec_rsweep, ierror)
      call check_success( ierror, "vector register for vec_rsweep")
      call adcl_request_create ( vec_rsweep, topo, &
        ADCL_FNCTSET_NEIGHBORHOOD, req_rsweep, ierror )
      call check_success( ierror, "request_create for req_rsweep")
      
      do i = 1, loopcnt 
         do j = 1, scnt
            adcl_rsweep(       j) = -1
            adcl_rsweep(  scnt+j) = 2*rank
            adcl_rsweep(2*scnt+j) = 2*rank+1
            adcl_rsweep(3*scnt+j) = -1
         end do 
         call adcl_request_start(req_rsweep, ierror)

         do j = 1, scnt
            if ( adcl_rsweep(j) .ne. 2*rank-1) then
               print *, "lb:", rank, adcl_rsweep(j)
            end if 
            if ( adcl_rsweep(3*scnt+j) .ne. 2*rank+2) then 
               if (rank .ne. nprocs-1) & 
                  print *, "ub:", rank, adcl_rsweep(3*scnt+j)
            end if
         end do 
      end do

      call adcl_request_free      ( req_rsweep,  ierror )
      call adcl_vector_deregister ( vec_rsweep,  ierror )
      call adcl_vmap_free         ( vmap_rsweep, ierror )
      deallocate (adcl_rsweep)

      end subroutine test_halo



!-----------------------------------------------------------------------
      subroutine set_displ(nprocs, num, rcnts, displ)
!-----------------------------------------------------------------------
!     helper function to set displacement and receive counts for an
!     allgatherv operation for varying num
!-----------------------------------------------------------------------
      implicit none
      integer, intent(in) :: nprocs, num
      integer, dimension(0:nprocs-1), intent(out) :: displ, rcnts

      integer :: n, offset

      offset = 0 
      do n = 0, nprocs-1
         rcnts(n) = num
         displ(n) = offset 
         offset = offset + num
      end do

      end subroutine set_displ

!-----------------------------------------------------------------------
      subroutine check_success( ier, what )
!-----------------------------------------------------------------------
      implicit none
      include 'ADCL.inc'
      integer, intent(in) :: ier
      character(*), intent(in) :: what

      if ( ADCL_SUCCESS .ne. ier) &
        write(*,*) what, " not successful"

      end subroutine check_success
