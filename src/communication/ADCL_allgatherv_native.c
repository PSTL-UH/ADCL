/* 
** Following are the ADCL copyrights
*/

/*
 * Copyright (c) 2008-2013 University of Houston. All rights reserved.
 * Copyright (c) 2008      High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"


void ADCL_allgatherv_native ( ADCL_request_t *req )
{
    ADCL_topology_t *topo = req->r_emethod->em_topo;
    ADCL_vmap_t *r_vmap = req->r_rvecs[0]->v_map;
    void *sbuf = req->r_svecs[0]->v_data;
    void *rbuf = req->r_rvecs[0]->v_data;
    MPI_Comm comm = topo->t_comm;
    
    MPI_Datatype sdtype;
    MPI_Datatype rdtype = req->r_rdats[0];
    int scount;

    int *rcounts = r_vmap->m_rcnts;
    int *rdispls = r_vmap->m_displ;

    if (MPI_IN_PLACE != sbuf) {
        scount = req->r_scnts[0];
        sdtype = req->r_sdats[0];
    }

    MPI_Allgatherv(sbuf, scount, sdtype, 
		   rbuf, rcounts, rdispls, rdtype, 
		   comm);

    return;
}

