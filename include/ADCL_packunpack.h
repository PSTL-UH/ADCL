/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_PACKUNPACK_H__
#define __ADCL_PACKUNPACK_H__

#include "mpi.h"

int ADCL_packunpack_init ( int num, int neighbors[], MPI_Comm comm,
               char ***sendbuf, MPI_Datatype *sdats,
               int **sp, char ***recvbuf, MPI_Datatype *rdats,
               int **rp);


void ADCL_packunpack_free ( int num, char ***sbuf, char ***rbuf,
                int **sp, int **rp );

#endif /* # __ADCL_PACKUNPACK_H__ */
