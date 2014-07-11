/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * Copyright (c) 2009           HLRS. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#ifndef __ADCL_NBC_H__
#define __ADCL_NBC_H__

#ifdef ADCL_DUMMY_MPI
#include "ADCL_dummy_mpi.h"
#else
#include "mpi.h"
#endif

struct ADCL_Ibcast_args_s {
  int alg;
  int segsize;
  int fanout;
  NBC_Schedule *schedule;
};

struct ADCL_Ialltoall_args_s {
  int alg;
  NBC_Schedule *schedule;
};

struct ADCL_Iallgather_args_s {
  int alg;
  NBC_Schedule *schedule;
};

struct ADCL_Ireduce_args_s {
  int alg;
  int segsize;
  NBC_Schedule *schedule;
};

#endif /* __ADCL_NBC_H__ */
