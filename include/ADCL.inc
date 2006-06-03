       include 'mpif.h'
       
       integer ADCL_SUCCESS, ADCL_NO_MEMORY, ADCL_ERROR_INTERNAL
       integer ADCL_USER_ERROR, ADCL_UNDEFINED

       parameter ( ADCL_SUCCESS        = 0 )
       parameter ( ADCL_NO_MEMORY      = -1)
       parameter ( ADCL_ERROR_INTERNAL = -2)
       parameter ( ADCL_USER_ERROR     = -3)
       parameter ( ADCL_UNDEFINED      = -4)

       integer ADCL_INVALID_ARG, ADCL_INVALID_NDIMS
       integer ADCL_INVALID_HWIDTH, ADCL_INVALID_DAT
       integer ADCL_INVALID_DATA, ADCL_INVALID_COMM
       integer ADCL_INVALID_DIMS, ADCL_INVALID_REQUEST
       integer ADCL_INVALID_NC, ADCL_INVALID_TYPE

       parameter ( ADCL_INVALID_ARG     = 10)
       parameter ( ADCL_INVALID_NDIMS   = 11)
       parameter ( ADCL_INVALID_DIMS    = 12)
       parameter ( ADCL_INVALID_HWIDTH  = 13)
       parameter ( ADCL_INVALID_DAT     = 14)
       parameter ( ADCL_INVALID_DATA    = 15)
       parameter ( ADCL_INVALID_COMM    = 16)
       parameter ( ADCL_INVALID_REQUEST = 17)
       parameter ( ADCL_INVALID_NC      = 18)
       parameter ( ADCL_INVALID_TYPE    = 19)

       
       integer ADCL_VECTOR_NULL, ADCL_REQUEST_NULL
       integer ADCL_NULL_FCTN_PTR 

       parameter ( ADCL_VECTOR_NULL    = -1)
       parameter ( ADCL_REQUEST_NULL   = -2)
       parameter ( ADCL_NULL_FCTN_PTR  = -3)

