/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/*
** Codes have been adapted to be used in ADCL
** Following are the ADCL copyrights
*/

/*
 * Copyright (c) 2008      University of Houston. All rights reserved.
 * Copyright (c) 2008      High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"

void
ADCL_allreduce_native( ADCL_request_t *req ) 
{
   int rank;
   int err;

   ADCL_topology_t *topo = req->r_emethod->em_topo;
   MPI_Comm comm = topo->t_comm;
   ADCL_vmap_t *s_vmap = req->r_svecs[0]->v_map;
   MPI_Op op = s_vmap->m_op;
   void *sbuf = req->r_svecs[0]->v_data;
   void *rbuf = req->r_rvecs[0]->v_data;
 
   /* Caution, this might be a hack */
   MPI_Datatype dtype = req->r_sdats[0]; 
   int count = req->r_svecs[0]->v_dims[0];
				    
   rank = topo->t_rank;

   err = MPI_Allreduce(sbuf, rbuf, count, dtype, op, comm);
   if (MPI_SUCCESS != err) {
       return;
   }

}

