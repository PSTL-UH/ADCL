#ifndef __ADCL_PACKUNPACK_H__
#define __ADCL_PACKUNPACK_H__

#include "mpi.h"

int ADCL_packunpack_init ( int num, int neighbors[], MPI_Comm comm, 
			   char ***sendbuf, MPI_Datatype *sdats, 
			   char ***recvbuf, MPI_Datatype *rdats );

void ADCL_packunpack_free ( int num, char ***sbuf, char ***rbuf);

#endif /* # __ADCL_PACKUNPACK_H__ */
