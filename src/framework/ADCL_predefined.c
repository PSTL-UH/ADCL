/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include "ADCL_internal.h"
#include "ADCL_config.h"

extern int ADCL_emethod_use_perfhypothesis;

ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_NN_TOTAL_NUM];
ADCL_attrset_t *ADCL_neighborhood_attrset;

ADCL_function_t *ADCL_neighborhood_functions[ADCL_METHOD_NN_TOTAL_NUM];
ADCL_fnctset_t *ADCL_neighborhood_fnctset;

ADCL_fnctset_t *ADCL_fnctset_rtol;
ADCL_fnctset_t *ADCL_fnctset_ltor;


const int ADCL_attr_mapping_aao=100;
const int ADCL_attr_mapping_pair=101;
const int ADCL_attr_mapping_hierarch=102;

const int ADCL_attr_noncont_ddt=110;
const int ADCL_attr_noncont_pack=111;
const int ADCL_attr_noncont_individual=112;

const int ADCL_attr_transfer_nn_IsendIrecv=120;
const int ADCL_attr_transfer_nn_SendIrecv=121;
const int ADCL_attr_transfer_nn_Send_Recv=122;
const int ADCL_attr_transfer_nn_Sendrecv=123;
#ifdef MPI_WIN
const int ADCL_attr_transfer_nn_FenceGet=124;
const int ADCL_attr_transfer_nn_FencePut=125;
const int ADCL_attr_transfer_nn_PostStartGet=126;
const int ADCL_attr_transfer_nn_PostStartPut=127;
#endif
const int ADCL_attr_transfer_allgatherv_linear=140;

const int ADCL_attr_numblocks_single=130;
const int ADCL_attr_numblocks_dual=131;

ADCL_attribute_t *ADCL_allgatherv_attrs[ADCL_ATTR_ALLGATHERV_TOTAL_NUM];
ADCL_attrset_t *ADCL_allgatherv_attrset;
ADCL_function_t *ADCL_allgatherv_functions[ADCL_METHOD_ALLGATHERV_TOTAL_NUM];
ADCL_fnctset_t *ADCL_allgatherv_fnctset;

ADCL_attribute_t *ADCL_allreduce_attrs[ADCL_ATTR_ALLREDUCE_TOTAL_NUM];
ADCL_attrset_t *ADCL_allreduce_attrset;
ADCL_function_t *ADCL_allreduce_functions[ADCL_METHOD_ALLREDUCE_TOTAL_NUM];
ADCL_fnctset_t *ADCL_allreduce_fnctset;

int ADCL_predefined_init ( void )
{
    int count=0;
    int m_nn_attr[3];
    int m_allgatherv_attr[1];
    int m_allreduce_attr[1];
    int ADCL_attr_mapping[ADCL_ATTR_MAPPING_MAX];
    int ADCL_attr_noncont[ADCL_ATTR_NONCONT_MAX];
    int ADCL_attr_transfer_nn[ADCL_ATTR_TRANSFER_MAX];
    //int ADCL_attr_transfer_allgatherv[ADCL_ATTR_TRANSFER_MAX];

    char *ADCL_attr_mapping_names[ADCL_ATTR_MAPPING_MAX] = { "aao ", "pair" };
    char *ADCL_attr_noncont_names[ADCL_ATTR_NONCONT_MAX] = { "ddt ","pack" };
#ifdef MPI_WIN
    char *ADCL_attr_transfer_nn_names[ADCL_ATTR_TRANSFER_MAX] = { "IsendIrecv", "SendIrecv",
                                                               "Send_Recv", "Sendrecv",
                                                               "FenceGet", "FencePut",
                                                               "StartPostGet","StartPostPut" };
#else
    char *ADCL_attr_transfer_nn_names[ADCL_ATTR_TRANSFER_MAX] = { "IsendIrecv", "SendIrecv",
                                                               "Send_Recv", "Sendrecv" };
#endif
    //char *ADCL_attr_transfer_allgatherv_names[ADCL_ATTR_TRANSFER_MAX] = { "linear" };
       /* bruck, linear, neighbor_exchange, ring, two_procs */

/* ******************************************************************** */
/* NEIGHBORHOOD  - Fortran function set 0                               */
/* ******************************************************************** */
    ADCL_attr_mapping[0] = ADCL_attr_mapping_aao;
    ADCL_attr_mapping[1] = ADCL_attr_mapping_pair;

    ADCL_attr_noncont[0] = ADCL_attr_noncont_ddt;
    ADCL_attr_noncont[1] = ADCL_attr_noncont_pack;

    ADCL_attr_transfer_nn[0] = ADCL_attr_transfer_nn_IsendIrecv;
    ADCL_attr_transfer_nn[1] = ADCL_attr_transfer_nn_SendIrecv;
    ADCL_attr_transfer_nn[2] = ADCL_attr_transfer_nn_Send_Recv;
    ADCL_attr_transfer_nn[3] = ADCL_attr_transfer_nn_Sendrecv;
#ifdef MPI_WIN
    ADCL_attr_transfer_nn[4] = ADCL_attr_transfer_nn_FenceGet;
    ADCL_attr_transfer_nn[5] = ADCL_attr_transfer_nn_FencePut;
    ADCL_attr_transfer_nn[6] = ADCL_attr_transfer_nn_PostStartGet;
    ADCL_attr_transfer_nn[7] = ADCL_attr_transfer_nn_PostStartPut;
#endif

    /* Define the attributes and the attributeset for the n-dimensional
       neighborhood communication */
    ADCL_attribute_create ( ADCL_ATTR_MAPPING_MAX, ADCL_attr_mapping,
                            ADCL_attr_mapping_names , "mapping",
                            &ADCL_neighborhood_attrs[0]);
    ADCL_attribute_create ( ADCL_ATTR_NONCONT_MAX, ADCL_attr_noncont,
                            ADCL_attr_noncont_names , "non contiguous",
                            &ADCL_neighborhood_attrs[1]);
    ADCL_attribute_create ( ADCL_ATTR_TRANSFER_MAX, ADCL_attr_transfer_nn,
                            ADCL_attr_transfer_nn_names , "transfer primitive",
                            &ADCL_neighborhood_attrs[2]);

    ADCL_attrset_create ( ADCL_ATTR_NN_TOTAL_NUM, ADCL_neighborhood_attrs,
                          &ADCL_neighborhood_attrset);



    /* Register function aao, ddt, IsendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_IsendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_IsendIrecv, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "IsendIrecv_aao",
                                 & ADCL_neighborhood_functions[count]);

    count++;

    /* pair, ddt, IsendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_IsendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_IsendIrecv, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "IsendIrecv_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;

    /* aao, pack, IsendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_IsendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_IsendIrecv_pack, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "IsendIrecv_aao_pack",
                                 &ADCL_neighborhood_functions[count]);
    count++;


    /* pair, pack, IsendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_IsendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_IsendIrecv_pack, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "IsendIrecv_pair_pack",
                                 &ADCL_neighborhood_functions[count]);
    count++;


    /* aao, ddt, SendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_SendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_SendIrecv, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "SendIrecv_aao",
                                 &ADCL_neighborhood_functions[count]);
    count++;

    /* pair, ddt, SendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_SendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_SendIrecv, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "SendIrecv_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;

    /* aao, pack, SendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_SendIrecv;
    /*  m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_SendIrecv_pack, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "SendIrecv_aao_pack",
                                 &ADCL_neighborhood_functions[count]);
    count++;

    /* pair, pack, SendIrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_SendIrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_SendIrecv_pack, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "SendIrecv_pair_pack",
                                 &ADCL_neighborhood_functions[count]);
    count++;


    /* pair, ddt, Send_Recv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_Send_Recv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_Send_Recv, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "Send_Recv_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;


    /* pair, ddt, Sendrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_Sendrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_Sendrecv, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "Sendrecv_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;


    /* pair, pack, Send_Recv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_Send_Recv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_Send_Recv_pack, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "Send_Recv_pair_pack",
                                 &ADCL_neighborhood_functions[count]);
    count ++;

    /* pair, pack, Sendrecv */
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_Sendrecv;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_Sendrecv_pack, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "Sendrecv_pair_pack",
                                 &ADCL_neighborhood_functions[count]);
    count++;

#ifdef MPI_WIN

#ifdef FENCE_PUT
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_FencePut;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_win_fence_put, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "WinFencePut_aao",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#endif /* WINFENCEPUT */


#ifdef FENCE_GET
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_FenceGet;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_win_fence_get, NULL,
                 ADCL_neighborhood_attrset,
                 m_nn_attr, "WinFenceGet_aao",
                 &ADCL_neighborhood_functions[count]);
    count++;
#endif /* WINFENCEGET */

#ifdef POSTSTART_PUT
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_PostStartPut;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_post_start_put, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "PostStartPut_aao",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#endif /* POSTSTARTPUT */

#ifdef POSTSTART_GET
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_PostStartGet;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_aao_post_start_get, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "PostStartGet_aao",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#endif /* POSTSTARTGET */

#  ifdef FENCE_PUT
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_FencePut;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_win_fence_put, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "WinFencePut_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#  endif /* WINFENCEPUT */

#  ifdef FENCE_GET
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_FenceGet;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_win_fence_get, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "WinFenceGet_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#  endif /* WINFENCEGET */


#  ifdef POSTSTART_PUT /* Comm 11*/
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_PostStartPut;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_post_start_put, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "PostStartPut_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#  endif /* POSTSTARTPUT */

#  ifdef POSTSTART_GET /* Comm 12*/
    m_nn_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_nn_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_PostStartGet;
    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_function_create_async ( ADCL_change_sb_pair_post_start_get, NULL,
                                 ADCL_neighborhood_attrset,
                                 m_nn_attr, "PostStartGet_pair",
                                 &ADCL_neighborhood_functions[count]);
    count++;
#  endif /* POSTSTARTGET */
#endif /* MPI_WIN */


    ADCL_fnctset_create ( ADCL_METHOD_NN_TOTAL_NUM,
                          ADCL_neighborhood_functions,
                          "Neighborhood communication",
                          &ADCL_neighborhood_fnctset );

    if ( count != ADCL_METHOD_NN_TOTAL_NUM) {
        ADCL_printf("Total Number wrong\n");
        return ADCL_ERROR_INTERNAL;
    }

/* ******************************************************************** */
/* ALLGATHERV  - Fortran function set 1                                 */
/* ******************************************************************** */


/*    ADCL_attribute_create ( ADCL_ATTR_TRANSFER_MAX, ADCL_attr_transfer_allgatherv,
                            ADCL_attr_transfer_allgatherv_names , "transfer primitive",
                            &ADCL_allgatherv_attrs[1]);

    ADCL_attrset_create ( ADCL_ATTR_ALLGATHERV_TOTAL_NUM, ADCL_allgatherv_attrs,
                          &ADCL_allgatherv_attrset); */


    count = 0;
    /* Register function aao, ddt, IsendIrecv */
    //m_allgatherv_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_allgatherv_linear;

    ADCL_function_create_async ( ADCL_allgatherv_recursivedoubling, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv recursive doubling",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_linear, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv linear",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_bruck, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv bruck",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_neighborexchange, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv neighbor exchange",
                                 & ADCL_allgatherv_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allgatherv_ring, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv ring",
                                 & ADCL_allgatherv_functions[count]);
    count++;


    /* this algo only works for topo->t_size == 2
     * ADCL_function_create_async ( ADCL_allgatherv_two_procs, NULL,
     *                              ADCL_ATTRSET_NULL,
     *                              m_allgatherv_attr, "Allgatherv two procs",
     *                              & ADCL_allgatherv_functions[count]);
     * count++; */

//    m_nn_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_nn_PostStartPut;
//    /* m_nn_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
//    ADCL_function_create_async ( ADCL_change_sb_aao_post_start_put, NULL,
//                                 ADCL_neighborhood_attrset,
//                                 m_nn_attr, "PostStartPut_aao",
//                                 &ADCL_neighborhood_functions[count]);



    ADCL_fnctset_create ( ADCL_METHOD_ALLGATHERV_TOTAL_NUM,
                          ADCL_allgatherv_functions,
                          "AllGatherV",
                          &ADCL_allgatherv_fnctset );

    if ( count != ADCL_METHOD_ALLGATHERV_TOTAL_NUM) {
        ADCL_printf("Total Number wrong\n");
        return ADCL_ERROR_INTERNAL;
    }


/* ******************************************************************** */
/* ALLREDUCE - Fortran function set 2                                   */
/* ******************************************************************** */

    count = 0;

    ADCL_function_create_async ( ADCL_allreduce_native, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce native",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_linear, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce linear",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_nonoverlapping, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce nonoverlapping",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_recursivedoubling, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce recursive doubling",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_function_create_async ( ADCL_allreduce_ring, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allreduce_attr, "Allreduce ring",
                                 & ADCL_allreduce_functions[count]);
    count++;

    ADCL_fnctset_create ( ADCL_METHOD_ALLREDUCE_TOTAL_NUM,
                          ADCL_allreduce_functions,
                          "AllReduce",
                          &ADCL_allreduce_fnctset );


    if ( count != ADCL_METHOD_ALLREDUCE_TOTAL_NUM) {
        ADCL_printf("Total Number wrong\n");
        return ADCL_ERROR_INTERNAL;
    }








    return ADCL_SUCCESS;
}

int ADCL_predefined_finalize ( void )
{
    /* Free the created function set */
    ADCL_Fnctset_free ( &ADCL_neighborhood_fnctset );
    /* Free the atrtribute set */
    ADCL_Attrset_free ( &ADCL_neighborhood_attrset );

    return ADCL_SUCCESS;
}
