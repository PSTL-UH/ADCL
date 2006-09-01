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
#include "ADCL_request.h"
#include "ADCL_method.h"
#include "ADCL_emethod.h"
#include "ADCL_memory.h"
#include "ADCL_subarray.h"
#include "ADCL_packunpack.h"
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

int ADCL_printf_init     ( void );
int ADCL_printf_finalize ( void );
int ADCL_printf          ( const char *format, ...);

int ADCL_readenv( void );
int ADCL_statistics_filter_timings  ( ADCL_emethod_t *emethods, int count, 
				      MPI_Comm comm );
int ADCL_statistics_determine_votes ( ADCL_emethod_t *emethods, int count, 
				      MPI_Comm comm );
int ADCL_statistics_global_max      ( ADCL_emethod_t *emethods, int count, 
				      MPI_Comm comm );

int ADCL_hypothesis_shrinklist_byattr ( ADCL_emethod_req_t *ermethods, int attribute,
					int required_value );


#endif /* __ADCL_INTERNAL_H__ */
