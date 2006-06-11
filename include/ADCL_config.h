#ifndef __ADCL_CONFIG_H__
#define __ADCL_CONFIG_H__

/* Choose type of non-contiguous message handling */
#define DERIVED_TYPES 1
#define PACK_UNPACK   1

/* Choose different communication pattern */
#define CHANGE_AAO    1
#define CHANGE_PAIR   1

/* Choose data transfer primitive */
#define ISEND_IRECV   1
#define SEND_IRECV    1
#define SEND_RECV     1


#endif