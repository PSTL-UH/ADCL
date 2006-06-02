#ifndef __ADCL_EMETHOD_H__
#define __ADCL_EMETHOD_H__

#include "ADCL_method.h"
#include <sys/time.h>
#define TIME_TYPE double

//#define TIME      MPI_Wtime()
#define TIME ADCL_emethod_time()

/* How many iterations are we testing each single method? Should
   probably be a parameter later on */
#define ADCL_EMETHOD_NUMTESTS 10


/* Structure used for evaluating a communication method. 
   It contains of the method itself and some statistical data */
struct ADCL_emethod_s {
    int                em_id; /* unique identifier */
    ADCL_method_t *em_method; /* which method am I associated with ? */
    int             em_count; /* how often has this routine already been called */
    double            em_sum; /* sum of all timings taken (local) */
    double            em_avg; /* average time required to execute this method (local) */
    double            em_max; /* max. time required to execute this function (local) */
    double            em_min; /* min. time required to execute this function (local) */
    int              em_lpts; /* local no. of pts by this method */
    int              em_gsum; /* global sum of no. of pts gather by this method */
};
typedef struct ADCL_emethod_s ADCL_emethod_t;

double ADCL_emethod_time(void);
ADCL_method_t * ADCL_emethod_get_method ( ADCL_emethod_t *emethod);
int ADCL_emethod_monitor ( ADCL_emethod_t *emethod, TIME_TYPE tstart, 
			   TIME_TYPE tend );

int ADCL_emethods_get_winner ( int count, ADCL_emethod_t *emethods, MPI_Comm comm );
int ADCL_emethods_get_next ( int count, ADCL_emethod_t *emethods, int lastmethod, 
			     int mode);
void ADCL_emethods_update ( int count, ADCL_emethod_t *emethods, int lastmethod, 
			    TIME_TYPE tstart, TIME_TYPE tend );

int ADCL_emethod_get_next_id (void);

#endif /* __ADCL_EMETHOD_H__ */
