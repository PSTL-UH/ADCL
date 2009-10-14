

!****************************************************************************
      subroutine DUMP_VECTOR ( arr, rank, dims )

        implicit none
        include 'ADCL.inc'
        character(100) ::  fmt

        integer rank, dims(3)
        DATATYPE arr(dims(1), dims(2), dims(3))
        integer i, j, k

        write (fmt,'(a,i0,a)') "(i3,a1,2i5,a1,", dims(1), "f12.5)"
        do k = 1, dims(3)
           do j = 1, dims(2)
              write (*,fmt) rank, ":", k, j, ":", (arr(i,j,k), i=1,dims(1))
           end do
        end do

        return
      end subroutine DUMP_VECTOR


!******************************************************m*********************
      subroutine DUMP_VECTOR_MPI ( arr, dims, comm )

        implicit none
        include 'ADCL.inc'

        integer dims(3), comm
        DATATYPE arr(dims(1), dims(2), dims(3))
        integer i, j, k, iproc, rank, size, ierror
        character(100) ::  fmt 

        call MPI_Comm_rank ( comm, rank, ierror )
        call MPI_Comm_size ( comm, size, ierror )

        write (fmt,'(a,i0,a)') "(i3,a1,2i5,a1,", dims(1), "f12.5)"

        do iproc = 0, size-1
           if ( iproc .eq. rank) then
              do k = 1, dims(3)
                 do j = 1, dims(2)
                    write (6,fmt) rank, ":", k, j, ":", (arr(i,j,k), i=1,dims(1))
                 end do
              end do
           end if 
           call MPI_Barrier ( comm, ierror )
        end do
        return
      end subroutine DUMP_VECTOR_MPI


