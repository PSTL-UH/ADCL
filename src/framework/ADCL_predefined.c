/*
 * Copyright (c) 2006-2007      University of Houston. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <math.h>
#include "ADCL_internal.h"
#include "ADCL_config.h"

/* External variables */
extern int ADCL_emethod_use_perfhypothesis;

/* Static functions declaration */
static void ADCL_neighborhood_read( FILE *fp, ADCL_Data data );
static void ADCL_neighborhood_write( FILE *fp, ADCL_Data data );
static int ADCL_neighborhood_filter( ADCL_Data data, void *filter_criteria );
static double ADCL_neighborhood_distance( ADCL_Data data1 , ADCL_Data data2 );

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
    ADCL_hist_functions_t *ADCL_neighborhood_hist_functions;

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
    /* Memory allocation for the structure */
    ADCL_neighborhood_hist_functions = (ADCL_hist_functions_t *)malloc(sizeof(ADCL_hist_functions_t));
    /* Initializing the structure */
    ADCL_neighborhood_hist_functions->hf_reader = (ADCL_hist_reader *)ADCL_neighborhood_read;
    ADCL_neighborhood_hist_functions->hf_writer = (ADCL_hist_reader *)ADCL_neighborhood_write;
    ADCL_neighborhood_hist_functions->hf_filter = (ADCL_hist_filter *)ADCL_neighborhood_filter;
    ADCL_neighborhood_hist_functions->hf_distance = (ADCL_hist_distance *)ADCL_neighborhood_distance;

    ADCL_fnctset_create ( ADCL_METHOD_NN_TOTAL_NUM,
                          ADCL_neighborhood_functions,
                          "Neighborhood communication",
                          &ADCL_neighborhood_fnctset );

    ADCL_fnctset_reg_hist_fnct ( ADCL_neighborhood_hist_functions , ADCL_neighborhood_fnctset );

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

    ADCL_function_create_async ( ADCL_allgatherv_native, NULL,
                                 ADCL_ATTRSET_NULL,
                                 m_allgatherv_attr, "Allgatherv native",
                                 & ADCL_allgatherv_functions[count]);
    count++;

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

void ADCL_neighborhood_read( FILE *fp, ADCL_Data data )
{
    int j;
    int nchar = 80;
    char *line = NULL;
    char *perf = NULL;

    line = (char *)malloc( nchar * sizeof(char) );
    fgets( line, nchar, fp ); /* RECORD Tag */
    /* Network Topology information */
    fgets( line, nchar, fp ); /* NTOPO Tag */
    fgets( line, nchar, fp ); /* NP */
    get_int_data_from_xml ( line, &data->d_np );
    fgets( line, nchar, fp ); /* Close NTOPO Tag */
    /* Logical Topology information */
    fgets( line, nchar, fp ); /* LTOPO Tag */
    fgets( line, nchar, fp ); /* NDIM Tag */
    get_int_data_from_xml ( line, &data->d_tndims );
    fgets( line, nchar, fp ); /* PERIOD Tag */
    data->d_tperiods = (int *)malloc( data->d_tndims*sizeof(int) );
    for ( j=0; j<data->d_tndims; j++ ) {
        fgets( line, nchar, fp ); /* DIM Tag */
        get_int_data_from_xml ( line, &(data->d_tperiods[j]) );
    }
    fgets( line, nchar, fp ); /* Close PERIOD Tag */
    fgets( line, nchar, fp ); /* Close LTOPO Tag */
    /* Vector information */
    fgets( line, nchar, fp ); /* VECT Tag */
    fgets( line, nchar, fp ); /* NDIM Tag */
    get_int_data_from_xml ( line, &data->d_vndims );
    fgets( line, nchar, fp ); /* DIMS Tag */
    data->d_vdims = (int *)malloc( data->d_vndims*sizeof(int) );
    for ( j=0; j<data->d_vndims; j++ ) {
        fgets( line, nchar, fp ); /* DIM Tag */
	get_int_data_from_xml ( line, &(data->d_vdims[j]) );
    }
    fgets( line, nchar, fp ); /* Close DIMS Tag */
    fgets( line, nchar, fp ); /* NC Tag */
    get_int_data_from_xml ( line, &data->d_nc );
    /* Memory allocation for cnts and displ */
    data->d_rcnts = (int *)calloc (data->d_np, sizeof(int));
    data->d_displ = (int *)calloc (data->d_np, sizeof(int));
    /* Reading the data */
    fgets( line, nchar, fp ); /* Opening MAP Tag */
    fgets( line, nchar, fp ); /* VECTYPE Tag */
    get_int_data_from_xml ( line, &data->d_vectype );
    fgets( line, nchar, fp ); /* HWIDTH Tag */
    get_int_data_from_xml ( line, &data->d_hwidth );
    fgets( line, nchar, fp ); /* Opening CNTS Tag */
    for ( j=0; j<data->d_np; j++ ) {
       fgets( line, nchar, fp ); /* CNT Tag */
       get_int_data_from_xml ( line, &(data->d_rcnts[j]) );
    }
    fgets( line, nchar, fp ); /* CLOSE CNTS Tag */
    fgets( line, nchar, fp ); /* Opening DISPL Tag */
    for ( j=0; j<data->d_np; j++ ) {
	fgets( line, nchar, fp ); /* DISPL Tag */
        get_int_data_from_xml ( line, &(data->d_displ[j]) );
    }
    fgets( line, nchar, fp ); /* Close DISPL Tag */
    fgets( line, nchar, fp ); /* OP Tag */
    get_int_data_from_xml ( line, (int *)&(data->d_op) );
    fgets( line, nchar, fp ); /* INPLACE Tag */
    get_int_data_from_xml ( line, &(data->d_inplace) );
    fgets( line, nchar, fp ); /* Close MAP Tag */
    fgets( line, nchar, fp ); /* Close VECT Tag */
    /* Attribute information */
    fgets( line, nchar, fp ); /* ATTR Tag */
    fgets( line, nchar, fp ); /* NUM Tag */
    get_int_data_from_xml ( line, &data->d_asmaxnum );
    fgets( line, nchar, fp ); /* ATTRVALS Tag */
    data->d_attrvals = (int *)malloc( data->d_asmaxnum*sizeof(int) );
    for ( j=0; j<data->d_asmaxnum; j++ ) {
       fgets( line, nchar, fp ); /* VAL Tag */
       get_int_data_from_xml ( line, &(data->d_attrvals[j]) );
    }
    fgets( line, nchar, fp ); /* Close ATTRVALS Tag */
    fgets( line, nchar, fp ); /* Close ATTR Tag */
    /* Function set and winner function */
    fgets( line, nchar, fp ); /* FUNC Tag */
    fgets( line, nchar, fp ); /* FNCTSET Tag */
    get_str_data_from_xml ( line, &data->d_fsname );
    fgets( line, nchar, fp ); /* WINNER  Tag */
    get_str_data_from_xml ( line, &data->d_wfname );
    fgets( line, nchar, fp ); /* FNCTNUM Tag */
    get_int_data_from_xml ( line, &data->d_fsnum );
    fgets( line, nchar, fp ); /* Close FUNC Tag */
    fgets( line, nchar, fp ); /* PERFS Tag */
    data->d_perf = (double *)malloc(data->d_fsnum*sizeof(double));
    for ( j=0; j<data->d_fsnum; j++ ) {
        fgets( line, nchar, fp ); /* PERF Tag */
        get_str_data_from_xml ( line, &perf );
        data->d_perf[j] = atof(perf);
    }
    fgets( line, nchar, fp ); /* Close PERFS Tag */
    fgets( line, nchar, fp ); /* Close RECORD Tag */

    return;
}

static void ADCL_neighborhood_write( FILE *fp, ADCL_Data data )
{
    int i, j, tndims, vndims ;
    int nchar = 80, nch;
    char *line = NULL;

    fprintf ( fp, "  <RECORD>\n" );
    /* Network Topology information */
    fprintf ( fp, "    <NTOPO>\n" );
    /* So far we have only np, later on this part might be extended significantly */
    fprintf ( fp, "      <NP>%d</NP>\n", data->d_np );
    fprintf ( fp, "    </NTOPO>\n" );
    /* Logical Topology information */
    fprintf ( fp, "    <LTOPO>\n" );
    tndims = data->d_tndims;
    fprintf ( fp, "      <NDIM>%d</NDIM>\n", tndims );
    fprintf ( fp, "      <PERIOD>\n");
    for ( j=0; j<tndims; j++) {
        fprintf ( fp, "        <DIM>%d</DIM>\n", data->d_tperiods[j] );
    }
    fprintf ( fp, "      </PERIOD>\n");
    fprintf ( fp, "    </LTOPO>\n" );
    /* Vector information */
    fprintf ( fp, "    <VECT>\n" );
    vndims = data->d_vndims;
    fprintf ( fp, "      <NDIM>%d</NDIM>\n", vndims );
    fprintf ( fp, "      <DIMS>\n");            
    for ( j=0; j<vndims; j++) {
        fprintf ( fp, "        <DIM>%d</DIM>\n", data->d_vdims[j] );
    }
    fprintf ( fp, "      </DIMS>\n");
    fprintf ( fp, "      <NC>%d</NC>\n", data->d_nc );
    /* Vector map information */
    fprintf ( fp, "      <MAP>\n");
    fprintf ( fp, "        <VECTYPE>%d</VECTYPE>\n", data->d_vectype );
    fprintf ( fp, "        <HWIDTH>%d</HWIDTH>\n", data->d_hwidth );
    fprintf ( fp, "        <CNTS>\n" );
    for ( j=0; j<data->d_np; j++ ) {
        fprintf ( fp, "          <CNT>%d</CNT>\n", data->d_rcnts[j] );
    }
    fprintf ( fp, "        </CNTS>\n" );
    fprintf ( fp, "        <DISPLS>\n" );
    for ( j=0; j<data->d_np; j++ ) {
        fprintf ( fp, "          <DISPL>%d</DISPL>\n", data->d_displ[j] );
    }
    fprintf ( fp, "        </DISPLS>\n" );
    fprintf ( fp, "        <OP>%d</OP>\n", data->d_op );
    fprintf ( fp, "        <INPLACE>%d</INPLACE>\n", data->d_inplace );
    fprintf ( fp, "      </MAP>\n");
    fprintf ( fp, "    </VECT>\n" );
    /* Attribute information */
    fprintf ( fp, "    <ATTR>\n" );
    fprintf ( fp, "      <NUM>%d</NUM>\n", data->d_asmaxnum );
    fprintf ( fp, "      <ATTRVALS>\n" );
    for ( j=0; j<data->d_asmaxnum; j++) {
        fprintf ( fp, "        <VAL>%d</VAL>\n", data->d_attrvals[j] );
    }
    fprintf ( fp, "      </ATTRVALS>\n" );
    fprintf ( fp, "    </ATTR>\n" );
    /* Function set and winner function */
    fprintf ( fp, "    <FUNC>\n" );
    fprintf ( fp, "      <FNCTSET>%s</FNCTSET>\n", data->d_fsname );
    fprintf ( fp, "      <WINNER>%s</WINNER>\n", data->d_wfname );
    fprintf ( fp, "      <FNCTNUM>%d</FNCTNUM>\n", data->d_fsnum );
    fprintf ( fp, "    </FUNC>\n" );
    /* Performance data */
    fprintf ( fp, "    <PERFS>\n" );
    for ( j=0; j<data->d_fsnum; j++) {   
       fprintf ( fp, "        <PERF>%.4f</PERF>\n", data->d_perf[j] );
    }
    fprintf ( fp, "    </PERFS>\n" );
    fprintf ( fp, "  </RECORD>\n" );

    return;
}

static int ADCL_neighborhood_filter(ADCL_Data data, void *filter_criteria )
{
    int retval;
    ADCL_neighborhood_criteria_t *criteria;
    /* Initialization */
    retval = 0;
    criteria= (ADCL_neighborhood_criteria_t *)filter_criteria;
    if ( NULL == criteria ) {
        return 1;
    }
    if( (0 == strcmp( criteria->c_fsname, data->d_fsname)) &&
        (criteria->c_tndims == data->d_tndims) ){
	retval = 1;
    }
    return retval;
}

static double ADCL_neighborhood_distance(ADCL_Data data1 , ADCL_Data data2 )
{
    double distance = 0;
    int i; 
    /* Euclidian distance is used here */
    for(i=0; i<data1->d_vndims; i++) {
	distance += pow( (data1->d_vdims[i] - data2->d_vdims[i]) , 2);
    }
    return sqrt(distance);
}

void ADCL_neighborhood_set_criteria( void *filter_criteria, ADCL_request_t *req )
{
    ADCL_neighborhood_criteria_t *criteria;
    criteria= (ADCL_neighborhood_criteria_t *)filter_criteria;

    /* Get the function set name */
    ADCL_Request_get_fsname( req, &(criteria->c_fsname) );
    /* Get the topology dimensions */
    ADCL_Request_get_tndims( req, &(criteria->c_tndims) );
    /* Other criteria may follow */

    return;
}
