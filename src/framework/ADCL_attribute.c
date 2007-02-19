#include "ADCL_internal.h"

ADCL_array_t *ADCL_attribute_farray;
static int ADCL_attribute_local_counter=0;

ADCL_array_t *ADCL_attrset_farray;
static int ADCL_attrset_local_counter=0;

int ADCL_attribute_create ( int maxnvalues, int *array_of_values, 
			    ADCL_attribute_t **attribute)
{
    ADCL_attribute_t *newattribute=NULL;

    newattribute = ( ADCL_attribute_t *) calloc (1, sizeof (ADCL_attribute_t));
    if ( NULL == newattribute ) {
	return ADCL_NO_MEMORY;
    }

    newattribute->a_id = ADCL_attribute_local_counter++;
    ADCL_array_get_next_free_pos ( ADCL_attribute_farray, &(newattribute->a_findex));
    ADCL_array_set_element ( ADCL_attribute_farray, newattribute->a_findex,
			     newattribute->a_id, newattribute );

    newattribute->a_refcnt     = 1;
    newattribute->a_maxnvalues = maxnvalues;
    newattribute->a_values = (int *) malloc ( maxnvalues * sizeof (int));
    if ( NULL == newattribute->a_values ) {
	free ( newattribute );
	return ADCL_NO_MEMORY;
    }
    
    memcpy ( newattribute->a_values, array_of_values, maxnvalues*sizeof(int));
        
    *attribute = newattribute;
    return ADCL_SUCCESS;
}

int ADCL_attribute_free ( ADCL_attribute_t **attribute)
{
    ADCL_attribute_t *tattribute=*attribute;

    if ( NULL != tattribute ) {
	if ( NULL != tattribute->a_values ) {
	    free (tattribute->a_values );
	}
	ADCL_array_remove_element ( ADCL_attribute_farray, tattribute->a_findex);
	free ( attribute );
    }

    *attribute = ADCL_ATTRIBUTE_NULL;
    return ADCL_SUCCESS;
}

int ADCL_attrset_create ( int maxnum, ADCL_attribute_t **array_of_attributes, ADCL_attrset_t **attrset)
{
    ADCL_attrset_t *newattrset=NULL;
        
    newattrset = ( ADCL_attrset_t *) calloc (1, sizeof (ADCL_attrset_t));
    if ( NULL == newattrset ) {
	return ADCL_NO_MEMORY;
    }

    newattrset->as_id = ADCL_attrset_local_counter++;
    ADCL_array_get_next_free_pos ( ADCL_attrset_farray, &(newattrset->as_findex));
    ADCL_array_set_element ( ADCL_attrset_farray, newattrset->as_findex,
			     newattrset->as_id, newattrset );

    newattrset->as_refcnt     = 1;
    newattrset->as_maxnum = maxnum;
    newattrset->as_attrs = (ADCL_attribute_t **) malloc ( maxnum * sizeof (ADCL_attribute_t *));
    if ( NULL == newattrset->as_attrs ) {
	free ( newattrset );
	return ADCL_NO_MEMORY;
    }
    
    memcpy ( newattrset->as_attrs, array_of_attributes, maxnum*sizeof(ADCL_attribute_t *));
    
    *attrset = newattrset;
    return ADCL_SUCCESS;
}

int ADCL_attrset_free ( ADCL_attrset_t **attrset)
{
    ADCL_attrset_t *tattrset=*attrset;

    if ( NULL != tattrset ) {
	if ( NULL != tattrset->as_attrs ) {
	    free ( tattrset->as_attrs );
	}
	ADCL_array_remove_element ( ADCL_attrset_farray, tattrset->as_findex);
	free ( tattrset );
    }

    *attrset = ADCL_ATTRSET_NULL;
    return ADCL_SUCCESS;
}
