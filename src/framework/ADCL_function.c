#include "ADCL_internal.h"
#include <string.h>

ADCL_array_t *ADCL_function_farray;
static int ADCL_local_function_counter=0;

ADCL_array_t *ADCL_fnctset_farray;
static int ADCL_local_fnctset_counter=0;



int ADCL_function_create_async ( ADCL_work_fnct_ptr *init_fnct, 
				 ADCL_work_fnct_ptr *wait_fnct, 
				 ADCL_attrset_t * attrset, 
				 int *array_of_attrvalues, char *name, 
				 ADCL_function_t **fnct)
{
    ADCL_function_t *newfunction;
    int ret=ADCL_SUCCESS;

    newfunction = ( ADCL_function_t *) calloc (1, sizeof (ADCL_function_t));
    if ( NULL == newfunction ) {
	return ADCL_NO_MEMORY;
    }

    newfunction->f_id = ADCL_local_function_counter++;
    ADCL_array_get_next_free_pos ( ADCL_function_farray, &(newfunction->f_findex));
    ADCL_array_set_element ( ADCL_function_farray, newfunction->f_findex,
			     newfunction->f_id, newfunction );

    newfunction->f_attrset = attrset;
    if (ADCL_ATTRSET_NULL != attrset ) {
	newfunction->f_attrvals = (int *) malloc ( sizeof(int) * attrset->as_maxnum );
	if ( NULL == newfunction->f_attrvals ) {
	    ret = ADCL_NO_MEMORY;
	    goto exit;
	}
	memcpy (newfunction->f_attrvals, array_of_attrvalues, attrset->as_maxnum*sizeof(int));
    }

    if ( NULL != name ) {
	newfunction->f_name = strdup ( name );
    }

    newfunction->f_iptr = init_fnct;
    newfunction->f_wptr = wait_fnct;
    if ( NULL != wait_fnct ) {
	newfunction->f_db   = 1; /* true */
    }

 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != newfunction->f_attrvals ) {
	    free ( newfunction->f_attrvals );
	}
	if ( NULL != newfunction->f_name ) {
	    free ( newfunction->f_name );
	}
	free ( newfunction );
    }

    *fnct = newfunction;
    return ret;
}
/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
int ADCL_function_free ( ADCL_function_t **fnct )
{
    ADCL_function_t *tfnct=*fnct;

    ADCL_array_remove_element ( ADCL_function_farray, tfnct->f_findex);

    if ( NULL != tfnct->f_attrvals ) {
	free ( tfnct->f_attrvals );
    }
    if ( NULL != tfnct->f_name ) {
	free ( tfnct->f_name );
    }
    free ( tfnct );

    *fnct = ADCL_FUNCTION_NULL;
    return ADCL_SUCCESS;
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
int ADCL_function_get_attrval ( ADCL_function_t *func, int attr_pos ) 
{
    if ( attr_pos > func->f_attrset->as_attrs_numval[attr_pos]) {
	return ADCL_ERROR_INTERNAL;
    }

    return func->f_attrvals[attr_pos];
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/

int ADCL_fnctset_create( int maxnum, ADCL_function_t **fncts, char *name, 
			 ADCL_fnctset_t **fnctset )
{
    int i, ret = ADCL_SUCCESS;
    ADCL_fnctset_t *newfnctset=NULL;

    newfnctset = ( ADCL_fnctset_t *) calloc (1, sizeof (ADCL_fnctset_t));
    if ( NULL == newfnctset ) {
	return ADCL_NO_MEMORY;
    }

    newfnctset->fs_id = ADCL_local_fnctset_counter++;
    ADCL_array_get_next_free_pos ( ADCL_fnctset_farray, &(newfnctset->fs_findex));
    ADCL_array_set_element ( ADCL_fnctset_farray, newfnctset->fs_findex,
			     newfnctset->fs_id, newfnctset );

    /* Make sure all functions have use the same attribute set */
    for (i=1; i<maxnum; i++ ) {
	if ( fncts[i]->f_attrset != fncts[0]->f_attrset ) {
	    ADCL_printf("Error Generating a function set: inconsistent attribute set across "
			"multiple functions ");
	    ret = ADCL_USER_ERROR;
	    goto exit;
	}
    }
    
    newfnctset->fs_attrset = fncts[0]->f_attrset;
    newfnctset->fs_maxnum  = maxnum;
    newfnctset->fs_fptrs   = (ADCL_function_t **) calloc ( 1, maxnum*sizeof (ADCL_function_t *));
    if ( NULL == newfnctset->fs_fptrs ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    memcpy ( newfnctset->fs_fptrs, fncts, maxnum * sizeof (ADCL_function_t *));

    if ( NULL != name ) {
	newfnctset->fs_name = strdup ( name );
    }

 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != newfnctset->fs_fptrs ) {
	    free ( newfnctset->fs_fptrs );
	}

	if ( NULL != newfnctset->fs_name ) {
	    free ( newfnctset->fs_name );
	}
    }

    *fnctset = newfnctset;
    return ret;
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
/* Please note, memory for the copy argument is already allocated ! */
int ADCL_fnctset_dup ( ADCL_fnctset_t *org, ADCL_fnctset_t *copy )
{
    int ret = ADCL_SUCCESS;

    copy->fs_id      = ADCL_local_fnctset_counter++;
    copy->fs_findex  = -1; /* not set */

    if ( ADCL_ATTRSET_NULL == org->fs_attrset ) {
	copy->fs_attrset = ADCL_ATTRSET_NULL;
    }
    else {
	ADCL_attrset_dup ( org->fs_attrset, &copy->fs_attrset );
    }
    

    copy->fs_maxnum  = org->fs_maxnum;
    copy->fs_fptrs   = (ADCL_function_t**)calloc(1,org->fs_maxnum*sizeof(ADCL_function_t*));
    if ( NULL == copy->fs_fptrs ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    memcpy ( copy->fs_fptrs, org->fs_fptrs, org->fs_maxnum * sizeof (ADCL_function_t *));

    if ( NULL != org->fs_name ) {
	copy->fs_name = strdup ( org->fs_name );
    }

 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != copy->fs_fptrs ) {
	    free ( copy->fs_fptrs );
	}

	if ( NULL != copy->fs_name ) {
	    free ( copy->fs_name );
	}
    }

    return ret;
}

/********************************************************************************/
/********************************************************************************/
/********************************************************************************/
int ADCL_fnctset_free ( ADCL_fnctset_t **fnctset)
{
    ADCL_fnctset_t *tfnctset=*fnctset;

    if ( NULL != tfnctset ) {
	if ( NULL != tfnctset->fs_fptrs ) {
	    free (tfnctset->fs_fptrs );
	}
	if ( tfnctset->fs_findex != -1 ) {
	    ADCL_array_remove_element ( ADCL_fnctset_farray, tfnctset->fs_findex);
	}
	else {
	    /* Free the attrset generated during the fnctset_dup operation */
	    ADCL_attrset_free ( &tfnctset->fs_attrset );
	}

	if ( NULL != tfnctset->fs_name ) {
	    free ( tfnctset->fs_name );
	}
	free ( tfnctset );
    }    

    *fnctset = ADCL_FNCTSET_NULL;
    return ADCL_SUCCESS;
}

