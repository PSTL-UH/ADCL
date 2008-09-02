/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2008           HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL.h"
#include "ADCL_internal.h"


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_Vmap_halo_allocate ( int vectype, int hwidth,
                 ADCL_Vmap *vmap )
{
    ADCL_vmap_t *tvmap=NULL;
    int err = ADCL_SUCCESS;         
          
    if (ADCL_VECTOR_HALO != vectype) {
        return ADCL_INVALID_VECTYPE;
    }

    if (hwidth < 0){
        return ADCL_INVALID_HWIDTH;
    }	

    err = ADCL_vmap_halo_allocate ( vectype, hwidth, &tvmap );

    *vmap = tvmap;

    return err;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_Vmap_list_allocate ( int vectype, int size, int* rcnts, int* displ, ADCL_Vmap *vmap )
{
    ADCL_vmap_t *tvmap=NULL;
    int err = ADCL_SUCCESS;         
 
    /* Halo cells are to be communicated */
    if (ADCL_VECTOR_LIST != vectype) {
        return ADCL_INVALID_VECTYPE;
    }

    if ( NULL == rcnts){
        return ADCL_INVALID_ARG;
    }	

    if ( NULL == displ) {
        return ADCL_INVALID_ARG; 
    }

    err = ADCL_vmap_list_allocate ( vectype, size, rcnts, displ, &tvmap );

    *vmap = tvmap;
    return err;
} 
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_Vmap_allreduce_allocate ( int vectype, MPI_Op op, ADCL_Vmap *vmap )
{
    ADCL_vmap_t *tvmap=NULL;
    int err = ADCL_SUCCESS;         
 
    /* Halo cells are to be communicated */
    if (ADCL_VECTOR_ALLREDUCE != vectype) {
        return ADCL_INVALID_VECTYPE;
    }

    if ( op != MPI_MIN && op != MPI_MAX && op != MPI_SUM){
       return ADCL_INVALID_ARG;
    }

    err = ADCL_vmap_allreduce_allocate ( vectype, op, &tvmap );

    *vmap = tvmap;
    return err;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_Vmap_all_allocate ( int vectype, ADCL_Vmap *vmap )
{
    ADCL_vmap_t *tvmap=NULL;
    int err = ADCL_SUCCESS;         
          
    /* Halo cells are to be communicated */
    if (ADCL_VECTOR_ALL != vectype) {
        return ADCL_INVALID_VECTYPE;
    }

    err = ADCL_vmap_all_allocate ( vectype, &tvmap );

    *vmap = tvmap;
    return err;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_Vmap_inplace_allocate ( int vectype, ADCL_Vmap *vmap )
{
    ADCL_vmap_t *tvmap=NULL;
    int err = ADCL_SUCCESS;         
          
    /* Halo cells are to be communicated */
    if (ADCL_VECTOR_INPLACE != vectype) {
        return ADCL_INVALID_VECTYPE;
    }

    err = ADCL_vmap_inplace_allocate ( vectype, &tvmap );

    *vmap = tvmap;
    return err;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
int ADCL_Vmap_free  ( ADCL_Vmap *vmap )
{
    int ret; 

    if ( NULL == vmap ) {
       return ADCL_INVALID_ARG;
    }

    ret =  ADCL_vmap_free ( vmap );

    // whatever happens internally, object has to appear deleted
    vmap = ADCL_VMAP_NULL; 

    return ret;
}


