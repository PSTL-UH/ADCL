#include "ADCL_internal.h"
#include "ADCL_config.h"

static int ADCL_local_id_counter=0;
int ADCL_method_total_num=0;
ADCL_method_t* ADCL_method_array=NULL;

extern int ADCL_emethod_use_perfhypothesis;
int *ADCL_method_perfhyp_list=NULL;

int ADCL_attr_max[ADCL_ATTR_TOTAL_NUM]={ADCL_ATTR_MAPPING_MAX,ADCL_ATTR_NONCONT_MAX,ADCL_ATTR_TRANSFER_MAX};

/* ADCL_attr_max[ADCL_ATTR_NUMBLOCKS]=ADCL_ATTR_NUMBLOCKS_MAX; */

const int ADCL_attr_mapping_aao=100;
const int ADCL_attr_mapping_pair=101;
const int ADCL_attr_mapping_hierarch=102;
int ADCL_attr_mapping[ADCL_ATTR_MAPPING_MAX]={ADCL_attr_mapping_aoo, 
					      ADCL_attr_mapping_pair};

const int ADCL_attr_noncont_ddt=110;
const int ADCL_attr_noncont_pack=111;
const int ADCL_attr_noncont_individual=112;
int ADCL_attr_noncont[ADCL_ATTR_NONCONT_MAX]={ADCL_attr_noncont_ddt, 
					      ADCL_attr_noncont_pack};

const int ADCL_attr_transfer_IsendIrecv=120;
const int ADCL_attr_transfer_SendIrecv=121;
const int ADCL_attr_transfer_SendRecv=122;
const int ADCL_attr_transfer_Sendrecv=123;
#ifdef MPI_WIN
const int ADCL_attr_transfer_FenceGet=124;
const int ADCL_attr_transfer_FencePut=125;
const int ADCL_attr_transfer_StartPostGet=126;
const int ADCL_attr_transfer_StartPostPut=127;
int ADCL_attr_transfer[ADCL_ATTR_TRANSFER_MAX]={ADCL_attr_transfer_IsendIrecv, 
						ADCL_attr_transfer_SendIrecv,
						ADCL_attr_transfer_SendRecv,
						ADCL_attr_transfer_Sendrecv,
						ADCL_attr_transfer_FenceGet,
						ADCL_attr_transfer_FencePut,
						ADCL_attr_transfer_StartPostGet,
						ADCL_attr_transfer_StartPostPut};
#else
int ADCL_attr_transfer[ADCL_ATTR_TRANSFER_MAX]={ADCL_attr_transfer_IsendIrecv, 
						ADCL_attr_transfer_SendIrecv,
						ADCL_attr_transfer_SendRecv,
						ADCL_attr_transfer_Sendrecv};
#endif

const int ADCL_attr_numblocks_single=130;
const int ADCL_attr_numblocks_dual=131;



#ifndef ADCL_NO_COMM_DEBUG
int ADCL_method_init ( void )
{
    int count=0;
    
#ifdef MPI_WIN
    ADCL_method_total_num = 20; 
#else
    ADCL_method_total_num = 12;
#endif

    /*Array of method_t */
    ADCL_method_array=(ADCL_method_t*)calloc(1, ADCL_method_total_num
					     * sizeof( ADCL_method_t));
    if ( NULL == ADCL_method_array ) {
	return ADCL_NO_MEMORY;
    }

    /* aao, ddt, IsendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;

    /*function pointer to ADCL_change_sb_aao_IsendIrecv */
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_IsendIrecv;
    strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_aao", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 

    /* aao, ddt, SendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_SendIrecv;
    strncpy ( ADCL_method_array[count].m_name, "SendIrecv_aao", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 

    /* pair, ddt, IsendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_IsendIrecv;
    strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_pair", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 

    /* pair, ddt, SendRecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_SendRecv;
    strncpy ( ADCL_method_array[count].m_name, "SendRecv_pair", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendRecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 

    /* pair, ddt, SendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_SendIrecv;
    strncpy ( ADCL_method_array[count].m_name, "SendIrecv_pair", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 

    /* pair, ddt, Sendrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_Sendrecv;
    strncpy ( ADCL_method_array[count].m_name, "Sendrecv_pair", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_Sendrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;

    /* aao, pack, IsendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_IsendIrecv_pack;
    strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_aao_pack", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 
    
    /* aao, pack, SendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_SendIrecv_pack;
    strncpy( ADCL_method_array[count].m_name, "SendIrecv_aao_pack", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;

    /* pair, pack, IsendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_IsendIrecv_pack;
    strncpy ( ADCL_method_array[count].m_name, "IsendIrecv_pair_pack", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_IsendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++; 
    
    /* pair, pack, SendRecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_SendRecv_pack;
    strncpy( ADCL_method_array[count].m_name, "SendRecv_pair_pack", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendRecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count ++;
    
    /* pair, pack, SendIrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_SendIrecv_pack;
    strncpy( ADCL_method_array[count].m_name, "SendIrecv_pair_pack", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_SendIrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;

    /* pair, pack, Sendrecv */
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_Sendrecv_pack;
    strncpy( ADCL_method_array[count].m_name, "Sendrecv_pair_pack", 32 );
    
    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_pack;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_Sendrecv;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
      count++;

#ifdef MPI_WIN
    
#ifdef FENCE_PUT /* Comm 9*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_win_fence_put;
    strncpy ( ADCL_method_array[count].m_name, "WinFencePut_aao", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_FencePut;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;
#endif /* WINFENCEPUT */
 

#ifdef FENCE_GET /* Comm 10*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_win_fence_get;
    strncpy ( ADCL_method_array[count].m_name, "WinFenceGet_aao", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_FenceGet;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;
#endif /* WINFENCEGET */
    
#ifdef POSTSTART_PUT /* Comm 11*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_post_start_put;
    strncpy ( ADCL_method_array[count].m_name, "PostStartPut_aao", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartPut;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;
#endif /* POSTSTARTPUT */
                  
#ifdef POSTSTART_GET /* Comm 12*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_aao_post_start_get;
    strncpy ( ADCL_method_array[count].m_name, "PostStartGet_aao", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_aao;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartGet;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */

    count++;
#endif /* POSTSTARTGET */
    
#  ifdef FENCE_PUT /* Comm 9*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_win_fence_put;
    strncpy ( ADCL_method_array[count].m_name, "WinFencePut_pair", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_FencePut;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;
#  endif /* WINFENCEPUT */

#  ifdef FENCE_GET /* Comm 10*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_win_fence_get;
    strncpy ( ADCL_method_array[count].m_name, "WinFenceGet_pair", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_FenceGet;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */
    count++;
#  endif /* WINFENCEGET */

    
#  ifdef POSTSTART_PUT /* Comm 11*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_post_start_put;
    strncpy ( ADCL_method_array[count].m_name, "PostStartPut_pair", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartPut;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */

    count++;
#  endif /* POSTSTARTPUT */

#  ifdef POSTSTART_GET /* Comm 12*/
    ADCL_method_array[count].m_id = ADCL_local_id_counter++;
    ADCL_method_array[count].m_ifunc = (ADCL_fnct_ptr*) ADCL_change_sb_pair_post_start_get;
    strncpy ( ADCL_method_array[count].m_name, "PostStartGet_pair", 32 );

    ADCL_method_array[count].m_attr[ADCL_ATTR_MAPPING]   = ADCL_attr_mapping_pair;
    ADCL_method_array[count].m_attr[ADCL_ATTR_NONCONT]   = ADCL_attr_noncont_ddt;
    ADCL_method_array[count].m_attr[ADCL_ATTR_TRANSFER]  = ADCL_attr_transfer_PostStartGet;
/*    ADCL_method_array[count].m_attr[ADCL_ATTR_NUMBLOCKS] = ADCL_attr_numblocks_single; */

    count++;
#  endif /* POSTSTARTGET */
#endif /* MPI_WIN */


      if( count != ADCL_method_total_num){    
	  ADCL_printf("Total Number wrong\n");
	  return ADCL_ERROR_INTERNAL; 
      }


      ADCL_method_perfhyp_list=malloc(ADCL_method_total_num*sizeof(int));
      if ( NULL == ADCL_method_perfhyp_list ) {
	  return ADCL_ERROR_INTERNAL;
      }

      ADCL_method_perfhyp_list[0]=0;
      ADCL_method_perfhyp_list[1]=2;
      ADCL_method_perfhyp_list[2]=6;
      ADCL_method_perfhyp_list[3]=8;
      ADCL_method_perfhyp_list[4]=1;
      ADCL_method_perfhyp_list[5]=4;
      ADCL_method_perfhyp_list[6]=7;
      ADCL_method_perfhyp_list[7]=10;
      ADCL_method_perfhyp_list[8]=3;
      ADCL_method_perfhyp_list[9]=5;
      ADCL_method_perfhyp_list[10]=9;
      ADCL_method_perfhyp_list[11]=11;

#ifdef MPI_WIN
      ADCL_method_perfhyp_list[12]=12;
      ADCL_method_perfhyp_list[13]=13;
      ADCL_method_perfhyp_list[14]=14;
      ADCL_method_perfhyp_list[15]=15;
      ADCL_method_perfhyp_list[16]=16;
      ADCL_method_perfhyp_list[17]=17;
      ADCL_method_perfhyp_list[18]=18;
      ADCL_method_perfhyp_list[19]=19;
#endif
      return ADCL_SUCCESS;
}
#endif

int ADCL_method_finalize (void)
{
    if ( NULL != ADCL_method_array  ) {
	free ( ADCL_method_array );
    }

    if ( NULL != ADCL_method_perfhyp_list ) {
	free ( ADCL_method_perfhyp_list );
    }

    return ADCL_SUCCESS;
}


int ADCL_get_num_methods (void)
{
    return ADCL_method_total_num;
}


ADCL_method_t* ADCL_get_method ( int i )
{
    if ( i>=ADCL_method_total_num || i<0 ) {
	return NULL;
    }

    if ( ADCL_emethod_use_perfhypothesis ) {
	return (&(ADCL_method_array[ADCL_method_perfhyp_list[i]]));
    }

    return (&(ADCL_method_array[i]));
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
#ifdef ADCL_NO_COMM_DEBUG

int singleblock_func0 ( void *req );
int singleblock_func1 ( void *req );
int singleblock_func2 ( void *req );

int dualblock_func0_init ( void *req );
int dualblock_func0_wait ( void *req );

int dualblock_func1_init ( void *req );
int dualblock_func1_wait ( void *req );

int ADCL_method_init ( void )
{
    /* Three steps: 
       1. generate a list of all available single-block methods
       2. determine the characteristics of the current environment
       3. generate a new, shorter list of single-block methods available

       4. generate a list of all available dual-block methods
       5. generate a new, shorter list of dual-block methods
    */

    /* right now we are just setting some values for code
       development and verification purposes. No attributes 
       set right now! 
    */


    ADCL_method_total_num = 5;
    ADCL_method_array=(ADCL_method_t*)calloc(1, ADCL_method_total_num * sizeof( ADCL_method_t));
    if ( NULL == ADCL_method_array ) {
	return ADCL_NO_MEMORY;
    }

    ADCL_method_array[0].m_ifunc = (ADCL_fnct_ptr*)singleblock_func0;
    ADCL_method_array[1].m_ifunc = (ADCL_fnct_ptr*)singleblock_func1;
    ADCL_method_array[2].m_ifunc = (ADCL_fnct_ptr*)singleblock_func2;

    ADCL_method_array[3].m_ifunc = (ADCL_fnct_ptr*)dualblock_func0_init;
    ADCL_method_array[3].m_wfunc = (ADCL_fnct_ptr*)dualblock_func0_wait;
    ADCL_method_array[3].m_db    = 1;

    ADCL_method_array[4].m_ifunc = (ADCL_fnct_ptr*)dualblock_func1_init;
    ADCL_method_array[4].m_wfunc = (ADCL_fnct_ptr*)dualblock_func1_wait;
    ADCL_method_array[4].m_db    = 1;

    return ADCL_SUCCESS;
}

int singleblock_func0 ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in singleblock_func0. Sleeping for 0.2 seconds\n", rank);
    usleep (200);
    return 0;
}

int singleblock_func1 ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in singleblock_func1. Sleeping for 0.4 seconds\n", rank);
    usleep (400);
    return 0;
}

int singleblock_func2 ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in singleblock_func2. Sleeping for 0.1 seconds\n", rank);
    usleep (100);
    return 0;
}

int dualblock_func0_init ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func0_init. Not Sleeping.\n", rank);
    return 0;
}

int dualblock_func1_init ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func1_init. Not sleeping.\n", rank);
    return 0;
}

int dualblock_func0_wait ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func0_wait. Sleeping for 0.1 seconds\n", rank);
    usleep (100);
    return 0;
}

int dualblock_func1_wait ( void *req ) 
{
    int rank;
    ADCL_request_t *preq = (ADCL_request_t *)req;

    MPI_Comm_rank ( preq->r_comm, &rank );
    printf("%d: in dualblock_func1_wait. Sleeping for 0.1 seconds\n", rank);
    usleep (100);
    return 0;
}
#endif
