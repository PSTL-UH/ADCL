       include 'mpif.h'
       
       integer ADCL_SUCCESS, ADCL_NO_MEMORY, ADCL_ERROR_INTERNAL
       integer ADCL_USER_ERROR, ADCL_UNDEFINED, ADCL_NOT_FOUND

       parameter ( ADCL_SUCCESS        = 0 )
       parameter ( ADCL_NO_MEMORY      = -1)
       parameter ( ADCL_ERROR_INTERNAL = -2)
       parameter ( ADCL_USER_ERROR     = -3)
       parameter ( ADCL_UNDEFINED      = -4)
       parameter ( ADCL_NOT_FOUND      = -5)

       integer ADCL_INVALID_ARG, ADCL_INVALID_NDIMS
       integer ADCL_INVALID_HWIDTH, ADCL_INVALID_DAT
       integer ADCL_INVALID_DATA, ADCL_INVALID_COMM
       integer ADCL_INVALID_DIMS, ADCL_INVALID_REQUEST
       integer ADCL_INVALID_NC, ADCL_INVALID_TYPE
       integer ADCL_INVALID_TOPOLOGY, ADCL_INVALID_ATTRIBUTE
       integer ADCL_INVALID_ATTRSET, ADCL_INVALID_FUNCTION
       integer ADCL_INVALID_WORK_FUNCTION_PTR, ADCL_INVALID_FNCTSET
       integer ADCL_INVALID_VECTOR, ADCL_INVALID_DIRECTION

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
       parameter ( ADCL_INVALID_TOPOLOGY = 20)
       parameter ( ADCL_INVALID_ATTRIBUTE = 21)
       parameter ( ADCL_INVALID_ATTRSET   = 22)
       parameter ( ADCL_INVALID_FUNCTION  = 23)
       parameter ( ADCL_INVALID_WORK_FUNCTION_PTR = 24)
       parameter ( ADCL_INVALID_FNCTSET    = 25)
       parameter ( ADCL_INVALID_VECTOR     = 26)
       parameter ( ADCL_INVALID_DIRECTION  = 27)
       
       integer ADCL_VECTOR_NULL, ADCL_REQUEST_NULL
       integer ADCL_TOPOLOGY_NULL, ADCL_ATTRIBUTE_NULL
       integer ADCL_ATTRSET_NULL, ADCL_FUNCTION_NULL
       integer ADCL_FNCTSET_NULL, ADCL_NULL_FNCT_PTR 

       parameter ( ADCL_VECTOR_NULL    = -1)
       parameter ( ADCL_REQUEST_NULL   = -2)
       parameter ( ADCL_TOPOLOGY_NULL  = -3)
       parameter ( ADCL_ATTRIBUTE_NULL = -4)
       parameter ( ADCL_ATTRSET_NULL   = -5)
       parameter ( ADCL_FUNCTION_NULL  = -6)
       parameter ( ADCL_FNCTSET_NULL   = -7)
       parameter ( ADCL_NULL_FNCT_PTR  = -8)

       integer ADCL_MAX_ATTRLEN, ADCL_MAX_NAMELEN
       parameter ( ADCL_MAX_ATTRLEN = 32 ) 
       parameter ( ADCL_MAX_NAMELEN = 32 )

       integer ADCL_DIRECTION_BOTH, ADCL_DIRECTION_LEFT_TO_RIGHT
       integer ADCL_DIRECTION_RIGHT_TO_LEFT
       parameter ( ADCL_DIRECTION_BOTH = 1 )
       parameter ( ADCL_DIRECTION_LEFT_TO_RIGHT = 2 )
       parameter ( ADCL_DIRECTION_RIGHT_TO_LEFT = 3 )

       integer ADCL_FNCTSET_NEIGHBORHOOD, ADCL_FNCTSET_SHIFT_LTOR
       integer ADCL_FNCTSET_SHIFT_RTOL
       parameter ( ADCL_FNCTSET_NEIGHBORHOOD = 0 )
       parameter ( ADCL_FNCTSET_SHIFT_RTOL = 0 )
       parameter ( ADCL_FNCTSET_SHIFT_LTOR = 0 )
