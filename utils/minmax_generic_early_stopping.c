#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "minmax_generic.h"

extern  int* nummethods;

void minmax_early_stopping ( int r_id, struct emethod **em, int outlier_factor, char *filename );

float bico(int n, int k); /* computes binomial coefficient (n, k) */
double factln(int n);     /* computes ln(n!) */
double gammaln(float xx); /* computes ln(Gamma(x)) */



/**********************************************************************/
void minmax_early_stopping ( int r_id, struct emethod **em, int outlier_factor, char *filename )
/**********************************************************************/
{
   int i, j, k;
   FILE *outf;
   struct lininf tline;
   int nhat, nImpl = nummethods[r_id];
   double Fhat_t, Ghat_t;
   double *timpl; /* array with execution times of each implementation */
   double *timpl_norm; /* normalized execution times of each implementation */
   double tmax; 
   int iMeas, iImpl;
   double eps= 0.05;
   double alpha = 0.1;

   timpl = (double *) malloc ( nummethods[r_id] * sizeof(double) );
   timpl_norm = (double *) malloc ( nummethods[r_id] * sizeof(double) );
   iMeas = 3;

   /* compute global min t(i) (for all implementations?) */
   minmax_filter_timings ( r_id, em, outlier_factor, iMeas );

   /* something like minmax_calc_decision  ( r_id, em, outlier_fraction ); */ 
   /* does not work like that should return min for each method */

   /* loop over number of implementations */
   for ( iImpl=3; iImpl<nImpl; iImpl++  ) {

      /* get maximum execution time for normalization */
      tmax = 0;
      for ( j=0; j<iImpl; j++){
          if ( timpl[j] > tmax ) tmax = timpl[j];
      }

      /* normalize execution times */
      /* count number of implementations <= 1-eps*/
      nhat = 0;
      for ( j=0; j<iImpl; j++  ) {
         timpl_norm[j] = timpl[j] / tmax;
         if ( timpl_norm[j] <= 1-eps ) nhat++;
      }

     /* apply early stopping criterion */
     Fhat_t = nhat / iImpl;
     Ghat_t = bico( ceil( nImpl*Fhat_t ), iImpl ) / bico(nImpl, iImpl);
     if (Ghat_t <= alpha)  break;
   }

   /* output iImpl, sum(execution_time)  */
   printf("request %d: ESC fulfilled after iImpl=%d\n", r_id, iImpl);
   printf("request %d: in %d\n", r_id, iImpl);


   //outf = fopen(filename, "w");
   //if ( NULL == outf ) {
   //    printf("calc_perit_filtered: could not open %s for writing\n", filename);
   //    exit (-1);
   //}

//    for (j=0; j<nummethods[r]; j++ ) {
//       for ( k=0; k<nummeas; k++ ) {
//          TLINE_INIT(tline);
//          for (i=0; i< numprocs; i++ ) {
//       	    if ( !em[i][j].em_poison[k] ) {
//       	        TLINE_MIN(tline, em[i][j].em_time[k], i);
//       	        TLINE_MAX(tline, em[i][j].em_time[k], i);
//       	    }
//       	    else {
//       	        em[i][j].em_cnt_outliers++;
//       	    }
//          }
//          //fprintf (outf, "%3d %8.4lf %3d %8.4lf %3d\n", 
//       	 //   j, tline.min, tline.minloc, tline.max, 
//       	 //   tline.maxloc );
//       }
//    }
// 
// 
//   for ( i=0; i< numprocs; i++ ) {
//      fprintf(outf, "# %d: ", i);
//      for ( j=0; j< nummethods[r]; j++ ) {
//           fprintf(outf, "%d ", em[i][j].em_cnt_outliers);
//      }
//      fprintf(outf, "\n");
//   }

  fclose (outf);
  return;

   free( timpl );
   free( timpl_norm );
}


float bico(int n, int k)
 /* Returns the binomial coefficient (n, k) */
{
   return floor(0.5+exp(factln(n)-factln(k)-factln(n-k)));
   // The floor function cleans up roundoårror for smaller values of n and k.
}

double factln(int n)
// Returns ln(n!).
{
static float a[101]; //A static array is automatically initialized to zero.
if (n < 0) printf("Negative factorial in routine factln");
if (n <= 1) return 0.0;
if (n <= 100) return a[n] ? a[n] : (a[n]=gammaln(n+1.0)); //In range of table.
else return gammaln(n+1.0); //Out of range of table.
}


double gammaln(float xx)
{
  // Returns the value ln[Gamma(xx)] for xx > 0.

  if (xx > 0) {
    double x, y, tmp, ser;
    static double cof[6] = { 76.18009172947146,
			     -86.50532032941677,
			     24.01409824083091,
			     -1.231739572450155,
			     0.1208650973866179e-2,
			     -0.5395239384953e-5
    };
    int j;

    y = x = xx;
    tmp = x + 5.5;
    tmp -= (x + 0.5) * log(tmp);
    ser = 1.000000000190015;
    for (j = 0; j <= 5; j++)
      ser += cof[j] / ++y;

    return (-tmp + log(2.5066282746310005 * ser / x));

  } else {
    return 1.0f;
  }
}




