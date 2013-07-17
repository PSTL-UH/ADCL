#include <math.h>
#include "ADCL_ibcast.h"

const int ADCL_attr_ibcast_alg_linear=1000;
const int ADCL_attr_ibcast_alg_binomial=1001;
const int ADCL_attr_ibcast_alg_chain=1002;
const int ADCL_attr_ibcast_alg_generic=1003;

// Fanout = 0 means undetermined (eg. for binomial alg.)
const int ADCL_attr_ibcast_fanout_0 = 0;
const int ADCL_attr_ibcast_fanout_1 = 1;
const int ADCL_attr_ibcast_fanout_2 = 2;
const int ADCL_attr_ibcast_fanout_3 = 3;
const int ADCL_attr_ibcast_fanout_4 = 4;
const int ADCL_attr_ibcast_fanout_5 = 5;
const int ADCL_attr_ibcast_fanout_n = 6;

const int ADCL_attr_progress_1 = 101;
const int ADCL_attr_progress_2 = 102;
const int ADCL_attr_progress_3 = 103;
const int ADCL_attr_progress_4 = 104;
const int ADCL_attr_progress_5 = 105;
const int ADCL_attr_progress_6 = 106;
const int ADCL_attr_progress_7 = 107;
const int ADCL_attr_progress_8 = 108;
const int ADCL_attr_progress_9 = 109;
const int ADCL_attr_progress_10 = 110;

ADCL_attribute_t *ADCL_ibcast_attrs[ADCL_ATTR_IBCAST_TOTAL_NUM];
ADCL_attrset_t *ADCL_ibcast_attrset;
ADCL_function_t *ADCL_ibcast_functions[ADCL_METHOD_IBCAST_TOTAL_NUM];
ADCL_fnctset_t *ADCL_ibcast_fnctset;

int ADCL_predefined_ibcast( void )
{
  int count=0,i;
    int m_ibcast_attr[ADCL_ATTR_IBCAST_TOTAL_NUM];
    int ADCL_attr_ibcast_alg[ADCL_ATTR_IBCAST_ALG_MAX];
    int ADCL_attr_ibcast_fanout[ADCL_ATTR_IBCAST_FANOUT_MAX];
    int ADCL_attr_progress[ADCL_ATTR_PROGRESS_MAX];

    char * ADCL_attr_ibcast_alg_names[ADCL_ATTR_IBCAST_ALG_MAX] = { "Ibcast_linear","Ibcast_binomial","Ibcast_chain","Ibcast_generic"}; 

    char * ADCL_attr_ibcast_fanout_names[ADCL_ATTR_IBCAST_FANOUT_MAX] = { "0","1","2","3","4","5","n" };

    char * ADCL_attr_progress_names[ADCL_ATTR_PROGRESS_MAX] = { "1","2","3","4","5","6","7","8","9","10"};

    ADCL_attr_ibcast_alg[0] = ADCL_attr_ibcast_alg_linear;
    ADCL_attr_ibcast_alg[1] = ADCL_attr_ibcast_alg_binomial;
    ADCL_attr_ibcast_alg[2] = ADCL_attr_ibcast_alg_chain;
    ADCL_attr_ibcast_alg[3] = ADCL_attr_ibcast_alg_generic;

    ADCL_attr_ibcast_fanout[0] = ADCL_attr_ibcast_fanout_0;
    ADCL_attr_ibcast_fanout[1] = ADCL_attr_ibcast_fanout_1;
    ADCL_attr_ibcast_fanout[2] = ADCL_attr_ibcast_fanout_2;
    ADCL_attr_ibcast_fanout[3] = ADCL_attr_ibcast_fanout_3;
    ADCL_attr_ibcast_fanout[4] = ADCL_attr_ibcast_fanout_4;
    ADCL_attr_ibcast_fanout[5] = ADCL_attr_ibcast_fanout_5;
    ADCL_attr_ibcast_fanout[6] = ADCL_attr_ibcast_fanout_n;

    ADCL_attr_progress[0] = ADCL_attr_progress_1;
    ADCL_attr_progress[1] = ADCL_attr_progress_2;
    ADCL_attr_progress[2] = ADCL_attr_progress_3;
    ADCL_attr_progress[3] = ADCL_attr_progress_4;
    ADCL_attr_progress[4] = ADCL_attr_progress_5;
    ADCL_attr_progress[5] = ADCL_attr_progress_6;
    ADCL_attr_progress[6] = ADCL_attr_progress_7;
    ADCL_attr_progress[7] = ADCL_attr_progress_8;
    ADCL_attr_progress[8] = ADCL_attr_progress_9;
    ADCL_attr_progress[9] = ADCL_attr_progress_10;

    ADCL_attribute_create ( ADCL_ATTR_IBCAST_ALG_MAX, ADCL_attr_ibcast_alg,
                            ADCL_attr_ibcast_alg_names, "ibcast algorithms",
                            &ADCL_ibcast_attrs[0] );
    ADCL_attribute_create ( ADCL_ATTR_IBCAST_FANOUT_MAX, ADCL_attr_ibcast_fanout,
                            ADCL_attr_ibcast_fanout_names, "ibcast fanout values",
                            &ADCL_ibcast_attrs[1] );
    ADCL_attribute_create ( ADCL_ATTR_PROGRESS_MAX, ADCL_attr_progress,
                            ADCL_attr_progress_names, "ibcast progress cycle",
                            &ADCL_ibcast_attrs[2] );

    ADCL_attrset_create ( ADCL_ATTR_IBCAST_TOTAL_NUM, ADCL_ibcast_attrs, &ADCL_ibcast_attrset);

/* ******************************************************************** */
/* IBCAST - Fortran function set 3                                   	*/
/* ******************************************************************** */

    count = 0;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_linear;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_n;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async( ADCL_ibcast_linear, ADCL_ibcast_wait,
				  ADCL_ibcast_attrset,
				  m_ibcast_attr, "Ibcast_linear",
				  & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_binomial;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_0;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_binomial, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_binomial",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;
	
    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_chain;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_1;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_chain, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_chain",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_generic;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_1;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_generic, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_generic_1",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_generic;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_2;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_generic, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_generic_2",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_generic;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_3;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_generic, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_generic_3",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_generic;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_4;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_generic, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_generic_4",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;

    m_ibcast_attr[0] = ADCL_attr_ibcast_alg_generic;
    m_ibcast_attr[1] = ADCL_attr_ibcast_fanout_5;

    for(i=0;i<ADCL_ATTR_PROGRESS_MAX;i++){
      m_ibcast_attr[2] = ADCL_attr_progress[i];

      ADCL_function_create_async ( ADCL_ibcast_generic, ADCL_ibcast_wait,
				   ADCL_ibcast_attrset,
				   m_ibcast_attr, "Ibcast_generic_5",
				   & ADCL_ibcast_functions[count+i]);
    }

    count+= ADCL_ATTR_PROGRESS_MAX;


    ADCL_fnctset_create ( ADCL_METHOD_IBCAST_TOTAL_NUM,
			  ADCL_ibcast_functions,
			  "Ibcast",
			  &ADCL_ibcast_fnctset );


    if ( count != ADCL_METHOD_IBCAST_TOTAL_NUM) {
      ADCL_printf("Total Number wrong\n");
      return ADCL_ERROR_INTERNAL;
    }

    return ADCL_SUCCESS;
}
