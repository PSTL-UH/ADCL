/*
 * Copyright (c) 2006-2013      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"

static int ADCL_vector_get_realdim ( int ndims, int *dims, int nc,
                     int *rndims, int **rdims );

ADCL_array_t* ADCL_vector_farray=NULL;
static int ADCL_local_vector_counter=0;


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_allocate_generic ( int ndims, int *dims, int nc,
           ADCL_vmap_t *vmap, MPI_Datatype dat, ADCL_vector_t **vec )
{
    ADCL_vector_t *tvec=NULL;
    int rndims, *rdims=NULL;

    /* Verification of the input parameters has been done in the
       user level layer. Allocate now a new vector object */
    tvec = (ADCL_vector_t *) calloc ( 1, sizeof (ADCL_vector_t) );
    if ( NULL == tvec ) {
        return ADCL_NO_MEMORY;
    }

    /* Set the according elements of the structure */
    tvec->v_id     = ADCL_local_vector_counter++;
    tvec->v_rfcnt  = 1;

    ADCL_array_get_next_free_pos ( ADCL_vector_farray, &(tvec->v_findex) );
    ADCL_array_set_element ( ADCL_vector_farray,
                 tvec->v_findex,
                 tvec->v_id,
                 tvec );

    ADCL_vector_get_realdim ( ndims, dims, nc, &rndims, &rdims );
    tvec->v_ndims   = rndims;
    tvec->v_nc      = nc;
    tvec->v_map     = vmap;
    tvec->v_map->m_rfcnt++;
    tvec->v_dims    = rdims;

    tvec->v_alloc   = TRUE;
    tvec->v_dat     = dat;

    if ( 0 < tvec->v_ndims) {
       /* allocate the according data array */
       tvec->v_data = ADCL_allocate_matrix ( rndims, rdims, dat, &(tvec->v_matrix) );
       if ( NULL == tvec->v_data ) {
           ADCL_vector_free ( &(tvec) );
           *vec = NULL;
           return ADCL_ERROR_INTERNAL;
       }
    }
    else {
       if ( ADCL_VECTOR_INPLACE == vmap->m_vectype ) {
	  tvec->v_data = MPI_IN_PLACE; 
       }
    }

    *vec = tvec;
    return ADCL_SUCCESS;


}
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_free  ( ADCL_vector_t **vec )
{
    ADCL_vector_t *tvec=*vec;
    int ret = ADCL_SUCCESS; 
    
    if ( NULL == tvec || ADCL_VECTOR_NULL == tvec ) {
	   return ADCL_ERROR_INTERNAL;
    }

    if ( NULL != tvec->v_map && ADCL_VMAP_NULL != tvec->v_map ) {
	ret = ADCL_vmap_free( &(tvec->v_map) ); 
    }	

    tvec->v_rfcnt--;
    if ( tvec->v_rfcnt == 0 ) {
        if ( NULL != tvec->v_dims ) {
            free ( tvec->v_dims);
        }

        ADCL_free_matrix ( tvec->v_ndims, tvec->v_dat, tvec->v_matrix );
        ADCL_array_remove_element ( ADCL_vector_farray, tvec->v_findex );
        free ( tvec );
        *vec = ADCL_VECTOR_NULL;
    }
    else if ( tvec->v_rfcnt < 0 ) {
        ret = ADCL_ERROR_INTERNAL;
    }

    return ret;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_register_generic ( int ndims, int *dims, int nc, ADCL_vmap_t *vmap,
               MPI_Datatype dat, void *data,  ADCL_vector_t **vec )
{
    ADCL_vector_t *tvec;
    int rndims, *rdims=NULL;


    /* Allocate a new vector object */
    tvec = (ADCL_vector_t *) malloc (sizeof(ADCL_vector_t) );
    if ( NULL == tvec ) {
        return ADCL_NO_MEMORY;
    }

    /* Set the according elements of the structure */
    tvec->v_id     = ADCL_local_vector_counter++;
    tvec->v_rfcnt  = 1;
    tvec->v_alloc  = FALSE;

    ADCL_array_get_next_free_pos ( ADCL_vector_farray, &(tvec->v_findex) );
    ADCL_array_set_element ( ADCL_vector_farray,
                             tvec->v_findex,
                             tvec->v_id,
                             tvec );

    ADCL_vector_get_realdim ( ndims, dims, nc, &rndims, &rdims );
    tvec->v_ndims   = rndims;
    tvec->v_nc      = nc;
    tvec->v_map     = vmap;
    tvec->v_map->m_rfcnt++;

    tvec->v_dims    = rdims;
    tvec->v_data    = data;
    tvec->v_matrix  = NULL;
    tvec->v_dat     = dat;

    /* Finally, set the returning handle correctly */
    *vec = tvec;
    return ADCL_SUCCESS;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_deregister  ( ADCL_vector_t **vec )
{
    ADCL_vector_t *tvec=*vec;
    //ADCL_vmap_t *vmap;

    tvec->v_rfcnt--;
    if ( tvec->v_rfcnt == 0 ) {
        if ( NULL != tvec->v_dims ) {
            free ( tvec->v_dims );
        }

        ADCL_array_remove_element ( ADCL_vector_farray, tvec->v_findex );
	ADCL_vmap_free( &(tvec->v_map) );
        free ( tvec );
    }
    else if ( tvec->v_rfcnt < 0 ) {
        return ADCL_ERROR_INTERNAL;
    }

    *vec = ADCL_VECTOR_NULL;
    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
void*  ADCL_vector_get_data_ptr ( ADCL_vector_t *vec )
{
    return vec->v_matrix;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_vector_add_reference ( ADCL_vector_t *vec )
{
    if ( NULL == vec || ADCL_VECTOR_NULL == vec ) {
       return ADCL_INVALID_VECTOR;
    }

    vec->v_rfcnt++;
    if ( NULL != vec->v_map ) {
       if ( ADCL_VMAP_NULL == vec->v_map) {
	  return ADCL_ERROR_INTERNAL;
       }
       vec->v_map->m_rfcnt++;
    }

    return ADCL_SUCCESS;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* non-public function */
static int ADCL_vector_get_realdim ( int ndims, int *dims, int nc,
                     int *rndims, int **rdims )
{
    /* the user may define an array as
       array[n1][n2] with n_c=0
       or
       array[n1][n2][1] with n_c=1  */

    int *ldims=NULL;
    int i, lndims;

    if ( nc == 0 ) {
        lndims = ndims;
    }
    else {
        lndims = ndims + 1;
    }

    ldims = (int *) malloc ( lndims * sizeof ( int));
    if ( NULL == ldims ) {
        return ADCL_NO_MEMORY;
    }
    for ( i=0; i<ndims; i++ ) {
        ldims[i] = dims[i];
    }

    if ( nc != 0 ) {
        ldims[lndims-1] = nc;
    }

    *rndims = lndims;
    *rdims = ldims;
    return ADCL_SUCCESS;
}

