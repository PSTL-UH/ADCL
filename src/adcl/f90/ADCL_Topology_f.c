#include "ADCL.h"
#include "ADCL_internal.h"
#include "ADCL_fprototypes.h"


#pragma weak adcl_topology_create_  = adcl_topology_create
#pragma weak adcl_topology_create__ = adcl_topology_create
#pragma weak ADCL_TOPOLOGY_CREATE   = adcl_topology_create

#pragma weak adcl_topology_free_  = adcl_topology_free
#pragma weak adcl_topology_free__ = adcl_topology_free
#pragma weak ADCL_TOPOLOGY_FREE   = adcl_topology_free


void adcl_topology_create ( int *ndims, int *lneighb, int *rneighb, int *coords, 
			   int *comm, int *topo, int *ierror )
{
    ADCL_topology_t *ctopo;
    MPI_Comm ccomm;
    
    if ( ( NULL == ndims )    || 
	 ( NULL == lneighb )  ||
	 ( NULL == rneighb  ) ||
	 ( NULL == coords )   ||
	 ( NULL == comm )     ||
	 ( NULL == topo ) )   {
	*ierror = ADCL_INVALID_ARG;
	return;
    }

    ccomm = MPI_Comm_f2c (*comm);
    if ( ccomm == MPI_COMM_NULL ) {
	*ierror = ADCL_INVALID_COMM;
	return;
    }
    
    *ierror = ADCL_topology_create ( *ndims, lneighb, rneighb, coords, ccomm, 
				     &ctopo );
    if ( *ierror == ADCL_SUCCESS ) {
	*topo = ctopo->t_findex;
    }

    return;
}

void adcl_topology_create_bycomm   ( int* cart_comm, int *topo, int *ierror )
{
    int topo_type;
    ADCL_topology_t *ctopo;
    MPI_Comm ccomm;

    if ( ( NULL == cart_comm ) ||
	 ( NULL == topo ) )    {
	*ierror = ADCL_INVALID_ARG;
	return;
    }

    ccomm = MPI_Comm_f2c (*cart_comm);
    if ( ccomm == MPI_COMM_NULL ) {
	*ierror = ADCL_INVALID_COMM;
	return;
    }
    MPI_Topo_test ( ccomm, &topo_type );
    if ( MPI_UNDEFINED==topo_type || MPI_GRAPH==topo_type ) {
        *ierror = ADCL_INVALID_COMM;
        return;
    }

    *ierror = ADCL_topology_create_bycomm ( ccomm, &ctopo );
    if ( *ierror == ADCL_SUCCESS ) {
	*topo = ctopo->t_findex;
    } 
    
    return;
}

void adcl_topology_free ( int *topo, int *ierror )
{
    ADCL_topology_t *ctopo;

    ctopo = (ADCL_topology_t *) ADCL_array_get_ptr_by_pos (ADCL_topology_farray, 
							   *topo );
    *ierror = ADCL_Topology_free ( &ctopo );
    return;
}

