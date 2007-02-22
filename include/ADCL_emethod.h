#ifndef __ADCL_EMETHOD_H__
#define __ADCL_EMETHOD_H__

#include "ADCL_sysconfig.h"


struct ADCL_emethod_s {
    int                   em_id; /* unique identifier */
    int               em_findex; /* index of this object in the fortran array */
    int                em_state; /* state of the object */
    int                 em_last; /* last element given out */

    ADCL_topology_t    *em_topo; /* pointer to topology object */
    ADCL_vector_t       *em_vec; /* pointer to vector object. Only the size of the 
				    data array is really required, not the buffer 
				    pointers. */
    ADCL_statistics_t  *em_stat; /* array of statics objects containing the 
				    measurements etc. Length of the array is equal 
				    to the no. of fncts registered in the fnctset */
    ADCL_fnctset_t   em_fnctset; /* copy of the function group associated with this object */
    ADCL_hypothesis_t   em_hypo; /* Performance hypothesis object of to this emethod */
};
typedef struct ADCL_emethod_s ADCL_emethod_t;


ADCL_emethod_t* ADCL_emethod_init ( MPI_Comm comm, int nneighbors, 
				    int *neighbors, int vndims, 
				    int *vdims, int vnc, int vhwidth);

void ADCL_emethod_free ( ADCL_emethod_t * er );

double ADCL_emethod_time(void);

ADCL_fnctset_t *ADCL_emethod_get_fnctset ( ADCL_emethod_t *emethod, int pos);

int ADCL_emethod_monitor ( ADCL_emethod_t *emethod, int pos,
			   TIME_TYPE tstart, TIME_TYPE tend );

int  ADCL_emethods_get_winner ( ADCL_emethod_t *ermethods, MPI_Comm comm );
int  ADCL_emethods_get_next ( ADCL_emethod_t *ermethods, int mode, int *flag);
void ADCL_emethods_update ( ADCL_emethod_t *ermethods, int pos, 
			    int flag, TIME_TYPE tstart, TIME_TYPE tend );

int ADCL_emethod_get_next_id (void);

ADCL_fnctset_t* ADCL_emethod_get_by_attrs ( ADCL_emethod_t *erm, int *attr);


#endif /* __ADCL_EMETHOD_H__ */
