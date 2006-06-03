       include 'mpif.inc'
       
       integer ADCL_SUCCESS, ADCL_NO_MEMORY, ADCL_ERROR_INTERNAL
       integer ADCL_USER_ERROR ADCL_UNDEFINED

       paramter ( ADCL_SUCCESS        = 0 )
       paramter ( ADCL_NO_MEMORY      = -1)
       paramter ( ADCL_ERROR_INTERNAL = -2)
       paramter ( ADCL_USER_ERROR     = -3)
       paramter ( ADCL_UNDEFINED      = -4)

       integer ADCL_INVALID_ARG, ADCL_INVALID_NDIMS
       integer ADCL_INVALID_HWIDTH, ADCL_INVALID_DAT
       integer ADCL_INVALID_DATA, ADCL_INVALID_COMM
       integer ADCL_INVALID_DIMS, ADCL_INVALID_REQUEST
       integer ADCL_INVALID_NC, ADCL_INVALID_TYPE

       paramter ( ADCL_INVALID_ARG     = 10)
       paramter ( ADCL_INVALID_NDIMS   = 11)
       paramter ( ADCL_INVALID_DIMS    = 12)
       paramter ( ADCL_INVALID_HWIDTH  = 13)
       paramter ( ADCL_INVALID_DAT     = 14)
       paramter ( ADCL_INVALID_DATA    = 15)
       paramter ( ADCL_INVALID_COMM    = 16)
       paramter ( ADCL_INVALID_REQUEST = 17)
       paramter ( ADCL_INVALID_NC      = 18)
       paramter ( ADCL_INVALID_TYPE    = 19)

       
       integer ADCL_VECTOR_NULL, ADCL_REQUEST_NULL
       integer ADCL_NULL_FCTN_PTR 

       paramter ( ADCL_VECTOR_NULL    = -1)
       paramter ( ADCL_REQUEST_NULL   = -2)
       paramter ( ADCL_NULL_FCTN_PTR  = -3)

