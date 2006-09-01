#ifndef __ADCL_EMETHOD_H__
#define __ADCL_EMETHOD_H__

#include "ADCL_method.h"
#include "ADCL_sysconfig.h"
#include <sys/time.h>
#include "mpi.h"

#define TIME_TYPE double

//#define TIME      MPI_Wtime()
#define TIME ADCL_emethod_time()
#define ADCL_EMETHOD_TIMEDIFF(_tstart,_tend,_exec) {         \
    if ( _tend > _tstart ) _exec = (double) (_tend-_tstart); \
    else _exec = (1000000.0 - _tstart) + _tend; }

#define ADCL_FLAG_PERF     -100
#define ADCL_FLAG_NOPERF   -101

/* Structure used for evaluating a communication method. 
** It contains the method itself, and statistical data
*/

struct ADCL_emethod_s {
    int                em_id; /* unique identifier */
    ADCL_method_t* em_method; /* which method am I associated with ? */
    int             em_count; /* how often has this routine already been called */
    int          em_rescount; /* how often has this routine already reported back */
    TIME_TYPE       *em_time; /* measurements */
    int          em_filtered; /* Has this data set already been filtered? */
    double           em_lpts; /* local no. of pts by this method */
    int            em_tested; /* flags that whether this method has been already tested*/
};

typedef struct ADCL_emethod_s ADCL_emethod_t;

struct ADCL_emethod_req_s {
  
    MPI_Comm                           er_comm;
    int                                 er_pos;
    int                               er_rfcnt;
    int                               er_state; /* state of the object */
    int                                er_last; /* last element given out */
    int                        er_max_emethods; /* length of the er_emethod array */
    int                        er_num_emethods; /* how many methods shall be evaluated */
    int                             er_vhwidth;
    int                                 er_vnc;
    int                              er_vndims;
    int                          er_nneighbors;
    int                          *er_neighbors;
    int                              *er_vdims;
    ADCL_emethod_t                *er_emethods;
    ADCL_method_t                  *er_wmethod; /* winner method used after the testing */

    char                             *er_cache;
    int er_attr_hypothesis[ADCL_ATTR_TOTAL_NUM]; /* List of performance hypothesis*/
    int er_attr_confidence[ADCL_ATTR_TOTAL_NUM]; /* List of confidence*/
    int          er_num_available_measurements; /* Counter keeping track of how many methods have already been measured*/

};

typedef struct ADCL_emethod_req_s ADCL_emethod_req_t;

int ADCL_emethod_req_init     ( void );
int ADCL_emethod_req_finalize ( void );

ADCL_emethod_req_t* ADCL_emethod_init ( MPI_Comm comm, int nneighbors, 
                                        int *neighbors, int vndims, 
					int *vdims, int vnc, int vhwidth);

void ADCL_emethod_free ( ADCL_emethod_req_t * er );

double ADCL_emethod_time(void);

ADCL_method_t *ADCL_emethod_get_method ( ADCL_emethod_req_t *emethod, int pos);

int ADCL_emethod_monitor ( ADCL_emethod_req_t *emethod, int pos,
			   TIME_TYPE tstart, TIME_TYPE tend );

int  ADCL_emethods_get_winner ( ADCL_emethod_req_t *ermethods, MPI_Comm comm );
int  ADCL_emethods_get_next ( ADCL_emethod_req_t *ermethods, int mode, int *flag);
void ADCL_emethods_update ( ADCL_emethod_req_t *ermethods, int pos, 
			    int flag, TIME_TYPE tstart, TIME_TYPE tend );

int ADCL_emethod_get_next_id (void);



#endif /* __ADCL_EMETHOD_H__ */
