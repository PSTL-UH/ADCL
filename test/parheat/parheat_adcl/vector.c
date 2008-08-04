#include <stdio.h>
#include "ADCL.h"
#include "mpi.h"

#define DIM0 8
#define DIM1 4
int main ( int argc, char ** argv )
{
	int dims[2];
	int hwidth=1;
	int comtype;
	double **data;
	ADCL_Vector vec;
	MPI_Init ( &argc, &argv );
	ADCL_Init ();
	dims[0] = DIM0 + 2*hwidth;
	dims[1] = DIM1 + 2*hwidth;
	ADCL_Vector_allocate ( 2, dims, 0, hwidth, comtype, MPI_DOUBLE, &data, &vec );
	ADCL_Vector_free ( &vec );
	ADCL_Finalize ();
	MPI_Finalize ();
	return 0;
}
