#ifndef __ADCL_TOPOLOGY_H__
#define __ADCL_TOPOLOGY_H__

#include "mpi.h"

struct ADCL_topology_s{
    int         t_id; /* id of the object */
    int     t_findex; /* index of this object in the fortran array */
    MPI_Comm  t_comm; /* communicator used for data exchange */
    int      t_ndims; /* number of dimension of this process topology */
    int *t_neighbors; /* array of neighboring processes, dimension 
			 2*t_ndims */    
    int    *t_coords; /* coordinate of this proc in the proc-topology, 
			 dimension t_ndims */
};
typedef struct ADCL_topology_s ADCL_topology_t;
extern ADCL_array_t *ADCL_topology_farray;

int ADCL_topology_create ( int ndims, int *lneighbors, int *rneighbors, 
			   int *coords, MPI_Comm comm, ADCL_topology_t **topo);
int ADCL_topology_create_bycomm ( MPI_Comm cart_comm, ADCL_topology_t **topo );
int ADCL_topology_free ( ADCL_topology_t **topo);

#endif
