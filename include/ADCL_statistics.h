#ifndef __ADCL_STATISTICS_H__
#define __ADCL_STATISTICS_H__

#include <sys/time.h>
#include "mpi.h"
#include "ADCL_attribute.h"

#define TIME_TYPE double


//#define TIME      MPI_Wtime()
#define TIME ADCL_statistics_time()
#define ADCL_STAT_TIMEDIFF(_tstart,_tend,_exec) {         \
    if ( _tend > _tstart ) _exec = (double) (_tend-_tstart); \
    else _exec = (1000000.0 - _tstart) + _tend; }

#define ADCL_FLAG_PERF     -100
#define ADCL_FLAG_NOPERF   -101

/* Possible values for STAT_flags */
#define ADCL_STAT_TESTED    0x00000001
#define ADCL_STAT_FILTERED  0x00000002
#define ADCL_STAT_EVAL      0x00000004

#define ADCL_STAT_IS_TESTED(STAT)   ((STAT)->s_flags & ADCL_STAT_TESTED)
#define ADCL_STAT_IS_FILTERED(STAT) ((STAT)->s_flags & ADCL_STAT_FILTERED)
#define ADCL_STAT_IS_EVAL(STAT)     ((STAT)->s_flags & ADCL_STAT_EVAL)

#define ADCL_STAT_SET_TESTED(STAT)   ((STAT)->s_flags |= ADCL_STAT_TESTED)
#define ADCL_STAT_SET_FILTERED(STAT) ((STAT)->s_flags |= ADCL_STAT_FILTERED)
#define ADCL_STAT_SET_EVAL(STAT)     ((STAT)->s_flags |= ADCL_STAT_EVAL)

struct ADCL_statistics_s{
    short         s_count; /* how often has this routine already been called */
    short      s_rescount; /* how often has this routine already reported back */
    TIME_TYPE    *s_time;  /* measurements */
    int          s_flags;  /* Has this data set already been filtered? */
    double      s_lpts[3]; /* local no. of pts by this function */
#ifdef V3
    double      s_gpts[3]; /* global no. of pts for this function */
#endif
};
typedef struct ADCL_statistics_s ADCL_statistics_t;

int ADCL_statistics_filter_timings  ( ADCL_statistics_t **stats, int count, 
				      int rank );
int ADCL_statistics_global_max ( ADCL_statistics_t **stats, int count,
 				 MPI_Comm comm, int num_blocks, int *blength, 
				 int *winners, int rank );

double ADCL_statistics_time(void);
#ifdef V3
int ADCL_statistics_global_max_v3 ( ADCL_statistics_t **statistics, int count,
				    MPI_Comm comm, int rank );
int ADCL_statistics_get_winner_v3 ( ADCL_statistics_t **statistics, int count, 
				    int *winner );
#endif

struct ADCL_hypothesis_s {
    int                *h_attr_hypothesis; /* List of performance hypothesis*/
    int                *h_attr_confidence; /* List of confidence values */
    ADCL_attribute_t **h_active_attr_list; /* List of attributes currently being investigated */
    int                  h_num_avail_meas; /* Counter keeping track of how many 
					       methods have already been tested*/
    int               h_num_required_meas;  
    int                h_num_active_attrs;  
#ifdef V3
    int                        *h_noneval; /* Array containing the positions of the 
					      functions in the function set which 
					      have been executed but not yet 
					      statistically analysed */
#endif
};
typedef struct ADCL_hypothesis_s ADCL_hypothesis_t;




#endif
