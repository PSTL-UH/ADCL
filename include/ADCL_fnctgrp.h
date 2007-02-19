#ifndef __ADCL_FNCTGRP_H__
#define __ADCL_FNCTGRP_H__

#include "mpi.h"

#include "ADCL.h"
#include "ADCL_attribute.h"


struct ADCL_fnctgrp_s{
    int                     f_id; /* id of the object */
    int                 f_findex; /* index of this object in the fortran array */
    int                 f_maxnum; /* no. of functions in this function-group */
    ADCL_work_fnct_ptr **f_fptrs; /* list of function pointers of this group. */
    ADCL_attrset_t    *f_attrset; /* attribute set used to characterize the function */
    int             **f_attrvals; /* array of attribute values for this particular function */
};
typedef struct ADCL_fnctgrp_s ADCL_fnctgrp_t;
extern ADCL_array_t *ADCL_fnctgrp_farray;

int ADCL_fnctgrp_create ( int maxnum, ADCL_fnctgrp_t **fnctgrp );
int ADCL_fnctgrp_free   ( ADCL_fnctgrp_t **fnctgrp );
int ADCL_fnctgrp_register_fnct ( ADCL_fnctgrp_t * fnctgrp, int pos, ADCL_work_fnct_ptr *fnct);
int ADCL_fnctgrp_register_fnct_and_attrset ( ADCL_fnctgrp_t * fnctgrp, int pos, ADCL_work_fnct_ptr *fnct, 
					     ADCL_attrset_t * attrset, int *array_of_attrvalues );

#endif
