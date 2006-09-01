#include "ADCL_internal.h"


/* An outlier defines as number of times larger 
   than the minimal value */
int ADCL_OUTLIER_FACTOR=3;

/* Percentage of outliers allowed such that we really 
   treat them as outliers */
int ADCL_OUTLIER_FRACTION=20;

/* what measure shall be used ? */
int ADCL_statistic_method=ADCL_STATISTIC_MAX;


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_statistics_filter_timings (ADCL_emethod_t *emethods, int count, 
				    MPI_Comm comm )
{
    int i, j, k;
    int numoutl, rank;
    TIME_TYPE min;

    MPI_Comm_rank ( comm, &rank );
    for (i=0; i < count; i++ ) {
	if ( ! emethods[i].em_filtered ) {
	    /* Determine the min  value for method i*/
	    for ( min=999999, j=0; j<emethods[i].em_rescount; j++ ) {
		if ( emethods[i].em_time[j] < min ) {
		    min = emethods[i].em_time[j];
		}
	    }	    

	    /* Count how many values are N times larger than the min. */
	    for ( numoutl=0, j=0; j<emethods[i].em_rescount; j++ ) {
		if ( emethods[i].em_time[j] >= (ADCL_OUTLIER_FACTOR * min) ) {
		    ADCL_printf("#%d: method %d meas. %d is outlier %lf min %lf\n",
				rank, i, j, emethods[i].em_time[j], min );
		    numoutl++;
		}
	    }
	    ADCL_printf("#%d: method %d num. of outliers %d min %lf\n",
			rank, i, numoutl, min );
	    
	    /* 
	    ** If the percentage of outliers is below a defined maximum,
	    ** we simply remove them from the list and adapt all counters.
	    ** If it is above, we can not assume, that they are outliers.
	    ** They might be generated by the method, e.g. by regularly
	    ** congesting the network.
	    */
	    if ((100*numoutl/emethods[i].em_rescount) < ADCL_OUTLIER_FRACTION ) {
		for ( k=0, j=0; j<emethods[i].em_rescount; j++ ) {
		    if (emethods[i].em_time[j] < (ADCL_OUTLIER_FACTOR * min)){
			emethods[i].em_time[k++] = emethods[i].em_time[j];
		    }
		}
		emethods[i].em_rescount -= numoutl;
		emethods[i].em_count -= numoutl;
	    }
	    emethods[i].em_filtered = 1; 
	}
    }
      
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Determine which method gets how many points */
int ADCL_statistics_determine_votes ( ADCL_emethod_t *emethods, int count, 
				      MPI_Comm comm )
{
    double sum, *sorted=NULL;
    int i, j, rank, pts;
    
    if (ADCL_STATISTIC_MAX == ADCL_statistic_method ) {
	for ( i=0; i < count; i++ ) {
	    for ( sum=0, j=0; j< emethods[i].em_rescount; j++ ) {
		sum += emethods[i].em_time[j];
	    }
	    emethods[i].em_lpts = sum/emethods[i].em_rescount;
	}
    }

    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_statistics_global_max ( ADCL_emethod_t *emethods, int count, 
				 MPI_Comm comm )
{
    int i, winner=-1;
    double *lpts, *gpts;

    lpts = (double *) malloc ( 2* count * sizeof(double));
    if ( NULL == lpts ) {
	return winner;
    }
    gpts = &(lpts[count]);

    for ( i = 0; i < count; i++ ) {
	lpts[i] = emethods[i].em_lpts;
    }

    if  ( ADCL_STATISTIC_MAX == ADCL_statistic_method ) {
	int min;

	MPI_Allreduce ( lpts, gpts, count, MPI_DOUBLE, MPI_MAX, comm);
	for ( winner=0, min=gpts[0], i = 1; i < count; i++ ) {
	    if ( gpts[i] < min ) {
		min = gpts[i];
		winner = i;
	    }
	}
    }

    free ( lpts );
    return winner;
}    

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
