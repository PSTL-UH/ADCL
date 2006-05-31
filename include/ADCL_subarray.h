#ifndef __ADCL_SUBARRAY_H__
#define __ADCL_SUBARRAY_H__

#include "mpi.h"
#include "ADCL_request.h"

/* ADCL_subarray_c:
   Description: generate the derived datatypes which describe, 
                which part of the array has to be sent/received
		to the according neighbor. It uses MPI_Type_create_subarray
		because of the simplicity to map it to the according object.
		
		Right now, I assume that this routine only works for 
		C like row-major ordering.

   @param numdims: number of dimenions:
   @param ldims:   array of dimension numdims containing the extent  
                   of the matrix in each dimension
   @param hwidth:  number of halo-cells to be considered.
                   assumption: ldims[...] contains 2*hwidth in 
                   each direction
   @param neighbors: array of dimension 2*numdims containing the ranks 
                     of neighboring processes. The array is assumed to 
		     be structured such that left and right neighbor 
		     for a certain dimension follow each other in this array. 
		     If a neighbor does not exist (e.g. this process
		     is at a boundary), the according value is 
		     assumed to be set to MPI_PROC_NULL.
		     So, neighbor[2*i] contains the rank of the left neighbor
		     process in dimension i, while neighbor[2*i+1] contains
		     the rank of the righ neighbor process in dimension i.
   @param order:  MPI_ORDER_C or MPI_ORDER_FORTRAN
		     
   @ret sdats:    array of MPI_Datatypes of dimension 2*numdims to be 
                  used for sending data. The left and right neighbor
		  for dimension x are subsequent in this array. 
   @ret rdats:    array of MPI_Datatypes of dimension 2*numdims to be 
                  used for receiving data. The left and right neighbor
		  for dimension x are subsequent in this array. 

*/

int  ADCL_subarray_init ( int topodims, int *neighbors, int vecndims, int *vecdims, 
			  int hwidth, int nc, int order, 
			  MPI_Datatype **sdats, MPI_Datatype **rdats);
void ADCL_subarray_free ( int num, MPI_Datatype **sdats, MPI_Datatype **rdats );


#endif /* __ADCL_SUBARRAY_H__ */
