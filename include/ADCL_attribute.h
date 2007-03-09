#ifndef __ADCL_ATTRIBUTE_H__
#define __ADCL_ATTRIBUTE_H__

#include "mpi.h"

#define ADCL_ATTR_NOT_SET   -1  /* attributes not set*/
#define ADCL_ATTR_NEW_BLOCK -2  /* signal that we start a new block.
                                   Used in the performance hypothesis v2 */


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
int ADCL_attribute_dup ( ADCL_attribute_t *org, ADCL_attribute_t **copy );


struct ADCL_attrset_s{
    int                   as_id; /* id of the object */
    int               as_findex; /* index of this object in the fortran array */
    int               as_refcnt; /* reference counter of this object */
    int               as_maxnum; /* no. of attributes contained in this set */
    int       *as_attrs_baseval; /* array containing the first values of each attribute
				    in this attribute set */
    int        *as_attrs_maxval; /* array containing the last values of each attribute
				    in this attribute set */
    int        *as_attrs_numval; /* array containing the num of different values for each attribute
				    in this attribute set */
    ADCL_attribute_t **as_attrs; /* array of ADCL_attributes in this set */
};
typedef struct ADCL_attrset_s ADCL_attrset_t;
extern ADCL_array_t *ADCL_attrset_farray;

int ADCL_attrset_create ( int maxnum, ADCL_attribute_t **array_of_attributes, 
			  ADCL_attrset_t **attrset);
int ADCL_attrset_free ( ADCL_attrset_t **attrset);
int ADCL_attrset_dup ( ADCL_attrset_t *org, ADCL_attrset_t **copy );

#endif
