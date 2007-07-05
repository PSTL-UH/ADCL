/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"

static int ADCL_local_id_counter = 0;
ADCL_array_t *ADCL_data_array = NULL;

static void ADCL_data_dump_to_file ( void );
static void ADCL_data_read_from_file ( void );

int ADCL_data_create ( ADCL_emethod_t *e ) 
{

    ADCL_data_t *data;
    ADCL_topology_t *topo = e->em_topo;
    ADCL_vector_t   *vec  = e->em_vec;
    int i;

    if ( ADCL_VECTOR_NULL == vec ) {
        return ADCL_INVALID_ARG;
    }
    data = (ADCL_data_t *) calloc (1, sizeof(ADCL_data_t));
    if ( NULL == data ) {
        return ADCL_NO_MEMORY;
    }
    /* Internal info for object management */
    data->d_id = ADCL_local_id_counter++;
    ADCL_array_get_next_free_pos ( ADCL_data_array, &data->d_findex );
    ADCL_array_set_element ( ADCL_data_array, data->d_findex, data->d_id, data );
    data->d_refcnt = 1;
    /* Topology information */
    data->d_comm  = topo->t_comm;
    data->d_tndims = topo->t_ndims;
    data->d_neighbors = (int *)malloc( 2*data->d_tndims*sizeof(int) );
    for ( i=0; i<2*data->d_tndims ;i++ ) {
        data->d_neighbors[i] = topo->t_neighbors[i];
    }
    /* Vector information */
    data->d_vndims = vec->v_ndims;
    data->d_vdims = (int *)malloc(data->d_vndims*sizeof(int) );
    for ( i=0; i<data->d_vndims ;i++ ) {
        data->d_vdims[i] = vec->v_dims[i];
    }
    data->d_nc = vec->v_nc;
    data->d_hwidth = vec->v_hwidth;
    data->d_comtype = vec->v_comtype;
    /* Function set and winner function */
    data->d_fnctset     = e->em_orgfnctset;
    data->d_wfunction   = e->em_wfunction;

    return ADCL_SUCCESS;
}

void ADCL_data_free ( void )
{
    int i, last;
    ADCL_data_t *data;

    ADCL_data_dump_to_file ( );

    last = ADCL_array_get_last ( ADCL_data_array );
    /* Free all the data objects */
    for ( i=0; i<= last; i++ ) {
        data = ( ADCL_data_t * ) ADCL_array_get_ptr_by_pos( ADCL_data_array, i );
        if ( NULL != data  ) {
            if ( NULL != data->d_neighbors ) {
                free ( data->d_neighbors );
            }
            if ( NULL != data->d_vdims ) {
                free ( data->d_vdims );
            }
            ADCL_array_remove_element ( ADCL_data_array, data->d_findex );
            free ( data );
        }
    }
    return;
}

int ADCL_data_find ( ADCL_emethod_t *e, ADCL_data_t **found_data )
{
    ADCL_data_t *data;
    ADCL_topology_t *topo = e->em_topo;
    ADCL_vector_t   *vec  = e->em_vec;
    int ret = ADCL_UNEQUAL;
    int i, j, last, found, result;

    ADCL_data_read_from_file ( );

    last = ADCL_array_get_last ( ADCL_data_array );
    for ( i=0; i<= last; i++ ) {
        data = ( ADCL_data_t * ) ADCL_array_get_ptr_by_pos( ADCL_data_array, i );
        if ( ADCL_VECTOR_NULL == vec ) {
            continue;
        }
        MPI_Comm_compare ( topo->t_comm, data->d_comm, &result );
        if ( ( result != MPI_IDENT) && (result != MPI_CONGRUENT) ) {
            continue;
        }
        found = i;
        if ( ( e->em_orgfnctset == data->d_fnctset ) &&
             ( topo->t_ndims    == data->d_tndims  ) &&
             ( vec->v_ndims     == data->d_vndims  ) &&
             ( vec->v_nc        == data->d_nc      ) &&
             ( vec->v_comtype   == data->d_comtype ) &&
             ( vec->v_hwidth    == data->d_hwidth  ) ) {
            for ( j=0; j<(2*topo->t_ndims); j++ ) {
                if ( topo->t_neighbors[i] != data->d_neighbors[i] ) {
                    found = -1;
                    break;
                }
            }
            for ( j=0 ; j<vec->v_ndims; j++ ){
                if ( vec->v_dims[i] != data->d_vdims[i] ) {
                    found = -1;
                    break;
	        }
	    }
	}
	else {
            continue;
        }
        if ( found == -1 ) {
            continue;
        }
	else {
            *found_data = data;
	    ret = ADCL_IDENT;
            ADCL_printf("#%d An identical problem is found, winner is %d %s \n",
                        topo->t_rank, data->d_wfunction->f_id ,
                        data->d_wfunction->f_name );
        }
    }
    return ret;
}

void ADCL_data_dump_to_file ( void )
{
    return;
}

void ADCL_data_read_from_file ( void )
{
    return;
}
