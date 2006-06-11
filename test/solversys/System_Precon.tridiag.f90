         subroutine System_Precon ( ierror )

!...Diese Routine Vorkonditioniert die Uranus Systemmatrix indem sie
!   das Tridiagonale System invertiert ( exakt) und dies als Vorkonditionierunsgmatrix
!   benutzt. Bei der Multiplikation der Vorkdonditionierungsmatrix mit der Systemmatrix
!   muessen allerdings einige Naeherungen gemacht werden, da ansonsten die Systemmatrix
!   mehr Diagonalen bekommen wuerde.

        USE globale_daten
        USE matrix
        USE rechte_seite
        USE ergebnis
        USE numerik_daten
        USE precon_matrix

        implicit none

        integer :: dmmax, ierror, i, j, k
        double precision, dimension (0:n1+1, 0:n2+1, 0:n3+1, nc ) :: rhs_temp

        precon_text = ' Preconditioning: Approximierte Linienrelaxation'

        dmmax = n1 * n2 * n3
        call System_invert_mat ( rm000, rmb00, rmf00, d1, d2, d3, dmmax, ierror )

!...Multiplikation der rechten Seite mit der Vorkonditionierungsmatrix

        do k =0, n3+1
           do j = 0, n2+1
              do i = 0, n1+1
                 rhs_temp(i, j, k, 1) = 0.0
              end do
           end do
        end do
              
        do k = dmku, dmko
           do j = dmju, dmjo
              do i = dmiu, dmio
                 rhs_temp(i, j, k, 1) = rhs(i, j, k, 1)
              end do
           end do
        end do

        call System_Change ( rhs_temp, ierror )
        
        call System_rhs_mat ( d1, d2, d3, rhs_temp, ierror )

        do k = dmku, dmko
           do j = dmju, dmjo
              do i = dmiu, dmio
                 rhs(i, j, k, 1) = rhs_temp(i, j, k, 1)
              end do
           end do
        end do


        return
      end subroutine System_Precon

!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************


         subroutine System_invert_mat ( rm000, down, up, d1, d2, d3, dmmax, ierror )

!...Diese Routine berechnet die inverse der Vorkonditionierungsmatrix
   
         USE globale_daten

         implicit none

!...dmmax = dmielem * dmjelem, dmkelem

         integer :: dmmax, ierror
         double precision, dimension ( dmmax, nc, nc) :: rm000, down, up, d1, d2, d3
         
!...Lokale Groessen

         integer :: k
         double precision, dimension (nc, nc) :: dum1, dum2, dum3, dum4
         double precision, dimension (dmmax, nc, nc)  :: rminv

!...Als erstes muss die gesamte Blockhauptdiagonale invertiert werden.

         call System_invert_maindiag( rm000, rminv, dmmax, ierror )

!...Berechnen der drei Diagonalen der Inversen

         dum1(1, 1) = up(1, 1, 1) * rminv(2, 1, 1)
         dum2(1, 1) = down(2, 1, 1) * rminv(1, 1, 1)
         dum3(1, 1) = 1.0 - dum1(1, 1) * dum2(1, 1)

         call System_invert_block(dum3, dum4, ierror )

         d1(1, 1, 1) = rminv(1, 1, 1)*dum4(1, 1)
         d2(1, 1, 1) = 0.0
         d3(1, 1, 1) = -d1(1, 1, 1) * rminv(2, 1, 1)

!=======================================================================
!...Schleife ueber alle Bloecke

         do k = 2, dmmax-1
!=======================================================================

            dum1(1, 1) = down(k, 1, 1) * rminv(k-1, 1, 1) * &
                 up(k-1, 1, 1) * rminv(k, 1, 1)
            dum2(1, 1) = up(k, 1, 1) * rminv(k+1, 1, 1) * &
                 down(k+1, 1, 1) * rminv(k, 1, 1)
            dum3(1, 1) = 1.0 - dum1(1, 1) - dum2(1, 1) 

            call System_invert_block(dum3, dum4, ierror )
            
            d1(k, 1, 1) = rminv(k, 1, 1) * dum4(1, 1)
            d2(k, 1, 1) = -d1(k, 1, 1) * down(k, 1, 1) * rminv(k-1, 1, 1)
            d3(k, 1, 1) = -d1(k, 1, 1) * up(k, 1, 1) * rminv(k+1, 1, 1)

!=======================================================================
!...Ende der Schleife ueber alle Bloecke

         end do
!=======================================================================
!...Berechnungen fuer den Fall k = dmmax

         dum1(1, 1) = down(dmmax, 1, 1)*rminv(dmmax-1, 1, 1) *&
              up(dmmax-1, 1, 1) * rminv(dmmax, 1, 1)
         dum3(1, 1) = 1.0 - dum1(1, 1)

         call System_invert_block(dum3, dum4, ierror )

         d1(dmmax, 1, 1) = rminv(dmmax, 1, 1) * dum4(1, 1)
         d2(dmmax, 1, 1) = -d1(dmmax, 1, 1) * rminv(dmmax-1, 1, 1)
         d3(dmmax, 1, 1) = 0.0

         ierror = 0

         return
         end


!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
         subroutine System_invert_block( bl1, bl2, ierror )

!...Diese Routine invertiert einen Block, wobei das in diesem Fall
!   aus einem einzigen Element besteht.


           USE globale_daten

           implicit none

           double precision, dimension (nc, nc ) :: bl1, bl2
           integer :: ierror

           if ( bl1(1, 1) .ne. 0 ) then
              bl2(1, 1) = 1.0 / bl1(1, 1)
           else
              bl2(1, 1) = 0.0
           end if

           ierror = 0

           return
         end subroutine System_invert_block

!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
         subroutine System_invert_maindiag(bld1, bld2, dmmax, ierror )

!...Diese Routine invertiert die Hauptdiagonale der Matrix, wobei das
!   in diesem Fall auch relativ einfach ist

           USE globale_daten
           
           implicit none
           
           integer :: dmmax, ierror, k
           double precision, dimension (dmmax, nc, nc ) :: bld1, bld2

           do k = 1, dmmax

              if ( bld1(k, 1, 1) .ne. 0 ) then
                 bld2(k, 1, 1) = 1.0/bld1(k, 1, 1)
              else
                 bld2(k, 1, 1) = 0.0
              end if

           end do


           ierror = 0
           return
         end subroutine System_invert_maindiag

!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************
!***********************************************************************

         subroutine System_rhs_mat ( d1, d2, d3, rhs_temp, ierror )

!...Diese Routine berechnet die neue rechte Seite

         USE globale_daten
         
         implicit none
         
         double precision, dimension (dmiu:dmio, dmju:dmjo, dmku:dmko, nc, nc ):: &
              d1, d2, d3

         double precision, dimension (dmiu:dmio, dmju:dmjo, dmku:dmko, nc):: v
         double precision, dimension (0:n1+1, 0:n2+1, 0:n3+1, nc):: rhs_temp
         
         integer :: i, j, k, oben, ierror

         do k= dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  v(i,j,k,1) = d1(i, j, k, 1, 1) * rhs_temp(i, j, k, 1)
               end do
            end do
         end do


         do k= dmku, dmko
            do j = dmju, dmjo
               do i = 1, dmio
                  v(i,j,k,1) = d2(i, j, k, 1, 1) * rhs_temp(i-1, j, k, 1)
               end do
            end do
         end do

         if ( rand_ab ) then
            oben = dmio -1
         else
            oben = dmio
         end if

         do k= dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, oben
                  v(i,j,k,1) = d3(i, j, k, 1, 1) * rhs_temp(i+1, j, k, 1)
               end do
            end do
         end do

!...Zurueckkopieren des Vektors auf rhs_temp

         do k= dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  rhs_temp(i,j,k,1) = v(i, j, k, 1) 
               end do
            end do
         end do


         ierror = 0
         return
       end subroutine System_rhs_mat
         
