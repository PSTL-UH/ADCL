#include "ADCL_internal.h"
#include "ADCL_config.h"

/*=================================================================*/
/* List of currently supported attributes 
   Each attribute consists of an attribute ID( given by the define statement),
   a name (for debugging purposes), the possible values (integer) and 
   the maximum number of values supported right now.
*/

/* Total number of attributes */
#define ADCL_ATTR_TOTAL_NUM  3  /* ignoring numblocks for right now */
#define ADCL_ATTR_NOT_SET   -1  /* attributes not set*/
#define ADCL_ATTR_NEW_BLOCK -2  /* signal that we start a new block. 
				   Used in the performance hypothesis v2 */

/* Store for each attribute the maximum number of possible values */
extern int ADCL_attr_max[ADCL_ATTR_TOTAL_NUM];
extern int ADCL_attr_base[ADCL_ATTR_TOTAL_NUM];
/*=================================================================*/
/* 
** The mapping step
** Possible values: aao, pair, hierarch
*/

#define ADCL_ATTR_MAPPING  0
#define ADCL_ATTR_MAPPING_MAX 2 /* Ignore hierarch for now */

const int ADCL_attr_mapping_aao=100;
const int ADCL_attr_mapping_pair=101;
const int ADCL_attr_mapping_hierarch=102;

/*=================================================================*/
/* 
** Methods for non-contiguous data transfer
** Possible values: derived datatypes, pack/unpack, individual
*/
#define ADCL_ATTR_NONCONT  1
#define ADCL_ATTR_NONCONT_MAX 2 /* Ignore individual for now */

const int ADCL_attr_noncont_ddt=110;
const int ADCL_attr_noncont_pack=111;
const int ADCL_attr_noncont_individual=112;

/*=================================================================*/
/* 
** Data transfer primitives
** Possible values: IsendIrecv, SendIrecv, SendRecv, Sendrecv,
**                  FenceGet, FencePut, PostStartGet, PostStartPut
*/
#define ADCL_ATTR_TRANSFER 2

#ifdef MPI_WIN
#define ADCL_ATTR_TRANSFER_MAX 4 
#else
#define ADCL_ATTR_TRANSFER_MAX 8
#endif

const int ADCL_attr_transfer_IsendIrecv=120;
const int ADCL_attr_transfer_SendIrecv=121;
const int ADCL_attr_transfer_SendRecv=122;
const int ADCL_attr_transfer_Sendrecv=123;
#ifdef MPI_WIN
const int ADCL_attr_transfer_FenceGet=124;
const int ADCL_attr_transfer_FencePut=125;
const int ADCL_attr_transfer_StartPostGet=126;
const int ADCL_attr_transfer_StartPostPut=127;
#endif

/*=================================================================*/

/* 
** indicate whether single or dual block method/operation
** (required for the overlap later on)
** Possible values: single, dual
*/
#define ADCL_ATTR_NUMBLOCKS 4
#define ADCL_ATTR_NUMBLOCKS_MAX 2

const int ADCL_attr_numblocks_single=130;
const int ADCL_attr_numblocks_dual=131;

/*=================================================================*/


extern int ADCL_emethod_use_perfhypothesis;

//int ADCL_attr_base[ADCL_ATTR_TOTAL_NUM];
// int ADCL_attr_max[ADCL_ATTR_TOTAL_NUM]={ADCL_ATTR_MAPPING_MAX,ADCL_ATTR_NONCONT_MAX,ADCL_ATTR_TRANSFER_MAX};
/* ADCL_attr_max[ADCL_ATTR_NUMBLOCKS]=ADCL_ATTR_NUMBLOCKS_MAX; */



ADCL_attribute_t *ADCL_neighborhood_attrs[ADCL_ATTR_TOTAL_NUM];
ADCL_attrset_t *ADCL_neighborhood_attrset;
ADCL_fnctset_t *ADCL_neighborhood_fnctset; 


int ADCL_predefined_init ( void )
{
    int count=0;
    int ADCL_attr_mapping[ADCL_ATTR_MAPPING_MAX];
    int ADCL_attr_noncont[ADCL_ATTR_NONCONT_MAX];
    int ADCL_attr_transfer[ADCL_ATTR_TRANSFER_MAX];
    int ADCL_method_total_num=0;
 
    
    ADCL_attr_mapping[0] = ADCL_attr_mapping_aao;
    ADCL_attr_mapping[1] = ADCL_attr_mapping_pair;

    ADCL_attr_noncont[0] = ADCL_attr_noncont_ddt;
    ADCL_attr_noncont[1] = ADCL_attr_noncont_pack;

    ADCL_attr_transfer[0] = ADCL_attr_transfer_IsendIrecv;
    ADCL_attr_transfer[1] = ADCL_attr_transfer_SendIrecv;
    ADCL_attr_transfer[2] = ADCL_attr_transfer_SendRecv;
    ADCL_attr_transfer[3] = ADCL_attr_transfer_Sendrecv;
#ifdef MPI_WIN
    ADCL_attr_transfer[4] = ADCL_attr_transfer_FenceGet;
    ADCL_attr_transfer[5] = ADCL_attr_transfer_FencePut;
    ADCL_attr_transfer[6] = ADCL_attr_transfer_StartPostGet;
    ADCL_attr_transfer[7] = ADCL_attr_transfer_StartPostPut;
#endif


    ADCL_attribute_create ( ADCL_ATTR_MAPPING_MAX, ADCL_attr_mapping, &ADCL_neighborhood_attrs[0]); 
    ADCL_attribute_create ( ADCL_ATTR_NONCONT_MAX, ADCL_attr_noncont, &ADCL_neighborhood_attrs[1]); 
    ADCL_attribute_create ( ADCL_ATTR_TRANSFER_MAX, ADCL_attr_transfer, &ADCL_neighborhood_attrs[2]); 
    
    ADCL_attrset_create ( ADCL_ATTR_TOTAL_NUM, ADCL_neighborhood_attrs, &ADCL_neighborhood_attrset);
    
#ifdef MPI_WIN
    ADCL_method_total_num = 20; 
#else
    ADCL_method_total_num = 12;
#endif
    ADCL_fnctset_create ( ADCL_method_total_num, "Neighborhood communication", 
			  &ADCL_neighborhood_fnctset);

    /* Register function aao, ddt, IsendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_IsendIrecv,
					     ADCL_neighborhood_attrset, 
					     m_attr, "IsendIrecv_aao");
    count++; 

    /* pair, ddt, IsendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_IsendIrecv,
					     ADCL_neighborhood_attrset, 
					     m_attr, "IsendIrecv_pair");
    count++; 

    /* aao, pack, IsendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_IsendIrecv_pack,
					     ADCL_neighborhood_attrset, 
					     m_attr, "IsendIrecv_aao_pack");
    count++; 


    /* pair, pack, IsendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_IsendIrecv_pack,
					     ADCL_neighborhood_attrset, 
					     m_attr, "IsendIrecv_pair_pack");
    count++; 
    

    /* aao, ddt, SendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_SendIrecv,
					     ADCL_neighborhood_attrset, 
					     m_attr, "SendIrecv_aao");
    count++; 

    /* pair, ddt, SendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_SendIrecv,
					     ADCL_neighborhood_attrset, 
					     m_attr, "SendIrecv_pair");
    count++; 

    /* aao, pack, SendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
    /*  m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_SendIrecv_pack,
					     ADCL_neighborhood_attrset, 
					     m_attr, "SendIrecv_aao_pack");
    count++;

    /* pair, pack, SendIrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_SendIrecv_pack,
					     ADCL_neighborhood_attrset, 
					     m_attr, "SendIrecv_pair_pack");
    count++;


    /* pair, ddt, SendRecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendRecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_SendRecv,
					     ADCL_neighborhood_attrset, 
					     m_attr, "SendRecv_pair");
    count++; 


    /* pair, ddt, Sendrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_Sendrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_Sendrecv,
					     ADCL_neighborhood_attrset, 
					     m_attr, "Sendrecv_pair");
    count++;

    
    /* pair, pack, SendRecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendRecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_SendRecv_pack,
					     ADCL_neighborhood_attrset, 
					     m_attr, "SendRecv_pair_pack");
    count ++;
    
    /* pair, pack, Sendrecv */
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_Sendrecv;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_Sendrecv_pack,
					     ADCL_neighborhood_attrset, 
					     m_attr, "Sendrecv_pair_pack");
    count++;

#ifdef MPI_WIN
    
#ifdef FENCE_PUT
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_FencePut;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_win_fence_put,
					     ADCL_neighborhood_attrset, 
					     m_attr, "WinFencePut_aao");
    count++;
#endif /* WINFENCEPUT */
 

#ifdef FENCE_GET 
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_FenceGet;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_win_fence_get,
					     ADCL_neighborhood_attrset, 
					     m_attr, "WinFenceGet_aao");
    count++;
#endif /* WINFENCEGET */
    
#ifdef POSTSTART_PUT
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartPut;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_post_start_put,
					     ADCL_neighborhood_attrset, 
					     m_attr, "PostStartPut_aao");
    count++;
#endif /* POSTSTARTPUT */
                  
#ifdef POSTSTART_GET
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartGet;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_aao_post_start_get,
					     ADCL_neighborhood_attrset, 
					     m_attr, "PostStartGet_aao");
    count++;
#endif /* POSTSTARTGET */
    
#  ifdef FENCE_PUT
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_FencePut;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_win_fence_put,
					     ADCL_neighborhood_attrset, 
					     m_attr, "WinFencePut_pair");
    count++;
#  endif /* WINFENCEPUT */

#  ifdef FENCE_GET
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_FenceGet;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_win_fence_get,
					     ADCL_neighborhood_attrset, 
					     m_attr, "WinFenceGet_pair");
    count++;
#  endif /* WINFENCEGET */

    
#  ifdef POSTSTART_PUT /* Comm 11*/
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartPut;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_post_start_put,
					     ADCL_neighborhood_attrset, 
					     m_attr, "PostStartPut_pair");
    count++;
#  endif /* POSTSTARTPUT */

#  ifdef POSTSTART_GET /* Comm 12*/
    m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartGet;
    /* m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    ADCL_fnctset_register_fnct_and_attrset ( ADCL_neighborhood_fnctset, count, 
					     ADCL_change_sb_pair_post_start_get,
					     ADCL_neighborhood_attrset, 
					     m_attr, "PostStartGet_pair");
    count++;
#  endif /* POSTSTARTGET */
#endif /* MPI_WIN */


      if( count != ADCL_method_total_num){    
	  ADCL_printf("Total Number wrong\n");
	  return ADCL_ERROR_INTERNAL; 
      }

      return ADCL_SUCCESS;
}



