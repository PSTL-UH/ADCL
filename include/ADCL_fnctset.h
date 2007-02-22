#ifndef __ADCL_FNCTSET_H__
#define __ADCL_FNCTSET_H__

#include "mpi.h"

#include "ADCL.h"
#include "ADCL_attribute.h"


struct ADCL_fnctset_s{
    int                     f_id; /* id of the object */
    int                 f_findex; /* index of this object in the fortran array */
    int                 f_maxnum; /* no. of functions in this function-group */
    ADCL_work_fnct_ptr **f_fptrs; /* list of function pointers of this group. */
    ADCL_attrset_t    *f_attrset; /* attribute set used to characterize the function */    
    int             **f_attrvals; /* array of attribute values for this particular 
				     function */
    char           f_fnctsetname[ADCL_MAX_NAMELEN]; /* Name of the attribute set */
    char           **f_fnctnames; /* name of each function */
};
typedef struct ADCL_fnctset_s ADCL_fnctset_t;
extern ADCL_array_t *ADCL_fnctset_farray;

int ADCL_fnctset_create ( int maxnum, char *name, ADCL_fnctset_t **fnctset );
int ADCL_fnctset_free   ( ADCL_fnctset_t **fnctset );
int ADCL_fnctset_register_fnct ( ADCL_fnctset_t * fnctset, int pos,
				 ADCL_work_fnct_ptr *fnct, char *name);
int ADCL_fnctset_register_fnct_and_attrset ( ADCL_fnctset_t * fnctset, int pos, 
					     ADCL_work_fnct_ptr *fnct, 
					     ADCL_attrset_t * attrset, 
					     int *array_of_attrvalues, char *name );

#endif
