#ifndef __ADCL_H__
#define __ADLC_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#include "mpi.h"

#include "ADCL_vector.h"
#include "ADCL_request.h"
#include "ADCL_method.h"
#include "ADCL_emethod.h"
#include "ADCL_memory.h"
#include "ADCL_subarray.h"
#include "ADCL_packunpack.h"


#define TRUE   1
#define FALSE  0

/* define error codes */
#define ADCL_SUCCESS        0
#define ADCL_NO_MEMORY      -1
#define ADCL_ERROR_INTERNAL -2
#define ADCL_USER_ERROR     -3
#define ADCL_UNDEFINED      -4

#define ADCL_INVALID_ARG     10
#define ADCL_INVALID_NDIMS   11
#define ADCL_INVALID_DIMS    12
#define ADCL_INVALID_HWIDTH  13
#define ADCL_INVALID_DAT     14
#define ADCL_INVALID_DATA    15
#define ADCL_INVALID_COMM    16
#define ADCL_INVALID_REQUEST 17
#define ADCL_INVALID_NC      18

#define ADCL_STATE_REGULAR  100
#define ADCL_STATE_TESTING  101
#define ADCL_STATE_DECISION 102

#define ADCL_EVAL_DONE  110

/* Prototypes of the User level interface functions */
int ADCL_Init (void );
int ADCL_Finalize (void );

int ADCL_printf( const char *format, ...);

#endif /* __ADCL_H__ */
