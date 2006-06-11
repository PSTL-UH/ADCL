         subroutine System_Precon ( ierror )

!...Vorkonditionierung auf Basis der Minimierung der Minimierung in der
!   quadrierten Frobeniusnorm (I-MA), wobei als Wichtungsmatrix die 
!   Einheitsmatrix gewaehlt wird.

           USE globale_daten
           USE matrix
           USE rechte_seite
           USE precon_matrix
           USE trans_mat
           USE hauptdiag_trans_mat
           USE precon_trans_mat
           USE numerik_daten

           implicit none
           
           integer :: i, j, k, oben, ierror
           double precision :: dum1, dum2, dum3
           double precision, dimension (0:n1+1, 0:n2+1, 0:n3+1, nc ) :: &
                temp_vekt

           precon_text = ' Preconditioning: Minimierung in der Forbenius-Norm'

!...Initialisieren des temporaeren Vektors

           do k = 0, n3+1
              do j = 0, n2+1
                 do i = 0, n1+1
                    temp_vekt(i, j, k, 1, 1) = 0.0
                 end do
              end do
           end do

!=======================================================================
!...Datenaustausch zwischen den Nachbarn

           call System_Matchange ( ierror )

!...Datenaustausch fuer rm000 zwischen den in x-Richtung benachbarten 
!   Prozessen

           call System_X_change ( ierror ) 

!=======================================================================
!...Sonderfall i=dmiu

           do k = dmku, dmko
              do j = dmju, dmjo
                 dum1 = rm000(1, j, k, 1, 1) * rm000(1, j, k, 1, 1) + &
                      rmf00(1, j, k, 1, 1) * rmf00(1, j, k, 1, 1) + &
                      rmb00(1, j, k, 1, 1) * rmb00(1, j, k, 1, 1) 
                 dum2 = rm0f0(1, j, k, 1, 1) * rm0f0(1, j, k, 1, 1) + &
                      rm0b0(1, j, k, 1, 1) * rm0b0(1, j, k, 1, 1) 
                 dum3 = rm00f(1, j, k, 1, 1) * rm00f(1, j, k, 1, 1) + &
                      rm00b(1, j, k, 1, 1) * rm00b(1, j, k, 1, 1) 
           
                 d1(dmiu, j, k, 1, 1) = rm000(dmiu, j, k, 1, 1) / &
                      ( dum1 + dum2 + dum3 )

                 d2(dmiu, j, k, 1, 1 ) =  rmf00temp (1, j, k, 1, 1) / &
                      ( rm000temp(1, j, k, 1, 1) * rmb00(dmiu, j, k, 1, 1) + &
                        rm000(dmiu, j, k, 1, 1) * rmf000temp(1, j, k, 1, 1) )

                 d3(dmiu, j, k, 1, 1 ) = rmb00(dmiu, j, k, 1, 1) / &
                      ( rm000(dmiu, j, k, 1, 1 ) * rmb00(dmiu+1, j, k, 1, 1) + &
                        rm000(dmiu+1, j, k, 1, 1) * rmf00(dmiu, j, k, 1, 1)
              end do
           end do

!=======================================================================
!...Schleife ueber alle innere Bloecke
           do k = dmku, dmko
              do j = dmju, dmjo
                 do i = dmiu+1, dmio-1
!=======================================================================
                    dum1 = rm000(i, j, k, 1, 1) * rm000(i, j, k, 1, 1) + &
                         rmf00(i, j, k, 1, 1) * rmf00(i, j, k, 1, 1) + &
                         rmb00(i, j, k, 1, 1) * rmb00(i, j, k, 1, 1) 
                    dum2 = rm0f0(i, j, k, 1, 1) * rm0f0(i, j, k, 1, 1) + &
                         rm0b0(i, j, k, 1, 1) * rm0b0(i, j, k, 1, 1) 
                    dum3 = rm00f(i, j, k, 1, 1) * rm00f(i, j, k, 1, 1) + &
                         rm00b(i, j, k, 1, 1) * rm00b(i, j, k, 1, 1) 
           
                    d1(i, j, k, 1, 1) = rm000(i, j, k, 1, 1) / &
                         ( dum1 + dum2 + dum3 )

                    d2(i, j, k, 1, 1 ) =  rmf00(i-1, j, k, 1, 1) / &
                         ( rm000(i-1, j, k, 1, 1) * rmb00(i, j, k, 1, 1) + &
                         rm000(i, j, k, 1, 1) * rmf000(i-1, j, k, 1, 1) )
                    
                    d3(i, j, k, 1, 1 ) = rmb00(i+1, j, k, 1, 1) / &
                         ( rm000(i, j, k, 1, 1 ) * rmb00(i+1, j, k, 1, 1) + &
                         rm000(i+1, j, k, 1, 1) * rmf00(i, j, k, 1, 1)
                    
!=======================================================================
!...Ende der Schleife ueber alle innere Bloecke
                 end do
              end do
           end do
!=======================================================================
!...Sonderfall i = dmio

           do k = dmku, dmko
              do j = dmju, dmjo

                 dum1 = rm000(dmio, j, k, 1, 1) * rm000(dmio, j, k, 1, 1) + &
                      rmf00(dmio, j, k, 1, 1) * rmf00(dmio, j, k, 1, 1) + &
                      rmb00(dmio, j, k, 1, 1) * rmb00(dmio, j, k, 1, 1) 
                 dum2 = rm0f0(dmio, j, k, 1, 1) * rm0f0(dmio, j, k, 1, 1) + &
                      rm0b0(dmio, j, k, 1, 1) * rm0b0(dmio, j, k, 1, 1) 
                 dum3 = rm00f(dmio, j, k, 1, 1) * rm00f(dmio, j, k, 1, 1) + &
                      rm00b(dmio, j, k, 1, 1) * rm00b(dmio, j, k, 1, 1) 
           
                 d1(dmio, j, k, 1, 1) = rm000(dmio, j, k, 1, 1) / &
                      ( dum1 + dum2 + dum3 )
                 
                 d2(dmio, j, k, 1, 1 ) =  rmf00(dmio-1, j, k, 1, 1) / &
                      ( rm000(dmio-1, j, k, 1, 1) * rmb00(dmio, j, k, 1, 1) + &
                      rm000(dmio, j, k, 1, 1) * rmf000(dmio-1, j, k, 1, 1) )
                 
                 d3(dmio, j, k, 1, 1 ) = rmb00temp(1, j, k, 1, 1) / &
                      ( rm000(dmio, j, k, 1, 1 ) * rmb00temp(1, j, k, 1, 1) + &
                      rm000temp(2, j, k, 1, 1) * rmf00(dmio, j, k, 1, 1)

              end do
           end do


!...Multiplikation der rechten Seite mit der Vorkonditionierungsmatrix
!   Dazu zuerst umkopieren der rechten Seite in einen temporaeren Vektor

           do k = dmku, dmko
              do j = dmju, dmjo
                 do i = dmiu, dmio
                    temp_vekt(i, j, k, 1) = rhs(i, j, k, 1)
                 end do
              end do
           end do

!...Datenaustausch fuer rhs

           call System_Change ( temp_vekt, ierror )

!...Multiplikation mit d1

           do k = dmku, dmko
              do j = dmju, dmjo
                 do i = dmiu, dmio
                    rhs(i, j, k, 1) = d1(i,j,k,1,1)*temp_vekt(i,j,k,1) 
                 end do
              end do
           end do

!...Multiplikation mit d2

           do k = dmku, dmko
              do j = dmju, dmjo
                 do i = 1, dmio
                    rhs(i,j,k,1) = rhs(i,j,k,1) + &
                         d2(i,j,k,1,1)*temp_vekt(i-1,j,k,1) 
                 end do
              end do
           end do
           
!...Multiplikation mit d3

           if ( rand_ab ) then
              oben = dmio -1
           else
              oben = dmio
           end if
           
           do k = dmku, dmko
              do j = dmju, dmjo
                 do i = dmiu, oben
                    rhs(i,j,k,1) = rhs(i,j,k,1) + &
                         d3(i,j,k,1,1)*temp_vekt(i+1,j,k,1)
                 end do
              end do
           end do
         

           ierror = 0

           return
         end subroutine System_Precon

!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************


         subroutine System_X_change ( ierror )

!...Datenaustausch fuer die Hauptdiagonale  nur mit den Nachbarn in 
!   x-Richtung

           USE globale daten
           USE hauptdiag_trans_mat
           USE matrix

           implicit none

           include 'mpif.h'

           integer :: ierror, i, j, k, size2, position, handnum
           integer, dimension ( MPI_STATUS_SIZE, 2 ) :: sendstatus
           integer, dimension ( MPI_STATUS_SIZE ) :: status
           integer, dimension ( 2 ) :: recvhandle, sendhandle

           double precision, dimension ((n2+2)*(n3+2)*nc*nc) ) :: &
                sendbuf1, sendbuf2, recvbuf1, revcbuf2

           size2 = (n2+2)*(n3+2)*nc*nc*SIZE_OF_REALx

!...Asynchrones Empfangen der rm000 der Nachbarprozessoren

           if (.not. rand_ab) then
              call MPI_IRECV(recvbuf1, size2, MPI_PACKED, tid_io, 5000,  &
                   MPI_COMM_WORLD, recvhandle(1), info)
           else
              recvhandle(1) = MPI_REQUEST_NULL
           endif

           if (.not. rand_sing) then
              call MPI_IRECV(recvbuf2, size2, MPI_PACKED, tid_iu, 5001,  &
                   MPI_COMM_WORLD, recvhandle(2), info)
           else
              recvhandle(2) = MPI_REQUEST_NULL
           endif

!...Asynchrones Senden der Matrizenteile an die Nachbarprozessoren

           if (.not. rand_sing) then
              position = 0
              do l = 1, nc
                 do n = 1, nc
                    do k = dmku, dmko
                       do j = dmju, dmjo
                          call MPI_PACK(rm000(1,j,k,n,l), 1, MPI_DOUBLE_PRECISION, sendbuf1,   &
                               size2, position, MPI_COMM_WORLD, info)
                       end do
                    end do
                 end do
              end do
              call MPI_ISEND(sendbuf1, position, MPI_PACKED, tid_iu, 5000, &
                   MPI_COMM_WORLD, sendhandle(1), info)
           else
              sendhandle(1) = MPI_REQUEST_NULL
           end if
           
           if (.not. rand_ab) then
              position = 0
              do l = 1, nc
                 do n = 1, nc
                    do  k = dmku, dmko
                       do  j = dmju, dmjo
                          call MPI_PACK(rm000(dmio,j,k,n, l), 1, MPI_DOUBLE_PRECISION, sendbuf2,  &
                               size2, position,  MPI_COMM_WORLD, info)
                       end do
                    end do
                 end do
              end do
              
              call MPI_ISEND(sendbuf2, position, MPI_PACKED, tid_io, 5001,  &
                   MPI_COMM_WORLD, sendhandle(2), info)
           else
              sendhandle(2) = MPI_REQUEST_NULL
           end if
           
!- - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - -      
!...Auspacken der empfangenen Nachrichten
      
           do ni = 1, 2
              
              call MPI_WAITANY(2, recvhandle, handnum, status, info)
              
              position = 0
              
              select case (handnum)
                 
              case (1)
                 do l = 1, nc
                    do n = 1, nc
                       do k = dmku, dmko
                          do j = dmju, dmjo
                             call MPI_UNPACK(recvbuf1, size2, position,  &
                                  rm000temp(2,j,k,n,l),1, MPI_DOUBLE_PRECISION,  &
                                  MPI_COMM_WORLD, info)
                          end do
                       end do
                    end do
                 end do
                 
              case (2)
                 do l = 1, nc
                    do n = 1, nc
                       do k = dmku, dmko
                          do j = dmju, dmjo
                             call MPI_UNPACK(recvbuf2, size2, position,  &
                                  rm000temp(1,j,k,n,l), 1, MPI_DOUBLE_PRECISION,  &
                                  MPI_COMM_WORLD, info)
                          end do
                       end do
                    end do
                 end do

              end select

              ierror = 0
              
              return
            end subroutine System_X_change









