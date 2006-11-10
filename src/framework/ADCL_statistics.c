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
int ADCL_statistics_filter_timings (ADCL_emethod_t **emethods, int count, 
				    int rank )
{
    int i, j, k;
    int numoutl;
    TIME_TYPE min;

    for (i=0; i < count; i++ ) {
      if ( !(ADCL_EM_IS_FILTERED(emethods[i]))) {
	/* Determine the min  value for method i*/
	for ( min=999999, j=0; j<emethods[i]->em_rescount; j++ ) {
	  if ( emethods[i]->em_time[j] < min ) {
	    min = emethods[i]->em_time[j];
	  }
	}	    
	
	/* Count how many values are N times larger than the min. */
	for ( numoutl=0, j=0; j<emethods[i]->em_rescount; j++ ) {
	  if ( emethods[i]->em_time[j] >= (ADCL_OUTLIER_FACTOR * min) ) {
#if 0 
	    ADCL_printf("#%d: method %d meas. %d is outlier %lf min %lf\n",
			rank, emethods[i]->em_method->m_id, j, 
			emethods[i]->em_time[j], min );
#endif
	    numoutl++;
	  }
	}
#if 0
	ADCL_printf("#%d: method %d num. of outliers %d min %lf\n",
		    rank, emethods[i]->em_method->m_id, numoutl, min );
#endif

	/* 
	** If the percentage of outliers is below a defined maximum,
	** we simply remove them from the list and adapt all counters.
	** If it is above, we can not assume, that they are outliers.
	** They might be generated by the method, e.g. by regularly
	** congesting the network.
	*/
	if ((100*numoutl/emethods[i]->em_rescount) < ADCL_OUTLIER_FRACTION ) {
	  for ( k=0, j=0; j<emethods[i]->em_rescount; j++ ) {
	    if (emethods[i]->em_time[j] < (ADCL_OUTLIER_FACTOR * min)){
	      emethods[i]->em_time[k++] = emethods[i]->em_time[j];
	    }
	  }
	  emethods[i]->em_rescount -= numoutl;
	  emethods[i]->em_count -= numoutl;
	}
	ADCL_EM_SET_FILTERED (emethods[i]); 
      }
    }
    
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* Determine which method gets how many points */
int ADCL_statistics_determine_votes ( ADCL_emethod_t **emethods, int count, 
				      int rank )
{
    double sum;
    int i, j;
    
    if (ADCL_STATISTIC_MAX == ADCL_statistic_method ) {
      for ( i=0; i < count; i++ ) {
	if (!(ADCL_EM_IS_EVAL(emethods[i]))  ) {
	  for ( sum=0, j=0; j< emethods[i]->em_rescount; j++ ) {
	    sum += emethods[i]->em_time[j];
	  }
	  emethods[i]->em_lpts = sum/emethods[i]->em_rescount;
	  ADCL_EM_SET_EVAL (emethods[i]);
#if 0 
	  ADCL_printf("#%d: method %d lpts %lf\n",
		      rank, emethods[i]->em_method->m_id, 
		      emethods[i]->em_lpts);
#endif
	}
      }
    }
    
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_statistics_global_max ( ADCL_emethod_t **emethods, int count,
 				 MPI_Comm comm, int num_blocks, int *blength, 
				 int *winners, int rank )
{
    int i, j, c;
    double *lpts, *gpts;

    lpts = (double *) malloc ( 2* count * sizeof(double));
    if ( NULL == lpts ) {
	return ADCL_NO_MEMORY;
    }
    gpts = &(lpts[count]);

    for ( i = 0; i < count; i++ ) {
	lpts[i] = emethods[i]->em_lpts;
    }

    if  ( ADCL_STATISTIC_MAX == ADCL_statistic_method ) {
      int min;
      
      MPI_Allreduce ( lpts, gpts, count, MPI_DOUBLE, MPI_MAX, comm);

#if 0      
      ADCL_printf("#%d: number of blocks %d \n", rank, num_blocks );
#endif
      for ( c=0, j = 0; j < num_blocks; j++) {
	for ( winners[j]=0, min=gpts[c], i = c; i < (c+blength[j]); i++ ) {
	  if ( gpts[i] < min ) {
	    min = gpts[i];
	    winners[j] = i;
	  }
	  ADCL_printf("#%d: block %d entry %d [%lf] current min: %d %d \n", 
		      rank, j, i, gpts[i], min, winners[j]);
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
