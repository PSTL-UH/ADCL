#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "minmax_generic.h"

extern int* nummethods;
extern int numprocs;
extern int outlier_factor;
extern int nummeas; 
 
void minmax_early_stopping ( int r_id, int impl_id, struct emethod **em); //, char *filename );

float bico(int n, int k); /* computes binomial coefficient (n, k) */
double factln(int n);     /* computes ln(n!) */
double gammaln(float xx); /* computes ln(Gamma(x)) */



/**************************************************************************************************/
void minmax_early_stopping ( int r_id, int impl_id, struct emethod **em) //, char *filename )
/* early stopping criterion (ESC) from Vuduc et al., "Statistical Methods for empirical 
   Search-Based Methods", but for measurements and not for implementations!!!                     */
/**************************************************************************************************/
{
   int i, j, k, p;
   FILE *outf;
   int nhat;
   double Fhat_t, Ghat_t;
   double *perf;      /* array with execution times of each implementation */
   double *perf_norm; /* normalized execution times of each implementation */
   double max; 
   int iMeas;
   double time; 
   double eps= 0.05;
   double alpha = 0.1;

   perf = (double *) malloc ( nummeas * sizeof(double) );
   perf_norm = (double *) malloc ( nummeas * sizeof(double) );
   if ( NULL == perf || NULL == perf_norm ) {
       printf("minmax_early_stopping: could not allocate memory\n");
       exit (-1);
   }
   /* compute performance as inverse of maximum execution time for a given implementation i_0 
      over all processes j, i.e. perf(iMeas) = 1 / max_{j=0)^numprocs t(i_0,j,iMeas) */
   for ( iMeas=0; iMeas<nummeas; iMeas++) {
      max = 0.;
      for ( p=0; p<numprocs; p++) {
         if ( max < em[p][impl_id].em_time[iMeas] )  max = em[p][impl_id].em_time[iMeas];
      }
      perf[iMeas] = 1./max;
   }
   
   /* loop over number of implementations */
   for ( iMeas=3; iMeas<nummeas; iMeas++  ) {
      /* compute global min t(i) (for all implementations?) */

      /* prepare for normalization: get maximum of performance so far */
      max = 0.;
      for (j=0; j<iMeas; j++ ) {
         if ( max < perf[j] )  max = perf[j];
      }
      /* normalize */
      for (j=0; j<iMeas; j++ ) {
         perf_norm[j] = perf[j]/max;
      }

      /* count number of implementations <= 1-eps*/
      nhat = 0;
      for ( j=0; j<iMeas; j++  ) {
         if ( perf_norm[j] <= 1-eps ) nhat++;
      }

     /* apply early stopping criterion */
     Fhat_t = (double) nhat / (double) iMeas;
     Ghat_t = bico( ceil( nummeas*Fhat_t ), iMeas ) / bico(nummeas, iMeas);
     if (Ghat_t <= alpha)  break;
   }

   /* output iImpl, sum(execution_time)  */
   printf("request %d: ESC fulfilled after iImpl=%d\n", r_id, iMeas);
   time = 0.; 
   for (j=0; j<iMeas; j++ ) {
      time += 1./perf[j];
   }
   printf("request %d: in %lf\n", r_id, time);


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
// fclose (outf);

   free( perf );
   free( perf_norm );
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




