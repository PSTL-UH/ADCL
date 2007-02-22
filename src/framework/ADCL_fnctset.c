#include "ADCL_internal.h"
#include <string.h>

ADCL_array_t *ADCL_fnctset_farray;
static int ADCL_local_counter=0;

int ADCL_fnctset_create ( int maxnum, char *name, ADCL_fnctset_t **fnctset )
{
    int i, ret = ADCL_SUCCESS;
    ADCL_fnctset_t *newfnctset=NULL;

    newfnctset = ( ADCL_fnctset_t *) calloc (1, sizeof (ADCL_fnctset_t));
    if ( NULL == newfnctset ) {
	return ADCL_NO_MEMORY;
    }

    newfnctset->f_id = ADCL_local_counter++;
    ADCL_array_get_next_free_pos ( ADCL_fnctset_farray, &(newfnctset->f_findex));
    ADCL_array_set_element ( ADCL_fnctset_farray, newfnctset->f_findex,
			     newfnctset->f_id, newfnctset );

    newfnctset->f_maxnum = maxnum;
    newfnctset->f_fptrs = (ADCL_work_fnct_ptr **) calloc ( 1, maxnum*sizeof (ADCL_work_fnct_ptr*));
    if ( NULL == newfnctset->f_fptrs ) {
	ret = ADCL_NO_MEMORY;
	goto exit;
    }
    
    newfnctset->f_attrset = ADCL_ATTRSET_NULL;
    newfnctset->f_attrvals = NULL;

    newfnctset->f_fnctnames = (char **) calloc ( 1, maxnum * sizeof (char *));
    if ( NULL == newfnctset->f_fnctnames ) {
	ret =  ADCL_NO_MEMORY;
	goto exit;
    }
    for ( i=0; i< maxnum; i++ ) {
	newfnctset->f_fnctnames[i] = (char *) malloc ( ADCL_MAX_NAMELEN );
	if ( NULL == newfnctset->f_fnctnames[i] ) {
	    ret =  ADCL_NO_MEMORY;
	    goto exit;
	}
    }

    if ( NULL != name ) {
	strncpy ( newfnctset->f_fnctsetname, name, ADCL_MAX_NAMELEN);
    }

 exit:
    if ( ret != ADCL_SUCCESS ) {
	if ( NULL != newfnctset->f_fptrs ) {
	    free ( newfnctset->f_fptrs );
	}

	if ( NULL != newfnctset->f_fnctnames ) {
	    for ( i=0; i<maxnum; i++ ) {
		if ( NULL != newfnctset->f_fnctnames[i] ) {
		    free ( newfnctset->f_fnctnames[i] );
		}
	    }
	    free ( newfnctset->f_fnctnames );
	}
    }

    *fnctset = newfnctset;
    return ADCL_SUCCESS;
}


int ADCL_fnctset_free ( ADCL_fnctset_t **fnctset)
{
    int i;
    ADCL_fnctset_t *tfnctset=*fnctset;

    if ( NULL != tfnctset ) {
	if ( NULL != tfnctset->f_fptrs ) {
	    free (tfnctset->f_fptrs );
	}
	ADCL_array_remove_element ( ADCL_fnctset_farray, tfnctset->f_findex);

	if ( NULL != tfnctset->f_fnctnames ) {
	    for ( i=0; i<tfnctset->f_maxnum; i++ ) {
		if ( NULL != tfnctset->f_fnctnames[i] ) {
		    free ( tfnctset->f_fnctnames[i] );
		}
	    }
	    free ( tnfcntset->f_fnctnames );
	}
	free ( fnctset );
    }    

    *fnctset = ADCL_FNCTSET_NULL;
    return ADCL_SUCCESS;
}

int ADCL_fnctset_register_fnct ( ADCL_fnctset_t * fnctset, int pos, ADCL_work_fnct_ptr *fnct, 
				 char *name)
{

    if ( NULL != fnctset->f_attrvals ) {
	/* Somebody registered a function pointer together with attributes already.
	** Thus, all functions have to be registered with attributes in order to 
	**  maintain a consistent view for the selection logic. 
	*/
	ADCL_printf("%s: inconsistent usage of ADCL_Fnctset object when registering functions\n",
		    __FILE__ ) ;
	return ADCL_USER_ERROR;
    }
       
    memcpy ( fnctset->f_fptrs[pos], fnct, sizeof(ADCL_work_fnct_ptr));
    if ( NULL != name  ) {
	strncpy (fnctset->f_fnctnames[pos], name, ADCL_MAX_NAMELEN );
    }
    return ADCL_SUCCESS;
}


int ADCL_fnctset_register_fnct_and_attrset ( ADCL_fnctset_t * fnctset, int pos, 
					     ADCL_work_fnct_ptr *fnct,
					     ADCL_attrset_t * attrset, 
					     int *array_of_attrvalues, char *name )
{
    int i, found=0;

    for (i=0; i< fnctset->f_maxnum; i++ ) {
	if ( NULL != fnctset->f_fptrs[i] ) {
	    found = 1;
	}
    }
    
    /* If found == 1, than the user has set already some function pointers. 
    ** In order to preserve consistency, either all function pointers have 
    ** to provide attributes or none.
    */
    if ( found && ( NULL == fnctset->f_attrvals) ) {
	ADCL_printf("%s: inconsistent usage of ADCL_Fnctset object when "
		    "registering functions\n",   __FILE__ ) ;
	return ADCL_USER_ERROR;
    }
    
    if ( NULL == fnctset->f_attrvals ) {
	fnctset->f_attrvals = (int **) malloc ( fnctset->f_maxnum * sizeof (int *));
	if ( NULL == fnctset->f_attrvals ) {
	    return ADCL_NO_MEMORY;
	}
	for ( i=0; i< fnctset->f_maxnum; i++ ) {
	    fnctset->f_attrvals[i] = (int *) malloc (attrset->as_maxnum * sizeof (int ));
	    if ( NULL == fnctset->f_attrvals[i] ) {
		return ADCL_NO_MEMORY;
	    }
	}
	
	fnctset->f_attrset = attrset;
    }

    /* Make sure all functions use the same attrset, else our attribute based 
       selection logic will fail */
    if ( fnctset->f_attrset != attrset ) {
	ADCL_printf("%s: inconsistent usage of ADCL_Fnctset object when attribute"
		    " sets\n",	    __FILE__ ) ;
	return ADCL_INVALID_ATTRSET;
    }

    /* Copy the real objects */
    memcpy ( fnctset->f_fptrs[pos], fnct, sizeof(ADCL_work_fnct_ptr));
    memcpy ( fnctset->f_attrvals[pos], array_of_attrvalues, sizeof(int) * attrset->as_maxnum );
    if ( NULL != name  ) {
	strncpy (fnctset->f_fnctnames[pos], name, ADCL_MAX_NAMELEN );
    }

    return ADCL_SUCCESS;
}
