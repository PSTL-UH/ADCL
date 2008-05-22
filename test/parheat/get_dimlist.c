/* get_dimlist.c: breaks down the node numbers to a set of   */
/*                prime numbers to make a good topology      */
/* input:  int num_nodes                                     */
/*         int *dim                                          */
/* output: int get_dimlist                                   */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
#include "parheat.h"

int get_dimlist( int num_nodes, int *ndim, int *dim )
{
  int *primelist;
  int num_primes, max_num_primes, prime_done;
  int dimindex, dimflag;
  int limit, zahl, teiler;
  int val, rest, count;
  int i, imax;

  *ndim = 1;
  for( i=0 ; i<3 ; i++ )
    dim[i] = 1;

  if( num_nodes > 1 )
  {
    max_num_primes = (int)( log( (double)num_nodes ) / log( 2.0 ) + 1.0 );
    primelist = (int *)malloc( max_num_primes*sizeof( int ) );
    for( i=0 ; i<3 ; i++ )
    dim[i]=1;

    count = 0;
    zahl = num_nodes;
    teiler = 2;
    while( ( zahl>1 ) && ( teiler<=num_nodes ) )
    {
      do
      {
        rest = zahl % teiler;
        if( rest == 0 )
        {
          primelist[count++] = teiler;
          zahl = zahl / teiler;
        }
      } while( rest == 0 );
      teiler++;
    }

    *ndim = count;
    if( ( imax=count ) > 3 )
    {
      imax = 3;
      *ndim = 3;
    }
    for( i=0 ; i<imax ; i++ )
      dim[i] = primelist[--count];
    num_primes = count;

    if( num_primes>0 )
    {
      for( i=count-1 ; i>=0 ; i-- )
      {
        dim[2] = dim[2] * primelist[i];
        int_sort( 3, dim );
      }
    }
    free( (void *)primelist );
  }
}
