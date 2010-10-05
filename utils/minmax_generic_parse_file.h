#ifndef __MINMAX_H__
#define __MINMAX_H__

#define MAXLINE 128
#define MAX(a,b) if (b > a) {a = b;}
#define MAXOBJ  20


/********************************************************************************************************************************/

struct emethod{
    int     em_count;                /* #measurements per method */
    int     em_nmeas;                /* #measurements per method read so far */
    double  *em_time;                /* array of measurements, dimension numtests */
    int     *em_poison;
    int     em_num_outliers;
    double  em_avg;
//    double  em_median;
//    double  em_1stquartile;
//    double  em_3rdquartile;
//    double  em_iqr;
//    double  em_llimit;
//    double  em_ulimit;
//    double  em_sum_filtered;
//    int     em_cnt_filtered;
//    int     em_cnt_outliers;  /* number of outliers */ 
    double  em_avg_filtered;
    double  em_perc_filtered;
};

/********************************************************************************************************************************/

struct s_perfline {         /* information of lines with performance information */
   int  proc;               /* processor number */
   char objstr[MAXLINE];    /* currently "emethod" or "timer" */
   int  obj_id;             /* id of object */
   int  req_id;             /* if request present, id of request, else -1 */
   int  method_id;          /* id of method */
   char fnctstr[MAXLINE];   /* name of the implementation */
   int  cpat;               /* communication pattern:
                               0 - next neighbor
                               1 - allreduce
                               2 - allgatherv */
   double time;             /* execution time */
};
typedef struct s_perfline PERFLINE;

/********************************************************************************************************************************/

struct s_object{
   char objstr[MAXLINE];   /* currently "emethod" or "timer" */
   int  obj_pos;           /* index of object in commentline->objects array */
   int  obj_id;            /* id of object */
   int  idx_range[2];      /* range of method ids */
   int  nimpl;             /* number of implementations */
   int  winnerfound;       /* flag */
   int  meas[2];           /* measurements until and after winner is found */
   int  ignore;            /* 1 if implemenation is fixed */
};

/********************************************************************************************************************************/
typedef struct s_object OBJECT;
struct s_commentline { /* informations of lines starting with # */
   int ignore_pattern[3];
   char fixed_algo[3][MAXLINE];
   int  numtests;
   int  nprocs;
   int  nobjects;         /* number of different objects */
   OBJECT objects[MAXOBJ];     /* object is identified by an object string and a number */
};
typedef struct s_commentline COMMLINE;

/********************************************************************************************************************************/

int ml(const int ndata,  double* const data, 
       double* nu, double *mu, double *sigma, double *val);

/* filtering strategies */
static void filter_heuristic ( int nmeas, int outlier_factor, double *time, 
   double *avg, double *avg_filtered, double *perc_filtered );
void minmax_heuristic_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, 
   int nprocs, int outlier_factor, int outlier_fraction );
void minmax_heuristic_collective ( int nimpl, int nmeas, int idx_start, double **timings, 
   int nprocs, int outlier_factor, int outlier_fraction );
static void calc_decision_heuristic ( int nimpl, int idx_start, double *unf, double *filt, double *perc,  int outlier_factor );

void minmax_iqr_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, int nprocs );
void minmax_iqr_collective ( int nimpl, int nmeas, int idx_start, double **timings, int nprocs );
static void filter_iqr ( int nmeas, double *time, double *avg_filtered, double *perc_filtered );

void minmax_cluster_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, int nprocs ) ;
void minmax_cluster_collective ( int nimpl, int nmeas, int idx_start, double **timings, int nprocs ) ;

void minmax_robust_local ( int nimpl, int nmeas, int idx_start, struct emethod **em, int nprocs ) ;
void minmax_robust_collective ( int nimpl, int nmeas, int idx_start, double **timings, int nprocs );
    



/*struct lininf {
    int    req;
    int    method;
    double min;
    double max;
    int    minloc;
    int    maxloc;
}; */

/*#define TLINE_INIT(_t) {_t.req=-1; _t.min=9999999999.99; _t.max=0.0; \
       _t.minloc=-1; _t.maxloc=-1;}
#define TLINE_MIN(_t, _time, _i){ \
           if ( _time < _t.min ) { \
	       _t.min    = _time;  \
	       _t.minloc = _i;}}
#define TLINE_MAX(_t, _time, _i) { \
	    if ( _time > _t.max ) { \
		_t.max = _time;     \
		_t.maxloc =_i;}} */



#endif
