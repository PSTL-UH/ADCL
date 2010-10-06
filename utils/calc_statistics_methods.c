/*
 * Copyright (c) 2009           High Performance Computing Center Stuttgart. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "minmax_generic_parse_file.h"


void init_cluster_vars(const int ndata, double *data, double outlier_fraction);
int HierarchicalClusterAnalysis(char metric, int transpose, char method,
   double* avg, int* nfilt);
void free_cluster_vars();

/**************************************************************************************************/
/**************************************************************************************************/
/* HEURISTIC                                                                                      */
/**************************************************************************************************/
/**************************************************************************************************/

/**************************************************************************************************/
void filter_heuristic ( int nmeas, int outlier_factor, double *time, 
   double *avg, double *avg_filtered, double *perc_filtered )
/**************************************************************************************************/
/* determines average, filtered average and percentage of filtered measurements for array of      */
/* measurements time                                                                              */ 
/* IN                                                                                             */
/* nmeas          - #measurements used for evaluation                                             */
/* outlier_factor - measurements larger than outlier_factor * min are considered as outliers      */
/* time           - array of measurements                                                         */
/* OUT                                                                                            */
/* avg            - computed mean                                                                 */
/* avg_filtered   - filtered mean                                                                 */
/* perc_filtered  - percentage of filtered data                                                   */
/**************************************************************************************************/
/* based on em_avg, em_avg_filtered, and em_perc_filtered, the maximum on each proc and the  */
/* total minimum are computed */
{
   int k, cnt_outliers;
   double sum, sum_filtered, min;

   sum = 0.0;
   sum_filtered = 0.0;
   cnt_outliers= 0;

   /* Determine the min value for */
   for ( min=1.0E15, k=0; k<nmeas; k++ ) {
      if ( time[k] < min ) { min = time[k]; }
   }

   /* Count how many values are N times larger than the min and
      mark those as outliers, sum up execution times of other values */
   for ( k=0; k<nmeas; k++ ) {
      sum += time[k]; 
      if ( time[k] > (outlier_factor * min) ) {
          //em[iproc][method_id].em_poison[k] = 1;   // set to use minmax_calc_per_iteration */
          cnt_outliers++;
#ifdef DEBUG
            printf("#%d: request %d method %d meas. %d is outlier %lf min %lf\n",
                   iproc, r_id, method_id, k,  time[method_id], min );
#endif
      }
      else {
         sum_filtered += time[k];
      }
   }

   /* calculate average (filtered) time and outlier percentage */
   *avg           = sum / nmeas; 
   *avg_filtered  = sum_filtered / (nmeas - cnt_outliers);
   *perc_filtered = (100. * cnt_outliers) / nmeas;

   return; 
}

/**************************************************************************************************/
void minmax_heuristic_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, 
   int nprocs, int outlier_factor, int outlier_fraction ) 
/**************************************************************************************************/
/* applies local flavor heuristic                                                                 */
/* - computes local (filtered) averages                                                           */
/* - maximizes over processors                                                                    */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas          - #measurements used for evaluation                                             */
/* idx_start      - offset of implementation                                                      */
/* em             - emethod object,  em[nobjects][nprocs][nimpl]                                  */
/* nprocs         - number of processes                                                           */
/* outlier_factor - measurements larger than outlier_factor * min are considered as outliers      */
/* outlier_fraction - accepted percentage of outliers                                             */
/**************************************************************************************************/
{
   int iproc, method_id;
   double *filt, *unf, *perc;

   unf   = (double *) malloc ( nimpl * sizeof(double) );
   filt  = (double *) malloc ( nimpl * sizeof(double) );
   perc  = (double *) malloc ( nimpl * sizeof(double) );

   /* compute (filtered) averages for each method on each process */
   for (iproc=0; iproc<nprocs; iproc++ ) {
      for ( method_id=0; method_id<nimpl; method_id++ ) {
         filter_heuristic ( nmeas, outlier_factor, em[iproc][method_id].em_time,  
             &em[iproc][method_id].em_avg, &em[iproc][method_id].em_avg_filtered, &em[iproc][method_id].em_perc_filtered );
      }
   }
   
   for (iproc=0; iproc<nprocs; iproc++ ) {
       printf("filt. avg. %lf avg %lf perc. %lf\n",  em[iproc][method_id].em_avg, em[iproc][method_id].em_avg_filtered, 
           em[iproc][method_id].em_perc_filtered );
   }

   /* calculate for each implementation max over all procs */
   for (method_id=0; method_id<nimpl; method_id++ ) {
       perc[method_id] = 0.0;
       unf[method_id]  = 0.0;
       filt[method_id] = 0.0;
       for (iproc=0; iproc<nprocs; iproc++ ) {
           MAX ( unf[method_id],  em[iproc][method_id].em_avg           );
           MAX ( filt[method_id], em[iproc][method_id].em_avg_filtered  );
           MAX ( perc[method_id], em[iproc][method_id].em_perc_filtered );
       }
   }

   printf("Local heuristic:\n"); 
   calc_decision_heuristic ( nimpl, idx_start, unf, filt, perc, outlier_fraction );

   free (unf);
   free (filt);
   free (perc);

   return;
}

/**************************************************************************************************/
void minmax_heuristic_collective ( int nimpl, int nmeas, int idx_start, double **timings, 
   int nprocs, int outlier_factor, int outlier_fraction ) 
/**************************************************************************************************/
/* applies collective flavor heuristic                                                            */
/* - computes (filtered) averages                                                                 */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas          - #measurements used for evaluation                                             */
/* idx_start      - offset of implementation                                                      */
/* timings        - "global" execution times                                                      */
/* nprocs         - number of processes                                                           */
/* outlier_factor - measurements larger than outlier_factor * min are considered as outliers      */
/* outlier_fraction - accepted percentage of outliers                                             */
/**************************************************************************************************/
{
   int method_id;
   double *filt, *unf, *perc;

   unf      = (double *) malloc ( nimpl * sizeof(double) );
   filt     = (double *) malloc ( nimpl * sizeof(double) );
   perc     = (double *) malloc ( nimpl * sizeof(double) );

   for ( method_id=0; method_id<nimpl; method_id++ ) {
      /* compute (filtered) averages for each method */
      filter_heuristic ( nmeas, outlier_factor, timings[method_id],  
         &unf[method_id], &filt[method_id], &perc[method_id] );
   }

   printf("Collective heuristic:\n"); 
   calc_decision_heuristic ( nimpl, idx_start, unf, filt, perc, outlier_fraction );

   free (unf);
   free(filt);
   free (perc);

   return;
}

/**************************************************************************************************/
void  calc_decision_heuristic ( int nimpl, int idx_start, double *unf, double *filt, double *perc,  
   int outlier_fraction ) 
/**************************************************************************************************/
/* does the last step of the heuristic (finding the best implementation out of an array with      */
/* estimated execution times for each implementation) and prints results                          */
/**************************************************************************************************/
{
   int method_id;
   double timpl, tmin, tmin_heuristic;
   int  minloc, minloc_heuristic;
   char isfilt_str[2], isfilt_min[2];

   tmin           = 1.E15;
   tmin_heuristic = 1.E15;
   /* calculate minimum communication time over all implementations */
   for ( method_id=0; method_id<nimpl; method_id++ ) {
      /* min without heuristic */
      if ( tmin > unf[method_id] ) {
           tmin   = unf[method_id];  
           minloc = method_id;
      }

      /* min  with heuristic */
      if ( perc[method_id] > outlier_fraction ) {
         strcpy(isfilt_str, "uf");
         timpl = unf[method_id];
      }
      else {
         strcpy(isfilt_str, "f");
         timpl = filt[method_id]; 
      }

      if ( tmin_heuristic >  timpl ) {
         tmin_heuristic = timpl;
         minloc_heuristic = method_id;
         strcpy(isfilt_min, isfilt_str);
      } 

      printf ("%d: result: %lf : %s :  unfiltered: %lf filtered: %lf perc: %lf\n",
              method_id, timpl, isfilt_str,  unf[method_id], filt[method_id], perc[method_id] );
   }

   printf("Without filtering: winner is %d\n", minloc + idx_start ); 
   printf("With    filtering: winner is %d (%s)\n\n", minloc_heuristic + idx_start, isfilt_str);

   return;
}


/**************************************************************************************************/
/**************************************************************************************************/
/* INTER-QUARTILE RANGE                                                                           */
/**************************************************************************************************/
/**************************************************************************************************/

/**************************************************************************************************/
static int tcompare ( const void *p, const void* q )
/**************************************************************************************************/
{
    double *a, *b;
    a = (double *) p;
    b = (double *) q;

    if ( *a < *b ) {
	return -1;
    }
    if ( *a == *b ) {
	return 0;
    }

    return 1;
}


/**************************************************************************************************/
void minmax_iqr_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, int nprocs ) 
/**************************************************************************************************/
{
    int iproc, method_id;
    double timpl, tmin, perc;
    int    minloc;


    /* Calculate the median of all measurement series */
    for (iproc=0; iproc < nprocs; iproc++ ) {
	for ( method_id=0; method_id<nimpl; method_id++ ) {
           filter_iqr ( nmeas, em[iproc][method_id].em_time, &em[iproc][method_id].em_avg_filtered,
              &em[iproc][method_id].em_perc_filtered );
	}	
    }

    /* compute max over procs and min over implementations */    
   printf("Local IQR:\n"); 
   tmin = 1.E15;
   for (method_id=0; method_id<nimpl; method_id++ ) {
       perc  = 0.0;
       timpl = 0.0;
       for (iproc=0; iproc<nprocs; iproc++ ) {
           MAX ( timpl, em[iproc][method_id].em_avg_filtered  );
           MAX ( perc, em[iproc][method_id].em_perc_filtered );
       }
       printf("%d : avg. filtered %lf perc. filtered %lf\n", method_id, timpl, perc );


       if ( tmin >  timpl ) {
          tmin = timpl;
          minloc = method_id;
       }
    }
    
    printf("Standard formula winner is %d\n", idx_start + minloc );

    return;
}

/**************************************************************************************************/
void minmax_iqr_collective ( int nimpl, int nmeas, int idx_start, double **timings, int nprocs ) 
/**************************************************************************************************/
/* applies collective flavor IQR                                                                  */
/* - computes (filtered) averages                                                                 */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas          - #measurements used for evaluation                                             */
/* idx_start      - offset of implementation                                                      */
/* timings        - "global" execution times                                                      */
/* nprocs         - number of processes                                                           */
/**************************************************************************************************/
{
    int method_id;
    double timpl, tmin, perc;
    int    minloc;

    printf("Collective IQR:\n"); 
    /* filter and compute min */    
    tmin = 1.E15;
    for ( method_id=0; method_id<nimpl; method_id++ ) {
       /* compute (filtered) averages for each method */
       filter_iqr ( nmeas,  timings[method_id], &timpl, &perc );
       printf("%d : avg. filtered %lf perc. filtered %lf\n", method_id, timpl, perc );

       if ( tmin >  timpl ) {
          tmin = timpl;
          minloc = method_id;
       }
    }

    printf("Standard formula winner is %d\n", idx_start + minloc );

    return;
}

/**************************************************************************************************/
void filter_iqr ( int nmeas, double *time, double *avg_filtered, double *perc_filtered )
/**************************************************************************************************/
/* determines filtered average and percentage of filtered measurements for array of               */
/* measurements                                                                                   */ 
/* IN                                                                                             */
/* nmeas          - #measurements used for evaluation                                             */
/* time           - array of measurements                                                         */
/* OUT                                                                                            */
/* avg_filtered   - filtered mean                                                                 */
/* perc_filtered  - percentage of filtered data                                                   */
/**************************************************************************************************/
{
   int    imeas;
   double sum_filtered;
   int    idx_m, idx_q;
   double median, q1, q3, iqr;
   double llimit, ulimit;
   int    cnt_outliers;

   /* sort data */
   qsort ( time, nmeas, sizeof(double), tcompare );

   /* calculate q1, median, q3 */
   if ( nmeas % 4 == 0 ) {
       median = 0.5 * ( time[ nmeas/2-1 ]  + time[ nmeas/2 ] ); 
       q1     = 0.5 * ( time[ nmeas/4-1 ]  + time[ nmeas/4 ] );
       q3     = 0.5 * ( time[ (nmeas*3)/4-1 ]  + time[ (nmeas*3)/4 ] );
   }
   else if ( nmeas % 4 == 1 ){
       idx_q  = (nmeas - 1)/4; 
     
       median = time[ (nmeas-1)/2 ]; 
       q1     = 0.5 * ( time[   idx_q-1 ] + time [   idx_q   ] ) ;
       q3     = 0.5 * ( time[ 3*idx_q   ] + time [ 3*idx_q+1 ] ) ;
   }
   else if ( nmeas % 4 == 2 ){
       idx_m  = (nmeas - 2)/2; 
       idx_q  = (nmeas - 2)/4; 

       median = 0.5 * ( time[ idx_m ]  + time[ idx_m+1 ] ); 
       q1     = time[ idx_q ] ;
       q3     = time[ nmeas-idx_q-1 ];
   }
   else {
       median = time[ (nmeas-1)/2 ]; 
       idx_q  = (nmeas - 3)/4; 
       q1     = time[ idx_q ] ;
       q3     = time[ nmeas-idx_q-1 ];
   }

#ifdef DEBUG
   printf("%d: q1 %lf, median %lf, q3 %lf\n", nmeas, q1, median, q3);
#endif

   /* calculate IQR and limits */	    
   iqr = q3 - q1; 
   llimit = q1  - (1.5 * iqr);
   ulimit = q3  + (1.5 * iqr);
   
   /* calculate filtered sum and #outliers */ 
   cnt_outliers = 0;
   sum_filtered = 0.;
   
   for ( imeas=0; imeas < nmeas; imeas++ ) {
       if ( time[imeas] < llimit || 
            time[imeas] > ulimit ) {
           cnt_outliers++;
       }
       else {
           sum_filtered += time[imeas];
       }
   }

   *avg_filtered  = sum_filtered / (nmeas - cnt_outliers);
   *perc_filtered = (100. * cnt_outliers) / nmeas;	    

   return;
}


/**************************************************************************************************/
void test_filter_iqr ( )
/**************************************************************************************************/
/* unit test for filter_iqr                                                                       */
/**************************************************************************************************/
{
    double *test;
    double avg_filt, perc_filt;

    test = malloc (12 * sizeof(double));
    test[0] = 0; test[1] = 1; test[2] = 2; test[3] = 3; test[4] = 4; test[5] = 5;
    test[6] = 6; test[7] = 7; test[8] = 8; test[9] = 9; test[10] = 10; test[11] = 11;

    filter_iqr (8, test, &avg_filt, &perc_filt);
    filter_iqr (9, test, &avg_filt, &perc_filt);
    filter_iqr (10, test, &avg_filt, &perc_filt);
    filter_iqr (11, test, &avg_filt, &perc_filt);

    free (test);

    /* output should be:
       8: q1 1.500000, median 3.500000, q3 5.500000
       9: q1 1.500000, median 4.000000, q3 6.500000
       10: q1 2.000000, median 4.500000, q3 7.000000
       11: q1 2.000000, median 5.000000, q3 8.000000
    */

    return;
}

/**************************************************************************************************/
/**************************************************************************************************/
/* CLUSTER                                                                                        */
/**************************************************************************************************/
/**************************************************************************************************/

/**************************************************************************************************/
void minmax_cluster_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, int nprocs, 
   double outlier_fraction ) 
/**************************************************************************************************/
{
    int iproc, method_id;
    double timpl, tmin, perc;
    int    minloc;
    int nfilt;
    char genemetric = 'e'; 
    char method = 'a';

    /* Calculate the median of all measurement series */
    for (iproc=0; iproc < nprocs; iproc++ ) {
	for ( method_id=0; method_id<nimpl; method_id++ ) {
            init_cluster_vars(nmeas, em[iproc][method_id].em_time, outlier_fraction);
	    HierarchicalClusterAnalysis(genemetric, 0, method, &(em[iproc][method_id].em_avg_filtered), &nfilt);
            em[iproc][method_id].em_perc_filtered = (100. * nfilt) / nmeas;
            free_cluster_vars();
	}	
    }

    /* compute max over procs and min over implementations */    
   printf("Local Cluster:\n"); 
   tmin = 1.E15;
   for (method_id=0; method_id<nimpl; method_id++ ) {
       perc  = 0.0;
       timpl = 0.0;
       for (iproc=0; iproc<nprocs; iproc++ ) {
           MAX ( timpl, em[iproc][method_id].em_avg_filtered  );
           MAX ( perc, em[iproc][method_id].em_perc_filtered );
       }
       printf("%d : avg. filtered %lf perc. filtered %lf\n", method_id, timpl, perc );


       if ( tmin >  timpl ) {
          tmin = timpl;
          minloc = method_id;
       }
    }
    
    printf("Cluster analysis winner is %d\n", idx_start + minloc );

    return;
}

/**************************************************************************************************/
void minmax_cluster_collective ( int nimpl, int nmeas, int idx_start, double **timings, int nprocs, 
     double outlier_fraction ) 
/**************************************************************************************************/
/* applies collective flavor cluster                                                              */
/* - computes (filtered) averages                                                                 */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas          - #measurements used for evaluation                                             */
/* idx_start      - offset of implementation                                                      */
/* timings        - "global" execution times                                                      */
/* nprocs         - number of processes                                                           */
/**************************************************************************************************/
{
    int method_id;
    double timpl, tmin, perc;
    int    minloc;
    int nfilt;
    char genemetric = 'e'; 
    char method = 'a';

    printf("Collective Cluster:\n"); 
    /* filter and compute min */    
    tmin = 1.E15;
    for ( method_id=0; method_id<nimpl; method_id++ ) {
       /* compute (filtered) averages for each method */
       init_cluster_vars(nmeas, timings[method_id], outlier_fraction );
       HierarchicalClusterAnalysis(genemetric, 0, method, &timpl, &nfilt);
       perc = (100. * nfilt) / nmeas;
       free_cluster_vars();
       printf("%d : avg. filtered %lf perc. filtered %lf\n", method_id, timpl, perc );

       if ( tmin >  timpl ) {
          tmin = timpl;
          minloc = method_id;
       }
    }

    printf("Cluster analysis winner is %d\n", idx_start + minloc );

    return;
}


/**************************************************************************************************/
/**************************************************************************************************/
/* ROBUST                                                                                         */
/**************************************************************************************************/
/**************************************************************************************************/

#if defined(GSL) || defined(NR)
/**************************************************************************************************/
void minmax_robust_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, int nprocs ) 
/**************************************************************************************************/
{
    int iproc, method_id;
    double timpl, tmin;
    int    minloc;
    double nu, sigma, val;

    /* Caculate ML-estimate of mu and sigma for t model */
    /* 30 measurements are not enough to estimate nu from set (nu=0 as input),
       so choose some reasonable value for nu, which is 4.0 or 6.0 */
#ifdef NU6
    nu = 6.0;
#else
    nu = 4.0;
#endif    
    //double avg, avgmin, romin, romax ;
    for ( method_id=0; method_id< nimpl; method_id++ ) {
        //avg = 0.0; romin=1e10; romax=0;
        for (iproc=0; iproc < nprocs; iproc++ ) {
	    ml ( nmeas, em[iproc][method_id].em_time, &nu, &(em[iproc][method_id].em_avg_filtered) , 
		 &sigma, &val );
            //avg = avg + em[iproc][method_id].em_avg_filtered;
            //if (em[iproc][method_id].em_avg_filtered < romin) romin = em[iproc][method_id].em_avg_filtered;
            //if (em[iproc][method_id].em_avg_filtered > romax) romax = em[iproc][method_id].em_avg_filtered;
            // printf("proc=%d, method=%d, mu=%lf, sigma=%lf\n", iproc, method_id, em[iproc][method_id].em_avg_filtered, sigma);
            
	}
        //printf("min: %lf avg %lf max %lf\n", romin, avg/numprocs, romax);
        //if (avg < avgmin) avgmin=avg;
    }

   //printf("Robust formula winner has minimum avg %fl\n", avgmin / nummeas);
    
    
    /* compute max over procs and min over implementations */    
   printf("Robust statistics:\n"); 
   tmin = 1.E15;
   for (method_id=0; method_id<nimpl; method_id++ ) {
       timpl = 0.0;
       for (iproc=0; iproc<nprocs; iproc++ ) {
           MAX ( timpl, em[iproc][method_id].em_avg_filtered  );
       }
       printf("%d : avg. %lf\n", method_id, timpl );

       if ( tmin >  timpl ) {
          tmin = timpl;
          minloc = method_id;
       }
    }
    
    printf("Robust statistics winner nu = %lf is %d\n", nu, idx_start + minloc );

    return;
}

/**************************************************************************************************/
void minmax_robust_collective ( int nimpl, int nmeas, int idx_start, double **timings, int nprocs ) 
/**************************************************************************************************/
/* applies collective flavor cluster                                                              */
/* - computes (filtered) averages                                                                 */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas          - #measurements used for evaluation                                             */
/* idx_start      - offset of implementation                                                      */
/* timings        - "global" execution times                                                      */
/* nprocs         - number of processes                                                           */
/**************************************************************************************************/
{
    int method_id;
    double timpl, tmin;
    int    minloc;
    double nu, sigma, val;

    printf("Collective robust statistics:\n"); 
    /* Caculate ML-estimate of mu and sigma for t model */
    /* 30 measurements are not enough to estimate nu from set (nu=0 as input),
       so choose some reasonable value for nu, which is 4.0 or 6.0 */
#ifdef NU6
    nu = 6.0;
#else
    nu = 4.0;
#endif    
    /* filter and compute min */    
    tmin = 1.E15;
    for ( method_id=0; method_id<nimpl; method_id++ ) {
       ml ( nmeas, timings[method_id], &nu, &timpl, &sigma, &val );
       printf("%d : avg. %lf\n", method_id, timpl );

       if ( tmin >  timpl ) {
          tmin = timpl;
          minloc = method_id;
       }
    }

    printf("Robust statistics winner nu = %lf is %d\n", nu, idx_start + minloc );

    return;
}

#endif









