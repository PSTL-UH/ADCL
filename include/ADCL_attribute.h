#ifndef __ADCL_ATTRIBUTE_H__
#define __ADCL_ATTRIBUTE_H__

#include "mpi.h"

struct ADCL_attribute_s{
    int          a_id; /* id of the object */
    int      a_findex; /* index of this object in the fortran array */
    int      a_refcnt; /* reference counter of this object */
    int  a_maxnvalues; /* no. of different values this attribute can have */
    int*     a_values; /* list of potential values for this attribute */
			     
};
typedef struct ADCL_attribute_s ADCL_attribute_t;
extern ADCL_array_t *ADCL_attribute_farray;

int ADCL_attribute_create ( int maxnvalues, int *array_of_values, 
			    ADCL_attribute_t **attribute);
int ADCL_attribute_free   ( ADCL_attribute_t **attribute);



struct ADCL_attrset_s{
    int                   as_id; /* id of the object */
    int               as_findex; /* index of this object in the fortran array */
    int               as_refcnt; /* reference counter of this object */
    int               as_maxnum; /* no. of attributes contained in this set */
    ADCL_attribute_t **as_attrs; /* array of ADCL_attributes in this set */
};
typedef struct ADCL_attrset_s ADCL_attrset_t;
extern ADCL_array_t *ADCL_attrset_farray;

int ADCL_attrset_create ( int maxnum, ADCL_attribute_t **array_of_attributes, 
			  ADCL_attrset_t **attrset);
int ADCL_attrset_free ( ADCL_attrset_t **attrset);


#endif
