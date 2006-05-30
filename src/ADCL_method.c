#include "ADCL.h"


static int ADCL_singleblock_total_num=0;
static int ADCL_dualblock_total_num=0;

static ADCL_method_t* ADCL_singleblock_array=NULL;
static ADCL_method_t* ADCL_dualblock_array=NULL;


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

    ADCL_singleblock_total_num = 3;
    ADCL_singleblock_array=(ADCL_method_t*)calloc(1, ADCL_singleblock_total_num
						   * sizeof( ADCL_method_t));
    if ( NULL == ADCL_singleblock_array ) {
	return ADCL_NO_MEMORY;
    }
    ADCL_singleblock_array[0].m_ifunc = (ADCL_fnct_ptr*)singleblock_func0;
    ADCL_singleblock_array[1].m_ifunc = (ADCL_fnct_ptr*)singleblock_func1;
    ADCL_singleblock_array[2].m_ifunc = (ADCL_fnct_ptr*)singleblock_func2;

    
    ADCL_dualblock_total_num=2;
    ADCL_dualblock_array=(ADCL_method_t*)calloc (1,ADCL_dualblock_total_num *
						 sizeof( ADCL_method_t));
    if ( NULL == ADCL_dualblock_array ) {
	return ADCL_NO_MEMORY;
    }

    ADCL_dualblock_array[0].m_ifunc = (ADCL_fnct_ptr*)dualblock_func0_init;
    ADCL_dualblock_array[0].m_wfunc = (ADCL_fnct_ptr*)dualblock_func0_wait;

    ADCL_dualblock_array[1].m_ifunc = (ADCL_fnct_ptr*)dualblock_func1_init;
    ADCL_dualblock_array[1].m_wfunc = (ADCL_fnct_ptr*)dualblock_func1_wait;
       
    return ADCL_SUCCESS;
}
int ADCL_method_finalize (void)
{
    if ( NULL != ADCL_singleblock_array  ) {
	free ( ADCL_singleblock_array );
    }

    if ( NULL != ADCL_dualblock_array  ) {
	free ( ADCL_dualblock_array );
    }

    return ADCL_SUCCESS;
}

int ADCL_method_get_num_attr ( ADCL_method_t *meth)
{
    return meth->m_nattr;
}

char* ADCL_method_get_attr_name ( ADCL_method_t *meth, int pos )
{
    char *attr;

    if ( pos >= meth->m_nattr ) {
	return NULL;
    }
    attr = strdup((const char *)(meth->m_attr[pos]) );
    return attr;
}

char* ADCL_method_get_attr_val (ADCL_method_t *meth, int i)
{
    char *val;

    if ( i >= meth->m_nattr ) {
	return NULL;
    }
    val = strdup(meth->m_val[i]);
    return val;
}

int ADCL_get_num_singleblock_methods (void)
{
    return ADCL_singleblock_total_num;
}

int ADCL_get_num_dualblock_methods (void)
{
    return ADCL_dualblock_total_num;
}

ADCL_method_t* ADCL_get_singleblock_method ( int i )
{
    if ( i>=ADCL_singleblock_total_num || i<0 ) {
	return NULL;
    }

    return (&(ADCL_singleblock_array[i]));
}

ADCL_method_t* ADCL_get_dualblock_method ( int i )
{
    if ( i>=ADCL_dualblock_total_num || i<0 ) {
	return NULL;
    }

    return (&(ADCL_dualblock_array[i]));
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
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
