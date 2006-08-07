#include <mpi.h>

int main(void)
{
	int rank;
    int m,n;
	
	int gsizes[2], psizes[4], lsizes[2], dims[2],periods[2];
	int start_indices[2], coords[2];
	MPI_Comm comm;
	MPI_Datatype newtype;    
	gsizes[0]=m;
	gsizes[1]=n;
	
	psizes[0]=2;
	psizes[3]=3;
	
	lsizes[0]=m/psizes[0];
	lsizes[1]=n/psizes[1];
	
	dims[0]=2;
	dims[1]=3;
	
	periods[0]=periods[1]=1;
	MPI_Cart_create(MPI_COMM_WORLD,2,dims,periods, 0,&comm);
	MPI_Comm_rank(comm,&rank);
	MPI_Cart_coords(comm,rank,2,coords);
	start_indices[0]=coords[0]*lsizes[0];
	start_indices[1]=coords[1]*lsizes[1];
	MPI_Type_create_subarray(2,gsizes,lsizes,start_indices,MPI_ORDER_C,MPI_FLOAT,&newtype);
	MPI_Type_commit(&newtype);
	
	return MPI_SUCCESS;

}
