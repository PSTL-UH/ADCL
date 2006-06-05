#ifndef __ADCL_EMETHOD_H__
#define __ADCL_EMETHOD_H__

#include "ADCL_method.h"
#include <sys/time.h>
#include "mpi.h"
#define TIME_TYPE double

//#define TIME      MPI_Wtime()
#define TIME ADCL_emethod_time()

/* How many iterations are we testing each single method? Should
   probably be a parameter later on */
#define ADCL_EMETHOD_NUMTESTS 10

#define ADCL_FLAG_PERF     -100
#define ADCL_FLAG_NOPERF   -101

/* Structure used for evaluating a communication method. 
** It contains the method itself, and statistical data
*/
struct ADCL_emethod_s {
    int                em_id; /* unique identifier */
    ADCL_method_t *em_method; /* which method am I associated with ? */
    int             em_count; /* how often has this routine already been called */
    int          em_rescount; /* how often has this routine already reported back */
    double            em_sum; /* sum of all timings taken (local) */
    double            em_avg; /* average time required to execute this method(local)*/
    double            em_max; /* max. time required to execute this function (local)*/
    double            em_min; /* min. time required to execute this function (local)*/
    int            em_lpts; /* local no. of pts by this method */
    int            em_gsum; /* global sum of no. of pts gather by this method */
};
typedef struct ADCL_emethod_s ADCL_emethod_t;


struct ADCL_emethod_req_s {
    MPI_Comm            er_comm;
    int                er_rfcnt;
    int                er_state; /* state of the object */
    int                 er_last; /* last element given out */
    int         er_num_emethods; /* how many methods shall be evaluated */
    int              er_vhwidth;
    int                  er_vnc;
    int               er_vndims;
    int           er_nneighbors;
    int           *er_neighbors;
    int               *er_vdims;
    ADCL_emethod_t *er_emethods;
};
typedef struct ADCL_emethod_req_s ADCL_emethod_req_t;

int ADCL_emethod_req_init     ( void );
int ADCL_emethod_req_finalize ( void );

ADCL_emethod_req_t * ADCL_emethod_init ( MPI_Comm comm, int nneighbors, 
					 int *neighbors, int vndims, 
					 int *vdims, int vnc, int vhwidth);

double ADCL_emethod_time(void);
ADCL_method_t * ADCL_emethod_get_method ( ADCL_emethod_req_t *emethod, int pos);
int ADCL_emethod_monitor ( ADCL_emethod_req_t *emethod, int pos, 
			   TIME_TYPE tstart, TIME_TYPE tend );

int ADCL_emethods_get_winner ( ADCL_emethod_req_t *ermethods, MPI_Comm comm );
int ADCL_emethods_get_next ( ADCL_emethod_req_t *ermethods, int mode, int *flag);
void ADCL_emethods_update ( ADCL_emethod_req_t *ermethods, int pos, 
			    int flag, TIME_TYPE tstart, TIME_TYPE tend );

int ADCL_emethod_get_next_id (void);

#endif /* __ADCL_EMETHOD_H__ */
