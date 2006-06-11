         subroutine System_Matmul ( vekt, erg_vekt, ierror )

!....Diese Routine fuehrt eine Matrix-Vektor Multiplikation aus.
!    Dabei wird allerdings zuerst mit der Systemmatrix
!    multipliziert, und anschliessend mit der Vorkonditionierungsmatrix

         USE globale_daten
         USE matrix
         USE precon_matrix

         implicit none

         double precision, dimension(0:n1+1, 0:n2+1, 0:n3+1, nc ):: vekt, v
         double precision, dimension(dmiu:dmio, dmju:dmjo, dmku:dmko, nc)::erg_vekt

!...Hilfsgroessen und Laufvariablen

         integer :: i, j, k
         integer :: oben, ierror

!...Initialisieren des zwischen_vektors v

         do k = 0, n3+1
            do j = 0, n2+1
               do i = 0, n1+1
                  v(i, j, k, 1) = 0.0
               end do
            end do
         end do


!...Initialisierung von erg_vekt

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  erg_vekt(i,j,k,1) = 0.0
               end do
            end do
         end do

!...Multiplikation mit rm000

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  erg_vekt(i,j,k,1) = rm000(i,j,k,1,1)*vekt(i,j,k,1) 
               end do
            end do
         end do

!         write (*,*) mytid, ' :::: After rm000 multiplication'

!...Multiplikation mit rmb00

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = 1, dmio
                  erg_vekt(i,j,k,1) = erg_vekt(i,j,k,1) + &
                      rmb00(i,j,k,1,1)*vekt(i-1,j,k,1) 
               end do
            end do
         end do

!        write (*,*) mytid, ' :::: After rmb00 multiplication'

!...Multiplikation mit rmf00

         if ( rand_ab ) then
            oben = dmio -1
         else
            oben = dmio
         end if
         
         do k = dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, oben
                  erg_vekt(i,j,k,1) = erg_vekt(i,j,k,1) + &
                      rmf00(i,j,k,1,1)*vekt(i+1,j,k,1)
               end do
            end do
         end do
!         
!         write (*,*) mytid, ' :::: After rmf00 multiplication'
!...Multiplikation mit rm0b0

         do k = dmku, dmko
            do j = 1, dmjo
               do i = dmiu, dmio
                  erg_vekt(i,j,k,1) = erg_vekt(i,j,k,1) + &
                      rm0b0(i,j,k,1,1)*vekt(i,j-1,k,1) 
               end do
            end do
         end do

!...Multiplikation mit rm0f0

         if ( rand_zu ) then
            oben = dmjo -1
         else
            oben = dmjo
         end if
         
         do k = dmku, dmko
            do j = dmju, oben
               do i = dmiu, dmio
                  erg_vekt(i,j,k,1) = erg_vekt(i,j,k,1) +  &
                      rm0f0(i,j,k,1,1)*vekt(i,j+1,k,1) 
               end do
            end do
         end do
         
!...Multiplikation mit rm00b

         do k = 1, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  erg_vekt(i,j,k,1) = erg_vekt(i,j,k,1) + &
                      rm00b(i,j,k,1,1)*vekt(i,j,k-1,1) 
               end do
            end do
         end do
       
!...Multiplikation mit rm00f
       
         if ( rand_symo ) then
            oben = dmko -1
         else
            oben = dmko
         end if
         
         do k = dmku, oben
            do j = dmju, dmjo
               do i = dmiu, dmio
                  erg_vekt(i,j,k,1) = erg_vekt(i,j,k,1) +  &
                      rm00f(i,j,k,1,1)*vekt(i,j,k+1,1) 
               end do
            end do
         end do
       
!...Umkopieren von erg_vekt auf v

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  v(i, j, k, 1) = erg_vekt (i, j, k, 1 )
               end do
            end do
         end do

!...Datenaustausch fuer v

         call System_Change (v, ierror )

!...Multiplikation mit d1

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, dmio
                  erg_vekt(i, j, k, 1) = d1(i, j, k, 1, 1) * v(i, j, k, 1)
               end do
            end do
         end do

!...MUltiplikation mit d2

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = 1, dmio
                  erg_vekt(i, j, k, 1) = d2(i, j, k, 1, 1) * v(i-1, j, k, 1 )
               end do
            end do
         end do

!...Mutlplikation mit d3

         if ( rand_ab ) then
            oben = dmio -1
         else
            oben = dmio
         end if

         do k = dmku, dmko
            do j = dmju, dmjo
               do i = dmiu, oben
                  erg_vekt(i, j, k, 1 ) = d3(i, j, k, 1, 1) * v(i+1, j, k, 1 )
               end do
            end do
         end do



         ierror = 0

         return
         end














