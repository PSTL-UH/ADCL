#ifndef __ADCL_REDUCE__
#define __ADCL_REDUCE__

#define ADCL_TAG_REDUCE 125

int ADCL_ddt_copy_content_same_ddt(MPI_Datatype dtype, int count, char* dest, char* src);
int ADCL_op_reduce( MPI_Op op, char *source, char *target,
                                  int count, MPI_Datatype dtype); 

int ADCL_basic_init ( MPI_Datatype btype, int cnt, MPI_Datatype **dats, int** cnts ); 
void ADCL_basic_free (MPI_Datatype **dats, int **cnts ); 
#endif
