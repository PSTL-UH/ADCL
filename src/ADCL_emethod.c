#include "ADCL_internal.h"


static int ADCL_local_id_counter=0;
static int adcl_compare ( const void*, const void* );

static ADCL_array_t *ADCL_emethod_req_array=NULL;

int ADCL_emethod_req_init ( void ) 
{
    ADCL_array_init ( &(ADCL_emethod_req_array), "ADCL_emethod_req", 32 );    
    return ADCL_SUCCESS;
}

int ADCL_emethod_req_finalize ( void ) 
{
    ADCL_array_free ( &(ADCL_emethod_req_array) );    
    return ADCL_SUCCESS;
}

ADCL_emethod_req_t * ADCL_emethod_init ( MPI_Comm comm, int nneighbors, 
					 int *neighbors, int vndims, 
					 int *vdims, int vnc, int vhwidth )
{
    int i, j, last, found=-1;
    ADCL_emethod_req_t *er;    
    int result;

    /* Check first, whether we have an entry in the ADCL_emethods_req_array,
       which fulfills already our requirements; 
    */
    last = ADCL_array_get_last ( ADCL_emethod_req_array );
    for ( i=0; i<= last; i++ ) {
	er = ( ADCL_emethod_req_t * ) ADCL_array_get_ptr_by_pos ( 
	    ADCL_emethod_req_array, i );

	MPI_Comm_compare ( er->er_comm, comm, &result );
	if ( ( result != MPI_IDENT) && (result != MPI_CONGRUENT) ) {
	    continue;
	}

	found = i;
	if ( ( er->er_nneighbors == nneighbors ) && 
	     ( er->er_vndims     == vndims     ) && 
	     ( er->er_vnc        == vnc        ) && 
	     ( er->er_vhwidth    == vhwidth    ) ) {

	    for ( j=0; j< er->er_nneighbors; j++ ) {
		if ( er->er_neighbors[i] != neighbors [i] ) {
		    found = -1;
		    break;
		}
	    }
	    if ( found == -1 ) {
		continue;
	    }
	    for ( j=0 ; j< er->er_vndims; j++ ){
		if ( er->er_vdims[i] != vdims[i] ) {
		    found = -1;
		    break;
		}
	    }
	    if ( found != -1 ) {
		break;
	    }
	}
    }

    if ( found > -1 ) {
	er->er_rfcnt++;
	return er;
    }
	
    /* we did not find this configuraion yet, so we have to add it */
    er = ( ADCL_emethod_req_t *) calloc (1, sizeof(ADCL_emethod_req_t));
    if ( NULL == er ) {
	return NULL;
    }
    
    er->er_comm       = comm;
    er->er_nneighbors = nneighbors;
    er->er_vndims     = vndims;
    er->er_rfcnt      = 1;
    er->er_state      = ADCL_STATE_TESTING;
    er->er_neighbors  = (int *) malloc ( nneighbors * sizeof(int));
    er->er_vdims      = (int *) malloc ( vndims * sizeof(int));
    if ( NULL == er->er_neighbors || NULL == er->er_vdims ) {
	free ( er );
	return NULL;
    }
    memcpy ( er->er_neighbors, neighbors, nneighbors * sizeof(int));
    memcpy ( er->er_vdims, vdims, vndims * sizeof(int));
    er->er_vnc     = vnc;
    er->er_vhwidth = vhwidth;

    er->er_num_emethods = ADCL_get_num_methods ();
    er->er_emethods     = (ADCL_emethod_t*) calloc ( 1, er->er_num_emethods *
						     sizeof(ADCL_emethod_t));
    if ( NULL == er->er_emethods ) {
	free ( er->er_vdims ) ;
	free ( er->er_neighbors );
	free ( er );
	return NULL;
    }

    for ( i=0; i< er->er_num_emethods; i++ ) {
	er->er_emethods[i].em_id     = ADCL_emethod_get_next_id ();
	er->er_emethods[i].em_method = ADCL_get_method(i);
	er->er_emethods[i].em_min    = 999999;
    }

    ADCL_array_get_next_free_pos ( ADCL_emethod_req_array, &er->er_pos );
    ADCL_array_set_element ( ADCL_emethod_req_array, 
			     er->er_pos, 
			     er->er_pos,
			     er );    
    return er;
}

void ADCL_emethod_free ( ADCL_emethod_req_t * er )
{
    er->er_rfcnt--;
    if ( er->er_rfcnt == 0 ) {
	if ( NULL != er->er_vdims  ) {
	    free ( er->er_vdims);
	}
	if ( NULL != er->er_neighbors ) {
	    free ( er->er_neighbors );
	}
	if ( NULL != er->er_emethods ) {
	    free ( er->er_emethods );
	}
	ADCL_array_remove_element ( ADCL_emethod_req_array, er->er_pos );
	free ( er );
    }

    return;
}

double ADCL_emethod_time (void) 
{ 
    struct timeval tp; 
    gettimeofday (&tp, NULL); 
    return tp.tv_usec;
}

int ADCL_emethod_get_next_id (void)
{
    return ADCL_local_id_counter++;
}

ADCL_method_t* ADCL_emethod_get_method ( ADCL_emethod_req_t *erm, int pos)
{
    return erm->er_emethods[pos].em_method;
}

    
int ADCL_emethod_monitor ( ADCL_emethod_req_t *ermethod, int pos, 
			   TIME_TYPE tstart, TIME_TYPE tend )
{
    /* to be done later */
    return ADCL_STATE_REGULAR;
}

int ADCL_emethods_get_winner ( ADCL_emethod_req_t *ermethod, 
			       MPI_Comm comm )
{
    int i, j, winner=-1;
    double *sorted=NULL;
    int pts, max;
    ADCL_emethod_t *emethods=ermethod->er_emethods;
    int count = ermethod->er_num_emethods;

    sorted = (double *) malloc ( 2*count*sizeof(double));
    if ( NULL == sorted ) {
	return ADCL_NO_MEMORY;
    }

    for ( i=0; i < count; i++ ) {
	sorted[2*i]     = emethods[i].em_avg;
	sorted[(2*i)+1] = emethods[i].em_id;
    }
    
    qsort ( sorted, count, 2*sizeof(double), adcl_compare );

    /* Give now the fastest method count-1 pts, the 2nd fastest
       method count-2 pts, ..., the slowest method 0 pts. */
    for ( pts=count-1, i=0; i< count; i++, pts-- ) {
	for (j=0; j< count; j++ ) {
	    if ( sorted[(2*i)+1] == emethods[j].em_id ) {
		emethods[j].em_lpts = pts;
		ADCL_printf("Assigning %d pts for method %d avg=%lf\n", 
			    pts, emethods[j].em_id, emethods[j].em_avg );
		continue;
	    }
	}
    }

    /* Determine now how many point each method achieved globally. The
       method with the largest number of points will be the chosen one.
    */
    max   = 0;
    for ( i=0; i<count; i++ ) {
	MPI_Allreduce ( &emethods[i].em_lpts, &emethods[i].em_gsum, 1, MPI_INT,
			MPI_SUM, comm );
	if ( emethods[i].em_gsum > max ) {
	    max = emethods[i].em_gsum;
	    winner = i;
	}
    }

    return winner;
}
int ADCL_emethods_get_next ( ADCL_emethod_req_t *er, int mode, int *flag )
{
    int i, next=ADCL_EVAL_DONE;
    int rflag = ADCL_FLAG_NOPERF;
    ADCL_emethod_t *emethod;

    for ( i=er->er_last; i< er->er_num_emethods; i++ ) {
	emethod = &(er->er_emethods[i]);
	if ( emethod->em_count < ADCL_EMETHOD_NUMTESTS ) {
	    next = i;
	    rflag = ADCL_FLAG_PERF;
	    emethod->em_count++;
	    break;
	}
    }
    
    if ( next != ADCL_EVAL_DONE) {
	er->er_last = next;
    }
    else { 
	/* 
	** At this point, all methods have been *initiated* the required number of 
	** times. However, we need to check, whether we are really done with 
	** evaluation, or whether we still have some performance data outstanding
	*/
	for ( i=er->er_last; i< er->er_num_emethods; i++ ) {
	    emethod = &(er->er_emethods[i]);
	    /*
	    ** remember that dual block methods need 2 *EMETHOD_NUMTESTS
	    ** reported back! 
	    */
	    if ( (emethod->em_method->m_db                           && 
		  (emethod->em_rescount < 2* ADCL_EMETHOD_NUMTESTS)) ||
		 (!emethod->em_method->m_db                          && 
		  (emethod->em_rescount < ADCL_EMETHOD_NUMTESTS )    )) {
		/* 
		** ok, some data is still outstanding. So we 
		** do not switch yet to the evaluation mode, 
		** which give the last method out once again with 
		** the noperf flag (= performance data not relevant
		** for evaluation 
		*/
		next = er->er_last;
		break;
	    }
	}
    }

    *flag = rflag;
    return next;
}

void ADCL_emethods_update ( ADCL_emethod_req_t *ermethods, int pos, int flag, 
			    TIME_TYPE tstart, TIME_TYPE tend )
{
    ADCL_emethod_t *emethods = ermethods->er_emethods;
    double exectime = (double) (tend-tstart);
    ADCL_emethod_t *tmpem;


    if ( flag == ADCL_FLAG_PERF ) {
	tmpem = &emethods[pos];
	tmpem->em_sum += exectime;
	tmpem->em_avg = tmpem->em_sum / tmpem->em_count;
	if ( exectime > tmpem->em_max ) {
	    tmpem->em_max = exectime;
	}
	if ( exectime < tmpem->em_min ) {
	    tmpem->em_min = exectime;
	}
	
	tmpem->em_rescount++;
    }
    return;
}


static int adcl_compare ( const void *p, const void *q )
{
    double *a, *b;
    
    a = (double *) p;
    b = (double *) q;

    /* simple tests are those where the avg execution times 
       are different */
    if ( a[0] < b[0] ) {
        return (-1);
    }
    if ( a[0] > b[0] ) {
        return (1);
    }

    /* ok, if the avg execution times are the same then we 
       chose the one with the smaller id number. Since these
       are however double values, this should hopefully not
       really happen */
    if ( a[0] == b[0] ) {
        if ( a[1] < b[1] ) {
            return (-1);
        }
        if ( a[1] > b[1] ) {
            return (1);
        }
    }
    return ( 0 );
}
