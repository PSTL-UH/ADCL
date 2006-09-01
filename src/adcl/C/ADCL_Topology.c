#include "ADCL.h"
#include "ADCL_internal.h"

int ADCL_Topology_create ( int ndims, int *lneighbors, int *rneighbors, 
			   int *coords, MPI_Comm comm, ADCL_Topology *topo)
{
    if ( 0 == ndims ) {
	return ADCL_INVALID_NDIMS;
    }

    if ( NULL == lneighbors || NULL == rneighbors ||
	 NULL == coords ) {
	return ADCL_INVALID_ARG;
    }

    if ( NULL == topo ) {
	return ADCL_INVALID_TOPOLOGY;
    }

    return ADCL_topology_create ( ndims, lneighbors, rneighbors, 
				  coords, comm, topo );
}

int ADCL_Topology_create_bycomm ( MPI_Comm cart_comm, ADCL_Topology *topo)
{
    int topo_type;
    
    /* Right now we can only handle cartesian topologies! */
    MPI_Topo_test ( cart_comm, &topo_type );
    if ( MPI_UNDEFINED==topo_type || MPI_GRAPH==topo_type ) {
	return ADCL_INVALID_COMM;
    }

    if ( NULL == topo ) {
	return ADCL_INVALID_TOPOLOGY;
    }

    return ADCL_topology_create_bycomm ( cart_comm, topo );
}


int ADCL_Topology_free ( ADCL_Topology *topo )
{
    ADCL_topology_t *ptopo = *topo;

    if ( NULL == topo  ) {
	return ADCL_INVALID_ARG;
    }
    if ( ptopo->t_id < 0 ) {
	return ADCL_INVALID_TOPOLOGY;
    }

    return ADCL_topology_free ( topo );
}
