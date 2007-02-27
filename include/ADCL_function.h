#ifndef __ADCL_FUNCTION_H__
#define __ADCL_FUNCTION_H__

#include "mpi.h"

#include "ADCL.h"
#include "ADCL_attribute.h"


struct ADCL_function_s {
    int                     f_id; /* id of the object */
    int                  f_index; /* index of this object in the fortran array */
    ADCL_work_fnct_ptr   *f_iptr; /* init function pointer. This pointer will 
				     also be used for single block functions */
    ADCL_work_fnct_ptr   *f_wptr; /* wait function pointer. Has to  be NULL 
				     for single block functions */
    ADCL_attrset_t    *f_attrset; /* attribute set characterizing this function */
    int              *f_attrvals; /* array of attribute values for this particular 
				     function */
    char                 *f_name; /* name of the function */
};
typedef struct ADCL_function_s ADCL_function_t;
extern ADCL_array_t *ADCL_function_farray;

int ADCL_function_create_async ( ADCL_work_fnct_ptr *init_fnct, 
				 ADCL_work_fnct_ptr *wait_fnct, 
				 ADCL_attrset_t * attrset, 
				 int *array_of_attrvalues, char *name, 
				 ADCL_function_t **fnct);
int ADCL_function_free ( ADCL_function_t **fnct );



struct ADCL_fnctset_s{
    int                    fs_id; /* id of the object */
    int                fs_findex; /* index of this object in the fortran array */
    int                fs_maxnum; /* no. of functions in this function-group */
    ADCL_function_t   **fs_fptrs; /* list of function pointers of this group. */
    ADCL_attrset_t   *fs_attrset; /* attribute set used to characterize the function */    
    char                *fs_name; /* Name of the attribute set */
};
typedef struct ADCL_fnctset_s ADCL_fnctset_t;
extern ADCL_array_t *ADCL_fnctset_farray;


int ADCL_fnctset_create ( int maxnum, ADCL_function_t **fncts, char *name, 
			  ADCL_fnctset_t **fnctset );
int ADCL_fnctset_free   ( ADCL_fnctset_t **fnctset );
int ADCL_fnctset_dup    ( ADCL_fnctset_t *org, ADCL_fnctset_t *dup );
#endif
