#include "ADCL_internal.h"


static int ADCL_local_id_counter=0;
static int adcl_compare ( const void*, const void* );

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

ADCL_method_t* ADCL_emethod_get_method ( ADCL_emethod_t *emethod)
{
    return emethod->em_method;
}

    
int ADCL_emethod_monitor ( ADCL_emethod_t *emethod, TIME_TYPE tstart, 
			   TIME_TYPE tend )
{
    /* to be done later */
    return ADCL_STATE_REGULAR;
}

int ADCL_emethods_get_winner ( int count, ADCL_emethod_t *emethods, 
			       MPI_Comm comm )
{
    int i, j, winner=-1;
    double *sorted=NULL;
    int pts, max;

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
int ADCL_emethods_get_next ( int count, ADCL_emethod_t *emethods, int last )
{
    int i, next=ADCL_EVAL_DONE;

    if ( last >= count ||  last < 0 ) {
	last = 0;
    }

    for ( i=last; i< count; i++ ) {
	if ( emethods[i].em_count < ADCL_EMETHOD_NUMTESTS ) {
	    next = i;
	    emethods[i].em_count++;
	    break;
	}
    }
    return next;
}

void ADCL_emethods_update ( int count, ADCL_emethod_t *emethods, int last, 
			    TIME_TYPE tstart, TIME_TYPE tend )
{
    ADCL_emethod_t *tmpem= &emethods[last];
    double exectime = (double) (tend-tstart);

    tmpem->em_sum += exectime;
    tmpem->em_avg = tmpem->em_sum / tmpem->em_count;
    if ( exectime > tmpem->em_max ) {
	tmpem->em_max = exectime;
    }
    if ( exectime < tmpem->em_min ) {
	tmpem->em_min = exectime;
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
