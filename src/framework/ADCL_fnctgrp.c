#include "ADCL_internal.h"

ADCL_array_t *ADCL_fnctgrp_farray;
static int ADCL_local_counter=0;

int ADCL_fnctgrp_create ( int maxnum, ADCL_fnctgrp_t **fnctgrp )
{
    ADCL_fnctgrp_t *newfnctgrp=NULL;

    newfnctgrp = ( ADCL_fnctgrp_t *) calloc (1, sizeof (ADCL_fnctgrp_t));
    if ( NULL == newfnctgrp ) {
	return ADCL_NO_MEMORY;
    }

    newfnctgrp->f_id = ADCL_local_counter++;
    ADCL_array_get_next_free_pos ( ADCL_fnctgrp_farray, &(newfnctgrp->f_findex));
    ADCL_array_set_element ( ADCL_fnctgrp_farray, newfnctgrp->f_findex,
			     newfnctgrp->f_id, newfnctgrp );

    newfnctgrp->f_maxnum = maxnum;
    newfnctgrp->f_fptrs = (ADCL_work_fnct_ptr **) calloc ( 1, maxnum*sizeof (ADCL_work_fnct_ptr *));
    if ( NULL == newfnctgrp->f_fptrs ) {
	free ( newfnctgrp );
	return ADCL_NO_MEMORY;
    }
    
    newfnctgrp->f_attrset = ADCL_ATTRSET_NULL;
    newfnctgrp->f_attrvals = NULL;
    
    *fnctgrp = newfnctgrp;
    return ADCL_SUCCESS;
}


int ADCL_fnctgrp_free ( ADCL_fnctgrp_t **fnctgrp)
{
    ADCL_fnctgrp_t *tfnctgrp=*fnctgrp;

    if ( NULL != tfnctgrp ) {
	if ( NULL != tfnctgrp->f_fptrs ) {
	    free (tfnctgrp->f_fptrs );
	}
	ADCL_array_remove_element ( ADCL_fnctgrp_farray, tfnctgrp->f_findex);
	free ( fnctgrp );
    }

    *fnctgrp = ADCL_FNCTGRP_NULL;
    return ADCL_SUCCESS;
}

int ADCL_fnctgrp_register_fnct ( ADCL_fnctgrp_t * fnctgrp, int pos, ADCL_work_fnct_ptr *fnct)
{

    if ( NULL != fnctgrp->f_attrvals ) {
	/* Somebody registered a function pointer together with attributes already.
	** Thus, all functions have to be registered with attributes in order to 
	**  maintain a consistent view for the selection logic. 
	*/
	ADCL_printf("%s: inconsistent usage of ADCL_Fnctgrp object when registering functions\n",
		    __FILE__ ) ;
	return ADCL_USER_ERROR;
    }
       
    memcpy ( fnctgrp->f_fptrs[pos], fnct, sizeof(ADCL_work_fnct_ptr));
    return ADCL_SUCCESS;
}


int ADCL_fnctgrp_register_fnct_and_attrset ( ADCL_fnctgrp_t * fnctgrp, int pos, 
					     ADCL_work_fnct_ptr *fnct,
					     ADCL_attrset_t * attrset, 
					     int *array_of_attrvalues )
{
    int i, found=0;

    for (i=0; i< fnctgrp->f_maxnum; i++ ) {
	if ( NULL != fnctgrp->f_fptrs[i] ) {
	    found = 1;
	}
    }
    
    /* If found == 1, than the user has set already some function pointers. 
    ** In order to preserve consistency, either all function pointers have 
    ** to provide attributes or none.
    */
    if ( found && ( NULL == fnctgrp->f_attrvals) ) {
	ADCL_printf("%s: inconsistent usage of ADCL_Fnctgrp object when "
		    "registering functions\n",   __FILE__ ) ;
	return ADCL_USER_ERROR;
    }
    
    if ( NULL == fnctgrp->f_attrvals ) {
	fnctgrp->f_attrvals = (int **) malloc ( fnctgrp->f_maxnum * sizeof (int *));
	if ( NULL == fnctgrp->f_attrvals ) {
	    return ADCL_NO_MEMORY;
	}
	for ( i=0; i< fnctgrp->f_maxnum; i++ ) {
	    fnctgrp->f_attrvals[i] = (int *) malloc (attrset->as_maxnum * sizeof (int ));
	    if ( NULL == fnctgrp->f_attrvals[i] ) {
		return ADCL_NO_MEMORY;
	    }
	}
	
	fnctgrp->f_attrset = attrset;
    }

    /* Make sure all functions use the same attrset, else our attribute based 
       selection logic will fail */
    if ( fnctgrp->f_attrset != attrset ) {
	ADCL_printf("%s: inconsistent usage of ADCL_Fnctgrp object when attribute"
		    " sets\n",	    __FILE__ ) ;
	return ADCL_INVALID_ATTRSET;
    }

    /* Copy the real objects */
    memcpy ( fnctgrp->f_fptrs[pos], fnct, sizeof(ADCL_work_fnct_ptr));
    memcpy ( fnctgrp->f_attrvals[pos], array_of_attrvalues, sizeof(int) * attrset->as_maxnum );

    return ADCL_SUCCESS;
}
