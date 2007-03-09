#include "ADCL_internal.h"


/* An outlier defines as number of times larger 
   than the minimal value */
int ADCL_OUTLIER_FACTOR=3;

/* Percentage of outliers allowed such that we really 
   treat them as outliers */
int ADCL_OUTLIER_FRACTION=50;

/* what measure shall be used ? */
int ADCL_statistic_method=ADCL_STATISTIC_MAX;

struct lininf {
    double min;
    double max;
    int    minloc;
    int    maxloc;
};

#define TLINE_INIT(_t) { _t.min=9999999999.99; _t.max=0.0; \
       _t.minloc=-1; _t.maxloc=-1;}
#define TLINE_MIN(_t, _time, _i){ \
           if ( _time < _t.min ) { \
               _t.min    = _time;  \
               _t.minloc = _i;}}
#define TLINE_MAX(_t, _time, _i) { \
            if ( _time > _t.max ) { \
                _t.max = _time;     \
                _t.maxloc =_i;}}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_statistics_filter_timings (ADCL_statistics_t **statistics, int count, 
				    int rank )
{
    int i, j;
    int numoutl;
    TIME_TYPE min;
    double sum, sum_filtered;

    for (i=0; i < count; i++ ) {
	sum          = 0.0;
	sum_filtered = 0.0;
	if ( !(ADCL_STAT_IS_FILTERED(statistics[i]))) {
	    /* Determine the min  value for method i*/
	    for ( min=999999, j=0; j<statistics[i]->s_rescount; j++ ) {
		if ( statistics[i]->s_time[j] < min ) {
		    min = statistics[i]->s_time[j];
		}
	    }	    
	
	    /* Count how many values are N times larger than the min. */
	    for ( numoutl=0, j=0; j<statistics[i]->s_rescount; j++ ) {
		sum += statistics[i]->s_time[j];
		if ( statistics[i]->s_time[j] >= (ADCL_OUTLIER_FACTOR * min) ) {
#if 0 
		    ADCL_printf("#%d: stat %d meas. %d is outlier %lf min "
				"%lf\n", rank, i, j, statistics[i]->s_time[j], min );
#endif
		    numoutl++;
		}
		else {
		    sum_filtered += statistics[i]->s_time[j];
		}
	    }

	    /* unfiltered avg. */
	    statistics[i]->s_lpts[0] = sum / statistics[i]->s_rescount; 

	    /* filtered avg. */
	    statistics[i]->s_lpts[1] = sum_filtered/(statistics[i]->s_rescount- 
						     numoutl );       
	    /* percentage of outliers */
	    statistics[i]->s_lpts[2] = 100*numoutl/statistics[i]->s_rescount; 

#if 0
	    ADCL_printf("#%d: stat %d num. of outliers %d min %lf avg. %lf "
			"filtered avg. %lf perc. %lf \n", rank, 
			i, numoutl, min, statistics[i]->s_lpts[0], 
			statistics[i]->s_lpts[1], statistics[i]->s_lpts[2]);
#endif

	    ADCL_STAT_SET_FILTERED (statistics[i]); 
	}
    }

    
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_statistics_global_max ( ADCL_statistics_t **statistics, int count,
 				 MPI_Comm comm, int num_blocks, int *blength, 
				 int *winners, int rank )
{
    int i, j, c;
    double *lpts, *gpts;
    struct lininf tline_filtered, tline_unfiltered;

    lpts = (double *) malloc ( 2 * 3 * count * sizeof(double) );
    if ( NULL == lpts ) {
	return ADCL_NO_MEMORY;
    }
    gpts = &(lpts[3 * count]);    

    for ( i = 0; i < count; i++ ) {
	lpts[3*i]   = statistics[i]->s_lpts[0];
	lpts[3*i+1] = statistics[i]->s_lpts[1];
	lpts[3*i+2] = statistics[i]->s_lpts[2];
    }

    if  ( ADCL_STATISTIC_MAX == ADCL_statistic_method ) {
	MPI_Allreduce ( lpts, gpts, 3 * count, MPI_DOUBLE, MPI_MAX, comm);

#if 0      
	ADCL_printf("#%d: number of blocks %d \n", rank, num_blocks );
#endif

	for ( c=0, j = 0; j < num_blocks; j++) {
	    TLINE_INIT ( tline_unfiltered );
	    TLINE_INIT ( tline_filtered );
	    for ( i = c; i < (c+blength[j]); i++ ) {
#if 0
		if ( rank == 0 ) {
		    ADCL_printf("#%d: %lf %lf %lf\n", i, gpts[3*i], 
				gpts[3*i+1], gpts[3*i+2]);
		}
#endif
		TLINE_MIN ( tline_unfiltered, gpts[3*i],  i );
		TLINE_MIN ( tline_filtered, gpts[3*i+1],  i );
	    }
	    if ( gpts[ 3 * tline_filtered.minloc + 2]<ADCL_OUTLIER_FRACTION){
		winners[j] = tline_filtered.minloc;
		ADCL_printf("#%d: block %d winner is %d (filtered) \n", 
			    rank, j, winners[j]);
	    }
	    else {
		winners[j] = tline_unfiltered.minloc;
		ADCL_printf("#%d: block %d winner is %d (unfiltered) \n", 
			    rank, j, winners[j]);
	    }

	    c+=blength[j];
	}
    }

    free ( lpts );
    return ADCL_SUCCESS;
}    

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
double ADCL_statistics_time (void) 
{ 
    struct timeval tp; 
    gettimeofday (&tp, NULL); 
    return tp.tv_usec;
}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
