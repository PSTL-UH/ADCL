/*
 * Copyright (c) 2010           High Performance Computing Center Stuttgart. All rights reserved.
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

#define MAXLINE 128
#define MIN(a,b) ( a < b ) ? a : b
#define MAX(a,b) ( a > b ) ? a : b

/* This program evaluates different statistical methods. Weights, which judge how bad 
   possible ranking errors are, are calculated from the verification runs. Based on the
   actual ranking errors of each statistical method, the maximum percentual error and the
   total percentual error are calculated. */ 


struct minmaxavg {
   double min; 
   double max;
   double avg;
};
typedef struct minmaxavg tminmaxavg; 

struct minmaxavg_array {
    tminmaxavg *impl;
};
typedef struct minmaxavg_array tminmaxavg_array;


double sgn(double x)
{
  if (x < 0)
    return -1;
  else if (x > 0)
    return 1;
  else
    return 0;
} 

/********************************************************************************************************************************/
int minmax_get_lines (char *filename){
/********************************************************************************************************************************/
/* determines number of lines of file "filename" */
    FILE *infd=NULL;
    int nlines=-1;
    char cmd[MAXLINE], line[MAXLINE];


    sprintf(cmd, "wc -l %s > lines.out", filename);
    system(cmd);
    infd = fopen ( "lines.out", "r" );
    if (NULL == infd) {
        printf("Could not open input file %s for reading\n", filename);
        exit (-1);
    }
    fscanf ( infd, "%[^\n]\n", line );
    sscanf ( line, "%d", &nlines );
    system("rm -f lines.out");
    //printf("Detected %s with %d lines\n", filename, nlines);

    return nlines;
}

void read_exectimes_from_file( char* exectimes_file, int nmethods, int nruns, double **timings) { 
    int i, j;
    FILE *infd=NULL;

    /* Read exec times */
    printf("Reading %s ...\n", exectimes_file);
    infd = fopen ( exectimes_file, "r" );
    for (i=0; i<nruns; i++) {
       for (j=0; j<nmethods; j++) {
          fscanf ( infd, "%lf\n", &timings[i][j] );
          // printf("%.12lf\n", timings[i][j]);
       }
    }
    fclose (infd);
 
    return;
}

void read_heuristic_timings_from_file( char* filttimes_file, int nmethods, int nruns, double **timings, 
    double **filt_timings, double **unfilt_timings, double **perc ) {
    int i, j;
    FILE *infd=NULL;

    /* Read exec times */
    printf("Reading %s ...\n", filttimes_file);
    infd = fopen ( filttimes_file, "r" );
    for (i=0; i<nruns; i++) {
       for (j=0; j<nmethods; j++) {
          fscanf ( infd, "%lf : %lf : %lf : %lf\n", &timings[i][j], &filt_timings[i][j],  &unfilt_timings[i][j], &perc[i][j]  );
          //printf("time %lf %lf %lf %lf\n", timings[i][j], filt_timings[i][j], unfilt_timings[i][j], perc[i][j] );
       }
    }
    fclose (infd);
}

void read_filtered_timings_from_file( char* filttimes_file, int nmethods, int nruns, double **timings, double **perc ) {
    int i, j;
    FILE *infd=NULL;

    /* Read exec times */
    printf("Reading %s ...\n", filttimes_file);
    infd = fopen ( filttimes_file, "r" );
    for (i=0; i<nruns; i++) {
       for (j=0; j<nmethods; j++) {
          fscanf ( infd, "%lf : %lf\n", &timings[i][j], &perc[i][j]  );
          //printf("time %lf %lf %lf %lf\n", timings[i][j], perc[i][j] );
       }
    }
    fclose (infd);
}

void read_timings_from_file( char* filttimes_file, int nmethods, int nruns, double **timings ) {
    int i, j;
    FILE *infd=NULL;

    /* Read exec times */
    printf("Reading %s ...\n", filttimes_file);
    infd = fopen ( filttimes_file, "r" );
    for (i=0; i<nruns; i++) {
       for (j=0; j<nmethods; j++) {
          fscanf ( infd, "%lf\n", &timings[i][j] );
          //printf("time %lf %lf %lf %lf\n", timings[i][j] );
       }
    }
    fclose (infd);
}

double get_minimum ( int n, double* array ) {
   int i; 
   double min; 

   min = array[0];
   for ( i=1; i<n; i++) {
      if ( min > array[i] ) min=array[i];
   }

   return min;
} 

void normalize ( int nmethods, int nruns, double **timings, tminmaxavg *ref_time, tminmaxavg *loss) {
    /* nmethods  - #implementations (IN)
       nruns     - #verification runs (IN)
       timings   - array of size [nruns][nmethods] with execution times of 
                   verification runs (IN)
       ref_time  - array of size [nmethods] derived from timings  (OUT)
       loss      - array of size [nmethods] with percentual losses based on 
                   the reference run when comparing one implementation to the 
                   best (OUT)  */
    int i, j; 
    double min_ref_time_min, min_ref_time_max, min_ref_time_avg;

    /* compute minimum, maximum and average run times of reference runs */
    min_ref_time_min = 1.E+15;  
    min_ref_time_max = 1.E+15;
    min_ref_time_avg = 1.E+15;
    for ( j=0; j<nmethods; j++) {
        ref_time[j].min = timings[0][j];      
        ref_time[j].max = timings[0][j];  
        ref_time[j].avg = timings[0][j];  
       
       for (i=1; i<nruns; i++) {
          ref_time[j].min = MIN ( (ref_time[j]).min, timings[i][j] );      
          ref_time[j].max = MAX ( (ref_time[j]).max, timings[i][j] );  
          ref_time[j].avg += timings[i][j] ;  
       }
       ref_time[j].avg /= nruns;

       /* calc minimum values for computation of losses */
       min_ref_time_min = MIN ( min_ref_time_min, ref_time[j].min );
       min_ref_time_max = MIN ( min_ref_time_max, ref_time[j].max );
       min_ref_time_avg = MIN ( min_ref_time_avg, ref_time[j].avg ); 
    }


    /* compute losses */
    for ( j=0; j<nmethods; j++) {
       loss[j].min = (ref_time[j].min-min_ref_time_min) / min_ref_time_min * 100; 
       loss[j].max = (ref_time[j].max-min_ref_time_max) / min_ref_time_max * 100; 
       loss[j].avg = (ref_time[j].avg-min_ref_time_avg) / min_ref_time_avg * 100; 
    }
  
    return;
}

void calc_stability_matrix( int nmethods, tminmaxavg *ref_time, double **stability ) { 
    int i, j; 

    for ( i=0; i<nmethods; i++) {
       for ( j=0; j<nmethods; j++) { 
          if ( ref_time[j].min > ref_time[i].max || ref_time[j].max < ref_time[i].min ) {
               stability[i][j] = 1.;
          }
          else { 
               stability[i][j] = 0.;
          }
       }
    }
    return; 
}

void calc_ref_weights ( int nmethods, double **stability, tminmaxavg *ref_loss, 
    tminmaxavg_array *ref_weights )  {
    /* calculates reference weights (how bad a wrong ranking of a statistical method is) 
       nmethods    - #implementations (IN)
       stability   - stability matrix (IN)
       ref_loss    - percentual losses of reference runs compared to best one
       ref_weights - weights for verification runs */
    int i, j; 

    for ( i=0; i<nmethods; i++) {
       for ( j=0; j<nmethods; j++) { 
          /* min */
          if ( ref_loss[i].min - ref_loss[j].max < ref_loss[i].max - ref_loss[j].min) {
              ref_weights[i].impl[j].min = (ref_loss[i].min - ref_loss[j].max) * stability[i][j]; 
          }
          else {
              ref_weights[i].impl[j].min = (ref_loss[i].max - ref_loss[j].min) * stability[i][j];
          }
          if ( ref_weights[i].impl[j].min < 0. )  ref_weights[i].impl[j].min = 0.;

          /* max */
          if ( ref_loss[i].min-ref_loss[j].max>ref_loss[i].max-ref_loss[j].min ) {
             ref_weights[i].impl[j].max = (ref_loss[i].min - ref_loss[j].max) * stability[i][j];
          }
          else {
             ref_weights[i].impl[j].max = (ref_loss[i].max - ref_loss[j].min) * stability[i][j];
          }
          if ( ref_weights[i].impl[j].max < 0. )  ref_weights[i].impl[j].max = 0.;

          /* avg */
          ref_weights[i].impl[j].avg = ( ref_loss[i].avg - ref_loss[j].avg ) * stability[i][j];
          if ( ref_weights[i].impl[j].avg < 0. )   ref_weights[i].impl[j].avg = 0.;
       }
    }
   return; 
}     

void calc_errors ( int nmethods, int nruns, double **timings, tminmaxavg *ref_time, tminmaxavg_array *ref_weights,
    tminmaxavg *max_stat_weight, tminmaxavg *sum_stat_weight ) { 
    /* computes the maximum error and the sum of all errors, i.e. overheads caused by wrong rankings of 
       a statistical method based on the weights of the reference runs  
       nmethods  - #implementations (IN)
       nruns     - #runs (IN)
       timings   - array of size [nruns][nmethods] with estimated averages of a statistical method (IN)
       ref_time  - array of size [nmethods] derived from timings (IN)
       ref_weights - array of size [nmethods] with  weights for verification runs  (IN) 
       max_stat_weight -  averaged maximum error/weight of wrong ranking (OUT) 
       sum_stat_weight -  averaged sum of errors/weights of wrong ranking (OUT)  */
    int irun, i, j; 
    tminmaxavg *max_stat_weights;  /* array of size nrun with maximum error/weight of wrong ranking */
    tminmaxavg *sum_stat_weights;  /* array of size nrun with sum of errors/weights of wrong ranking */

    max_stat_weights = (tminmaxavg *) malloc ( nruns * sizeof(tminmaxavg) );
    sum_stat_weights = (tminmaxavg *) malloc ( nruns * sizeof(tminmaxavg) );

    for ( irun=0; irun<nruns; irun++) {
       max_stat_weights[irun].min = 0.;
       max_stat_weights[irun].max = 0.;
       max_stat_weights[irun].avg = 0.;

       sum_stat_weights[irun].min = 0.;
       sum_stat_weights[irun].max = 0.;
       sum_stat_weights[irun].avg = 0.;
       for ( i=0; i<nmethods; i++) {
          for ( j=0; j<nmethods; j++) {
             if ( sgn(timings[irun][i]-timings[irun][j]) != sgn(ref_time[i].avg - ref_time[j].avg) ) {
                 max_stat_weights[irun].min = MAX ( max_stat_weights[irun].min, ref_weights[i].impl[j].min );
                 max_stat_weights[irun].max = MAX ( max_stat_weights[irun].max, ref_weights[i].impl[j].max );
                 max_stat_weights[irun].avg = MAX ( max_stat_weights[irun].avg, ref_weights[i].impl[j].avg );

                 sum_stat_weights[irun].min += ref_weights[i].impl[j].min;
                 sum_stat_weights[irun].max += ref_weights[i].impl[j].max;
                 sum_stat_weights[irun].avg += ref_weights[i].impl[j].avg;
             }
          }
       }
    }

#ifdef DEBUG
   printf("maximum error\n");
   printf("run    min        max       avg\n"); 
   for ( irun=0; irun<nruns; irun++) {
      printf("%d: %lf %lf %lf\n", irun, max_stat_weights[irun].min, max_stat_weights[irun].max, max_stat_weights[irun].avg); 
   }

   printf("sum of errors\n");
   for ( irun=0; irun<nruns; irun++) {
      printf("%d: %lf %lf %lf\n ", irun, sum_stat_weights[irun].min, sum_stat_weights[irun].max, sum_stat_weights[irun].avg); 
   }
#endif

    /* average over runs */
    max_stat_weight[0].min = max_stat_weights[0].min;
    max_stat_weight[0].max = max_stat_weights[0].max;
    max_stat_weight[0].avg = max_stat_weights[0].avg;
  
    sum_stat_weight[0].min = sum_stat_weights[0].min;
    sum_stat_weight[0].max = sum_stat_weights[0].max;
    sum_stat_weight[0].avg = sum_stat_weights[0].avg;  
    for ( irun=1; irun<nruns; irun++) {
       max_stat_weight[0].min += max_stat_weights[irun].min;  
       max_stat_weight[0].max += max_stat_weights[irun].max;  
       max_stat_weight[0].avg += max_stat_weights[irun].avg;  

       sum_stat_weight[0].min += sum_stat_weights[irun].min;  
       sum_stat_weight[0].max += sum_stat_weights[irun].max;  
       sum_stat_weight[0].avg += sum_stat_weights[irun].avg;  
    }
    max_stat_weight[0].min /= nruns;
    max_stat_weight[0].max /= nruns;
    max_stat_weight[0].avg /= nruns;

    sum_stat_weight[0].min /= nruns;
    sum_stat_weight[0].max /= nruns;
    sum_stat_weight[0].avg /= nruns;

    free (max_stat_weights);
    free (sum_stat_weights);

    return;
}

/**********************************************************************/
int main (int argc, char **argv )
/* reads *.out files and inits em_time and em_avg */
{
   int i, j, irun; 
   int nlines; 
   double **exectimes;
   double **timings, **filt_timings, **unfilt_timings, **perc ;
   char exectimes_file[MAXLINE] = "exec_times.txt\0";
   char testcase[MAXLINE];
   int nruns = 3;
   int nmethods;          /* #implementations */
   double time; 
   tminmaxavg *ref_time;  /* min, max, avg execution times of verification runs */
   tminmaxavg *ref_loss;  /* percentual losses of reference runs compared to best one */
   double **stability;    /* stability matrix */
   tminmaxavg_array *ref_weights; /* weights for verification runs */
   int nstats = 9;        /* #statistical methods */
   tminmaxavg *max_error; /* array of size nstats with maximum averaged error/weights of wrong rankings */ 
   tminmaxavg *sum_error; /* array of size nstats with total averaged error/weights of wrong rankings */ 
   FILE *infd=NULL;

   /* parse argument */
   if ( argc != 2 ) { 
      printf("Usage: ./exe  name_of_testcase\n");
   }
   else {
      strcpy ( testcase, argv[1] );
   }

   /* parse command line arguments */
   nlines = minmax_get_lines(exectimes_file);
   nmethods = nlines / nruns;

   /* read data from reference runs and prepare evaluation */
   /* ---------------------------------------------------- */
    exectimes = (double**) malloc ( nruns * sizeof(double*) );
    for (i=0; i<nruns; i++) {
       exectimes[i] = (double*) malloc ( nmethods * sizeof(double) );
    }

   read_exectimes_from_file( exectimes_file, nmethods, nruns, exectimes );

   /* normalize / divide by min */
   ref_time = (tminmaxavg *) malloc ( nmethods * sizeof(tminmaxavg) ); 
   ref_loss = (tminmaxavg *) malloc ( nmethods * sizeof (tminmaxavg) );
   normalize ( nmethods, nruns, exectimes, ref_time, ref_loss );

   /* calc stability matrix */
   stability = (double **) malloc ( nmethods * sizeof (double*) );
   for ( i=0; i<nmethods; i++) {
      stability[i] = (double *) malloc ( nmethods * sizeof (double) );
   }
   calc_stability_matrix( nmethods, ref_time, stability );

   /* calc weights */
   ref_weights = (tminmaxavg_array*) malloc ( nmethods * sizeof(tminmaxavg_array ) );
   for ( i=0; i<nmethods; i++) {
       ref_weights[i].impl = (tminmaxavg *) malloc ( nmethods * sizeof(tminmaxavg) ); 
   }
   calc_ref_weights ( nmethods,  stability, ref_loss, ref_weights ); 

   /* cleanup */
   for ( i=0; i<nmethods; i++) {
       free (stability[i] );
   }
   free ( stability );

    
   /* evaluate different statistical methods               */
   /* ---------------------------------------------------- */

   timings = (double**) malloc ( nruns * sizeof(double*) );
   filt_timings = (double**) malloc ( nruns * sizeof(double*) );
   unfilt_timings = (double**) malloc ( nruns * sizeof(double*) );
   perc = (double**) malloc ( nruns * sizeof(double*) );
   for (i=0; i<nruns; i++) {
      timings[i] = (double*) malloc ( nmethods * sizeof(double) );
      filt_timings[i] = (double*) malloc ( nmethods * sizeof(double) );
      unfilt_timings[i] = (double*) malloc ( nmethods * sizeof(double) );
      perc[i] = (double*) malloc ( nmethods * sizeof(double) );
   }

   max_error = (tminmaxavg *) malloc ( nstats * sizeof(tminmaxavg) ); 
   sum_error = (tminmaxavg *) malloc ( nstats * sizeof(tminmaxavg) ); 

   read_heuristic_timings_from_file("heuristic_local.txt", nmethods, nruns, timings, 
          filt_timings, unfilt_timings, perc );
   /* no_filter */
   calc_errors ( nmethods, nruns, unfilt_timings, ref_time, ref_weights, &max_error[0], &sum_error[0] ); 
   /* heuristic_local */
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[1], &sum_error[1] ); 

   /* heuristic_collective */
   read_heuristic_timings_from_file("heuristic_collective.txt", nmethods, nruns, timings, 
          filt_timings, unfilt_timings, perc );
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[2], &sum_error[2] ); 

   /* iqr_local */
   read_filtered_timings_from_file("iqr_local.txt", nmethods, nruns, timings, perc); 
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[3], &sum_error[3] ); 

   /* iqr_collective */
   read_filtered_timings_from_file("iqr_collective.txt", nmethods, nruns, timings, perc); 
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[4], &sum_error[4] ); 

   /* cluster_local */
   read_filtered_timings_from_file("cluster_local.txt", nmethods, nruns, timings, perc); 
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[5], &sum_error[5] ); 

   /* cluster_collective */
   read_filtered_timings_from_file("cluster_collective.txt", nmethods, nruns, timings, perc); 
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[6], &sum_error[6] ); 

   /* robust_local */
   read_timings_from_file("robust_local.txt", nmethods, nruns, timings); 
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[7], &sum_error[7] ); 

   /* robust_collective */
   read_timings_from_file("robust_collective.txt", nmethods, nruns, timings); 
   calc_errors ( nmethods, nruns, timings, ref_time, ref_weights, &max_error[8], &sum_error[8] ); 

   /* output */
   if ( infd = fopen("max_error.txt", "r"))
   {
     fclose(infd);
   }
   else { /* write header */ 
      infd = fopen("max_error.txt", "w");
      fprintf(infd, "testcase             unf   avg_heur_loc  avg_heur_coll    avg_iqr_loc   avg_iqr_coll",
                    "  avg_clust_loc  avg_clus_coll    avg_rob_loc   avg_rob_coll");
      fprintf(infd, "      ymin_unf  ymin_heur_loc ymin_heur_coll   ymin_iqr_loc  ymin_iqr_coll ymin_clust_loc ymin_clus_coll   ymin_rob_loc  ymin_rob_coll");
      fprintf(infd, "      ymax_unf  ymax_heur_loc ymax_heur_coll   ymax_iqr_loc  ymax_iqr_coll ymax_clust_loc ymax_clus_coll   ymax_rob_loc  ymax_rob_coll\n");
      fclose(infd);

      infd = fopen("sum_error.txt", "w");
      fprintf(infd, "testcase             unf   avg_heur_loc  avg_heur_coll    avg_iqr_loc   avg_iqr_coll",
                    "  avg_clust_loc  avg_clus_coll    avg_rob_loc   avg_rob_coll");
      fprintf(infd, "      ymin_unf  ymin_heur_loc ymin_heur_coll   ymin_iqr_loc  ymin_iqr_coll ymin_clust_loc ymin_clus_coll   ymin_rob_loc  ymin_rob_coll");
      fprintf(infd, "      ymax_unf  ymax_heur_loc ymax_heur_coll   ymax_iqr_loc  ymax_iqr_coll ymax_clust_loc ymax_clus_coll   ymax_rob_loc  ymax_rob_coll\n");
      fclose(infd);
   } 

   //printf("maximum error\n");
   //printf("avg y_min y_max \n");
   infd = fopen("max_error.txt", "a");
   fprintf(infd, "%s", testcase);
   for ( i=0; i<nstats; i++) { fprintf(infd, "%15.6lf", max_error[i].avg); } 
   for ( i=0; i<nstats; i++) { fprintf(infd, "%15.6lf", max_error[i].avg - max_error[i].min); } 
   for ( i=0; i<nstats; i++) { fprintf(infd, "%15.6lf", max_error[i].max - max_error[i].avg); } 
   fprintf(infd, "\n"); 
   fclose (infd);

   infd = fopen("sum_error.txt", "a");
   fprintf(infd, "%s", testcase);
   for ( i=0; i<nstats; i++) { fprintf(infd, "%15.6lf", sum_error[i].avg); } 
   for ( i=0; i<nstats; i++) { fprintf(infd, "%15.6lf", sum_error[i].avg - sum_error[i].min); } 
   for ( i=0; i<nstats; i++) { fprintf(infd, "%15.6lf", sum_error[i].max - sum_error[i].avg); } 
   fprintf(infd, "\n"); 
   fclose (infd);


   /* cleanup */
   free (ref_time);
   free (ref_loss);
   for ( i=0; i<nmethods; i++) {
       free (ref_weights[i].impl);
   }
   free (ref_weights); 
   free (max_error);
   free (sum_error);
    for (i=0; i<nruns; i++) {
       free (exectimes[i]);
      free (timings[i]);
      free (filt_timings[i]);
      free (unfilt_timings[i]);
      free (perc[i]);
    }
   free (exectimes); 
   free (timings );
   free (filt_timings);
   free (unfilt_timings);
   free (perc);

}

