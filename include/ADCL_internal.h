#ifndef __ADCL_INTERNAL_H__
#define __ADCL_INTERNAL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#include "ADCL.h"
#include "ADCL_array.h"
#include "ADCL_vector.h"
#include "ADCL_topology.h"
#include "ADCL_attribute.h"
#include "ADCL_emethod.h"
#include "ADCL_fnctset.h"
#include "ADCL_memory.h"
#include "ADCL_subarray.h"
#include "ADCL_packunpack.h"
#include "ADCL_request.h"
#include "ADCL_change.h"

#include "ADCL_config.h"
#include "ADCL_sysconfig.h"


#define TRUE   1
#define FALSE  0

#define ADCL_STATE_REGULAR  100
#define ADCL_STATE_TESTING  101
#define ADCL_STATE_DECISION 102

#define ADCL_EVAL_DONE  110

#define ADCL_STATISTIC_VOTE 0
#define ADCL_STATISTIC_MAX  1


/* Some prototypes of functions which do not deserve their own header file */
int ADCL_printf_init     ( void );
int ADCL_printf_finalize ( void );
int ADCL_printf          ( const char *format, ...);

int ADCL_readenv( void );
int ADCL_statistics_filter_timings  ( ADCL_emethod_t **emethods, int count, 
				      int rank );
int ADCL_statistics_global_max ( ADCL_emethod_t **emethods, int count,
 				 MPI_Comm comm, int num_blocks, int *blength, 
				 int *winners, int rank );

int ADCL_hypothesis_shrinklist_byattr ( ADCL_emethod_req_t *ermethod, 
                                        int attribute, int required_value );
int ADCL_hypothesis_set     ( ADCL_emethod_req_t *er, int attr, int attrval );
int ADCL_hypothesis_eval_v2 ( ADCL_emethod_req_t *er );


#endif /* __ADCL_INTERNAL_H__ */
