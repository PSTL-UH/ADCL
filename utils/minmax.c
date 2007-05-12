#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "minmax.h"

/* Parameters of the application */
int outlier_factor=3, outlier_fraction=50;
int output_files=0; /* false */
int numprocs=-1, nummethods=-1, nummeas=-1;
int deconly=0;
int filter=0;             

/* Prototypes */
void minmax_init     (int argc, char ** argv, struct emethod ***em );
void minmax_read_input ( struct emethod **em );
void minmax_finalize ( struct emethod ***em ); 

void minmax_filter_timings     ( struct emethod **em, int ofac );
void minmax_calc_per_iteration ( struct emethod **em, char *filename );
void minmax_calc_statistics    ( struct emethod **em, char *filename );
void minmax_clear_poison_field ( struct emethod **em);
void minmax_calc_decision      ( struct emethod **em, int outlier_fraction ); 

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int main (int argc, char **argv )
{
    struct emethod **emethods;

    /* Aquire the required memory and read input files */
    minmax_init ( argc, argv, &emethods );
    minmax_read_input ( emethods );

    /* Second step: calculate statistics, filter data etc. */
    if ( output_files ) {
	minmax_calc_per_iteration ( emethods, "minmax.out" );
    }

    minmax_filter_timings     ( emethods, outlier_factor);
    minmax_calc_decision      ( emethods, outlier_fraction );
    if ( output_files ) {
	minmax_calc_per_iteration ( emethods, "minmax-filtered.out" );
	minmax_calc_statistics    ( emethods, "minmax-median.out");
    }


    /* Free the aquired memory */
    minmax_finalize (&emethods); 
    return ( 0 );
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void minmax_read_input ( struct emethod **emethods ) 
{
    char line[MAXLINE], *basestr;
    char inname[64],  reqstr[64];
    struct lininf tline;
    int ret, req, i, no_line, all_done=0;
    int method, pos, count;
    double time;
    FILE **infd=NULL;

    /* Open the input files */
    infd = (FILE **) malloc ( numprocs * sizeof(FILE *) );
    if ( NULL == infd ) {
	exit (-1);
    }

    for ( i = 0; i < numprocs; i++ ) {
	sprintf( inname, "%d.out", i);
	infd[i] = fopen ( inname, "r" );
	if (NULL == infd[i] ) {
	    printf("Could not open input file %s for reading\n", inname );
	    exit (-1);
	}
    }	


    /* Read infile and store the values in the according emethod structures */
    while ( all_done < numprocs ) {
	TLINE_INIT(tline);
	
	for ( i=0; i< numprocs; i++ ) {
	    no_line = 0;
	    while ( !no_line ) {
		ret = fscanf ( infd[i], "%[^\n]\n", line );
		if ( EOF == ret ) {
		    all_done++;
		    break;
		}
		if ( NULL != strstr ( line, "winner is") &&
		     deconly ){
		    goto exit;
		}
		if ( line[0] == '#' ) {
		    /* Skip comment lines */
		    continue;
		}
		no_line = 1;
	    }	    
	    
	    if ( i == 0 ) {
		/*read the parameters of the run */
		basestr = strstr ( line, "request" );
		sscanf ( basestr, "%7s %d", reqstr, &tline.req );
		
		basestr = strstr ( line, "method" );
		sscanf ( basestr, "%6s %d", reqstr, &tline.method );
		
		basestr = strstr ( line, ")" );
		sscanf ( basestr, "%1s %lf\n", reqstr, &time );
	    }
	    else {
		/*read the parameters of the run */
		basestr = strstr ( line, "request" );
		sscanf ( basestr, "%7s %d", reqstr, &req );
		if ( req != tline.req ) {
		    printf("Request mismatch at process %d \n", i);
		}
		
		basestr = strstr ( line, "method" );
		sscanf ( basestr, "%6s %d", reqstr, &method );
		if ( method != tline.method ) {
		    printf ("Method mismatch at process %d\n", i);
		}		

		basestr = strstr ( line, ")" );
		sscanf ( basestr, "%1s %lf\n", reqstr, &time );
	    }		
	    
	    pos = emethods[i][tline.method].em_rescount;
	    count = emethods[i][tline.method].em_count;
	    if ( pos >= count ) {
		goto exit;
	    }
	    emethods[i][tline.method].em_time[pos] = time;
	    emethods[i][tline.method].em_rescount++;
	    emethods[i][tline.method].em_avg += time/count;
	}
    }

 exit:

    for ( i=0; i< numprocs; i++ ) {
	fclose ( infd[i]);
    }

    free ( infd );

    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void minmax_filter_timings ( struct emethod **em, int outlier_factor )
{
    int i, j, k;
    double min;

    for (i=0; i < numprocs; i++ ) {
	for ( j=0; j< nummethods; j++ ) {

	    em[i][j].em_sum_filtered = 0.0;
	    em[i][j].em_cnt_filtered = 0;
	    em[i][j].em_avg_filtered = 0.0;

	    /* Determine the min  value for method [i][j]*/
	    for ( min=999999, k=0; k<em[i][j].em_rescount; k++ ) {
		if ( em[i][j].em_time[k] < min ) {
		    min = em[i][j].em_time[k];
		}
	    }

	    /* Count how many values are N times larger than the min. */
	    for ( k=0; k<em[i][j].em_rescount; k++ ) {
		if ( em[i][j].em_time[k] >= (outlier_factor * min) ) {
		    em[i][j].em_poison[k] = 1;
		    em[i][j].em_cnt_filtered ++;
#ifdef DEBUG
		    printf("#%d: method %d meas. %d is outlier %lf min %lf\n",
			   i, j, k,  em[i][j].em_time[j], min );
#endif
		}
		else {
		    em[i][j].em_sum_filtered += em[i][j].em_time[k];
		}
	    }
	    if ( em[i][j].em_cnt_filtered < em[i][j].em_rescount ) {
		em[i][j].em_avg_filtered  = em[i][j].em_sum_filtered/
		    (em[i][j].em_rescount - em[i][j].em_cnt_filtered);
		em[i][j].em_perc_filtered = 100 * em[i][j].em_cnt_filtered/em[i][j].em_rescount;
	    }

	}
    }


    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void minmax_calc_per_iteration ( struct emethod **em, char *filename )
{
    int i, j, k;
    FILE *outf;
    struct lininf tline;

    outf = fopen(filename, "w");
    if ( NULL == outf ) {
	printf("calc_perit_filtered: could not open %s for writing\n", filename);
	exit (-1);
    }

    for (j=0; j< nummethods; j++ ) {
	for ( k=0; k<nummeas; k++ ) {
	    TLINE_INIT(tline);
	    for (i=0; i< numprocs; i++ ) {
		if ( !em[i][j].em_poison[k] ) {
		    TLINE_MIN(tline, em[i][j].em_time[k], i);
		    TLINE_MAX(tline, em[i][j].em_time[k], i);
		}
		else {
		    em[i][j].em_num_outliers++;
		}
	    }
	    fprintf (outf, "%3d %8.4lf %3d %8.4lf %3d\n", 
		     j, tline.min, tline.minloc, tline.max, 
		     tline.maxloc );
	}
    }


   for ( i=0; i< numprocs; i++ ) {
        fprintf(outf, "# %d: ", i);
        for ( j=0; j< nummethods; j++ ) {
            fprintf(outf, " %d ", em[i][j].em_num_outliers);
        }
        fprintf(outf, "\n");
    }

    fclose (outf);
    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
static int tcompare ( const void *p, const void* q )
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


void minmax_calc_statistics ( struct emethod **em, char *filename ) 
{
    FILE *outf=NULL;
    struct lininf tline, tline2, tline3;
    int i, j;

    outf = fopen(filename, "a");
    if ( NULL == outf ) {
	printf("calc_statistics: could not open %s for writing\n", filename );
	exit (-1);
    }
    fprintf(outf, "\n");
    
    /* Calculate the median of all measurement series */
    for (i=0; i < numprocs; i++ ) {
	for ( j=0; j< nummethods; j++ ) {
	    qsort ( em[i][j].em_time, em[i][j].em_rescount, sizeof(double), 
		    tcompare );
	    em[i][j].em_median = em[i][j].em_time[ (em[i][j].em_rescount/2)];
	}	
    }
    
    for (j=0; j< nummethods; j++ ) {
	TLINE_INIT(tline);
	TLINE_INIT(tline2);
	TLINE_INIT(tline3);
	for (i=0; i< numprocs; i++ ) {
	    TLINE_MIN(tline, em[i][j].em_median, i);
	    TLINE_MAX(tline, em[i][j].em_median, i);
	    TLINE_MIN(tline2, em[i][j].em_avg, i);
	    TLINE_MAX(tline2, em[i][j].em_avg, i);
	    TLINE_MIN(tline3, em[i][j].em_avg_filtered, i);
	    TLINE_MAX(tline3, em[i][j].em_avg_filtered, i);
	}
	fprintf (outf, "%3d %12.3lf %3d %12.3lf %3d %12.3lf %3d %12.3lf %3d "
		 " %12.3lf %3d %12.3lf %3d \n", 
		 j, tline.min, tline.minloc, tline.max, tline.maxloc,
		 tline2.min, tline2.minloc, tline2.max, tline2.maxloc,
		 tline3.min, tline3.minloc, tline3.max, tline3.maxloc );
    }

    fclose (outf);
    return;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void minmax_calc_decision ( struct emethod **em, int outlier_fraction ) 
{
    struct lininf *tline_perc, *tline_unf, *tline_filt;
    int *meth;
    struct lininf tline_avg, tline_filtered_avg, tline_new;
    int i, j;

    tline_perc  = (struct lininf *) malloc ( sizeof(struct lininf) * nummethods ); 
    tline_unf   = (struct lininf *) malloc ( sizeof(struct lininf) * nummethods ); 
    tline_filt  = (struct lininf *) malloc ( sizeof(struct lininf) * nummethods ); 
    meth  = (int *) malloc ( sizeof(int) * nummethods ); 
    if ( NULL == tline_perc || NULL == tline_unf || NULL == tline_filt || NULL == meth) {
	printf("minmax_calc_decision: could not allocate memory\n");
	exit (-1);
    }

    for (j=0; j< nummethods; j++ ) {
	TLINE_INIT(tline_perc[j]);
	TLINE_INIT(tline_unf[j]);
	TLINE_INIT(tline_filt[j]);
	for (i=0; i< numprocs; i++ ) {
	    TLINE_MAX(tline_unf[j], em[i][j].em_avg, i);
	    TLINE_MAX(tline_filt[j], em[i][j].em_avg_filtered, i);
	    TLINE_MAX(tline_perc[j], em[i][j].em_perc_filtered, i);
	}
    }

    /* The final decision and output */
    TLINE_INIT (tline_avg);
    TLINE_INIT (tline_filtered_avg);
    TLINE_INIT (tline_new);
    for ( j=0; j<nummethods; j++  ) {
	printf("%d: unfiltered: %lf filtered: %lf perc: %lf\n", 
	       j, tline_unf[j].max, tline_filt[j].max, tline_perc[j].max );
	TLINE_MIN (tline_avg, tline_unf[j].max, j);
	TLINE_MIN (tline_filtered_avg, tline_filt[j].max, j);

	if ( tline_perc[j].max > outlier_fraction ) {
	    meth[j] = tline_unf[j].max;
	}
	else {
	    meth[j] = tline_filt[j].max;
	}
	TLINE_MIN(tline_new, meth[j], j);
    }


    printf("New decision winner is %d \n", tline_new.minloc );
    if ( tline_perc[tline_filtered_avg.minloc].max < outlier_fraction ) 
	printf ("Prev. decision winner is %d (filtered)\n",  tline_filtered_avg.minloc );
    else
	printf ("Prev. decision Winner is %d (unfiltered)\n",  tline_avg.minloc );
	

    free ( tline_perc );
    free ( tline_unf);
    free ( tline_filt);
    free ( meth);

    return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void minmax_init (int argc, char ** argv, struct emethod ***emethods) 
{
    struct emethod **em;
    int i, j, tmpargc=4;

    if (argc < 4 )
    {
	printf(" Usage : minmax <numprocs> <nummethods> <nummeas>"
	       " <options>\n\n");
	printf("   minmax takes the output files generated by ADCL and \n");
	printf("   determines for each individual measurement the minimal and \n");
	printf("   maximual value across all processes and the according locations\n");
	printf(" Options: \n");
	printf("   <numprocs>   : number of processes \n");
	printf("   <nummethods> : number of evaluated implementations \n");
	printf("   <nummeas>    : number of measurements per implementation \n");
	printf("   -deconly     : stop after the decision procedure\n");
	printf("   -ofiles      : write output files \n");
	printf("   -ofraction   : outlier fraction to be used \n");
	printf("   -ofactor     : outlier factor to be used \n");

	exit ( 1 ) ;
    }
    
    numprocs    = atoi( argv[1] );
    nummethods  = atoi (argv[2] );
    nummeas     = atoi ( argv[3]);
    
    while ( tmpargc < argc ) {
	if ( strncmp ( argv[tmpargc], "-deconly", strlen("-deconly") )== 0 ) {
	    deconly = 1;
	}
	else if ( strncmp(argv[tmpargc], "-ofiles", strlen("-ofiles") )== 0 ) {
	    deconly=1;
	    output_files = 1;
	}
	else if ( strncmp(argv[tmpargc], "-ofactor", strlen("-ofactor") )== 0 ) {
	    tmpargc++;
	    outlier_factor = atoi ( argv[tmpargc] );
	}
	else if ( strncmp(argv[tmpargc], "-ofraction", strlen("-ofraction") )== 0 ) {
	    tmpargc++;
	    outlier_fraction = atoi ( argv[tmpargc] );
	}

	tmpargc++;
    } 

    if ( NULL != emethods ) {
	/* Allocate the required emethods array to hold the overall data */
	em = ( struct emethod **) malloc ( numprocs * sizeof ( struct emethod *));
	if ( NULL == em ) {
	    exit (-1);
	}
	
	for ( i=0; i< numprocs; i++ ) {
	    em[i] = (struct emethod *) calloc (1, nummethods*sizeof(struct emethod));
	    if  ( NULL == em[i] ) {
		exit (-1);
	    }
	    
	    for (j=0; j< nummethods; j++ ) {
		em[i][j].em_time = (double *) calloc (1, nummeas * sizeof(double));
		if ( NULL == em[i][j].em_time ) {
		    exit (-1);
		}
		em[i][j].em_poison = (int *) calloc (1, nummeas * sizeof(int));
		if ( NULL == em[i][j].em_poison ) {
		    exit (-1);
		}
		em[i][j].em_count    = nummeas;
	    }
	}
	
	*emethods = em;
    }

    return;
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
void minmax_clear_poison_field ( struct emethod **em)
{
    int i, j, k;

    for ( i=0; i<numprocs; i++ ) {
        for (j=0; j< nummethods; j++ ) {
            for ( k=0; k<nummeas; k++ ) {
                em[i][j].em_poison[k] = 0;
            }
        }
    }

    return;
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
void minmax_finalize ( struct emethod ***emethods )
{
    int i, j;
    struct emethod **em = *emethods;

    for ( i=0; i< numprocs; i++ ) {
	for (j=0; j< nummethods; j++ ) {
	    if ( NULL != em[i][j].em_time ) {
		free ( em[i][j].em_time );
	    }
	    
	    if ( NULL != em[i][j].em_poison ) {
		free ( em[i][j].em_poison );
	    }
	}
	if ( NULL != em[i] ) {
	    free ( em[i] );
	}
    }

    if ( NULL != em ) {
	free ( em );
    }

    *emethods = NULL;
    return;
}
