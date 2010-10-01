/*
 * Copyright (c) 2009           High Performance Computing Center Stuttgart. All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "minmax_generic_parse_file.h"
#define MAXOBJ  20


static int deconly; 
static int output_files;
static int outlier_factor=3;
static int outlier_fraction=20;

 
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

static int minmax_init_perfline( PERFLINE *perfline ); 
/********************************************************************************************************************************/
int minmax_init_perfline( PERFLINE *perfline ) {
/********************************************************************************************************************************/
   perfline->proc = -1;
   perfline->objstr[0] = '\0';
   perfline->obj_id = -1;
   perfline->req_id = -1; 
   perfline->method_id = -1;
   perfline->fnctstr[0] = '\0';
   perfline->cpat = -1; 
   perfline->time = -999999.0;

   return 0; 
}

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
typedef struct s_object OBJECT;

static int minmax_init_object( OBJECT* object );
/********************************************************************************************************************************/
int minmax_init_object( OBJECT* object ) {
/********************************************************************************************************************************/
   object->objstr[0] = '\0';
   object->obj_id = -1;
   object->idx_range[0] = 9999;
   object->idx_range[1] = -1;
   object->nimpl = -1;
   object->winnerfound = 0;
   object->meas[0] = 0; 
   object->meas[1] = 0; 
   object->ignore = 0;

   return 0; 
} 

/********************************************************************************************************************************/
struct s_commentline { /* informations of lines starting with # */
   int ignore_pattern[3];
   char fixed_algo[3][MAXLINE];     
   int  numtests;
   int  nprocs; 
   int  nobjects;         /* number of different objects */
   OBJECT objects[MAXOBJ];     /* object is identified by an object string and a number */
};
typedef struct s_commentline COMMLINE;

static int minmax_init_commentline ( COMMLINE* commentline );
/********************************************************************************************************************************/
int minmax_init_commentline ( COMMLINE* commentline ) {
/********************************************************************************************************************************/
   int i; 
   for ( i=0; i<3; i++) {
     commentline->ignore_pattern[i] = 0; 
     commentline->fixed_algo[i][0]  = '\0';
   }
   commentline->numtests = 20;
   commentline->nprocs=-1;
   commentline->nobjects  = 0;     
   for ( i=0; i<MAXOBJ; i++) {
      minmax_init_object ( &commentline->objects[i] );
   }

   return 0; 
}


static void minmax_read_perfline( char* line, PERFLINE* perfline, 
        COMMLINE* commentline ) ;
static int minmax_read_commentline ( char* line, COMMLINE* commentline) ;
static int minmax_get_lines (char *filename); 
static int minmax_get_object ( char* objstr, int* obj_id, int cpat, COMMLINE* commentline, OBJECT** object ); 
static void minmax_print_info ( COMMLINE *commentline );
static void minmax_print_object ( OBJECT* object, int idx);
static int minmax_get_nb_files (char *file_pattern); 
static int minmax_compare_perflines ( PERFLINE *perfline1, PERFLINE *perfline2 );
static int minmax_compare_commentlines ( COMMLINE* commline1, COMMLINE* commline2 );
static void minmax_init ( COMMLINE *commline, struct emethod ****emethods);
static void minmax_parse_args (int argc, char ** argv); 
static void integrety_check (int nlines, COMMLINE *commentline, PERFLINE *perfline);
static void minmax_heuristic_local (int nimpl, int nmeas, int idx_start, struct emethod **em, 
   int nprocs, int outlier_factor );
static void minmax_heuristic_collective (int nimpl, int nmeas, int idx_start, struct emethod **em, 
   int nprocs, int outlier_factor );
static void  calc_decision ( int nimpl, int idx_start, double *unf, double *filt, double *perc,  int outlier_factor ); 


 
//void minmax_parse_file ( struct emethod ***emethods )
/**********************************************************************/
int main (int argc, char **argv )
/* reads *.out files and inits em_time and em_avg */
{
    int nlines=-1, nlines_new=-1, nperflines = 0; 
    int ret, i, iproc;
    int obj_id, method_id, obj_pos; 
    int pos, count; 
    char line[MAXLINE], inname[MAXLINE];

    FILE *infd=NULL;
    COMMLINE *commentline = NULL, *commentline_new = NULL; 
    PERFLINE *perfline = NULL; 
    //PERFLINE *perfline_new = NULL; 
    OBJECT *object; 
    struct emethod ***emethods;
    //int proc, obj_id, req_id, method_id, cpat, ignore_type, numtest;
    //double time;

    /* parse command line arguments */
    minmax_parse_args (argc, argv); 

    /* Determine #lines in 0.out */
    nlines = minmax_get_lines("0.out");
    if ( 0 > nlines ) {
       printf("Negative number of lines in 0.out");
       exit(-1);
    }

    /* Allocate memory */
    perfline = ( PERFLINE* ) malloc ( nlines * sizeof (PERFLINE) ); 
    nperflines = 0; 
    commentline = ( COMMLINE* ) malloc ( sizeof (COMMLINE) ); 
    commentline_new = ( COMMLINE* ) malloc ( sizeof (COMMLINE) ); 

    /* Determine number of out-files (processes) */
    minmax_init_commentline( commentline );
    commentline->nprocs = minmax_get_nb_files ("*.out"); 
    if ( 0 > commentline->nprocs ) {
       printf("No out-files.");
       exit(-1);
    }

    /* Parse 0.out */
    printf("Parsing 0.out ...\n");
    infd = fopen ( "0.out", "r" );
    for (i=0; i<nlines; i++) {
       ret = fscanf ( infd, "%[^\n]\n", line );
       if ( line[0] == '#') { 
          minmax_read_commentline ( line, commentline );
       }    
       else if (line[0] == '0' && line[1] == ':') { 
          minmax_init_perfline( &perfline[i] ); //nperflines] );
          minmax_read_perfline( line, &perfline[i], commentline ); 
          //printf("%d: %s %d request %d method %d (%s) time %lf, cpat=%d, ignore=%d\n", proc, objstr, obj_id,
          //   req_id, method_id, fnctstr, time, cpat;
          nperflines++;
       }
       else {
            printf( "Unable to parse line %s\n", line );
            exit (-1);
       }
    }
    fclose ( infd );
    minmax_print_info ( commentline ); 

    /* integrity check, are *.out files consistent? */
    printf("\nStarting integrity check ...\n");
    integrety_check(nlines, commentline, perfline); 


    /* Compute additional variables */
    for ( i=0; i<commentline->nobjects; i++) {
       object = &commentline->objects[i]; 
       object->nimpl = object->idx_range[1] - object->idx_range[0] + 1;
    }


    /* Copy performance information of *.out in emethod-structure */
    printf("\nExtracting performance data ...\n");
    minmax_init ( commentline, &emethods);
    for ( iproc=0; iproc<commentline->nprocs; iproc++ ) {
       sprintf( inname, "%d.out", iproc);
       printf("... of file %s...", inname);
       infd = fopen ( inname, "r" );
       if (NULL == infd ) {
           printf("Could not open input file %s for reading\n", inname );
           exit (-1);
       }
       for (i=0; i<nlines; i++) {
          ret = fscanf ( infd, "%[^\n]\n", line );
          if ( line[0] != '#') { 
             minmax_init_perfline( perfline );
             minmax_read_perfline( line, perfline, commentline_new ); 
             minmax_get_object ( perfline->objstr, &perfline->obj_id, perfline->cpat, commentline, &object ); 

             if ( object->ignore ) continue; 
 
             obj_pos = object->obj_pos;
             method_id = perfline->method_id-object->idx_range[0];

             pos = emethods[obj_pos][iproc][method_id].em_nmeas;
             count = emethods[obj_pos][iproc][method_id].em_count;
             if ( pos < count ) { 
                 emethods[obj_pos][iproc][method_id].em_time[pos] = perfline->time;
                 //printf("obj. %d, meas %d, method %d, pos %d: time %lf\n", obj_pos, iproc, method_id, 
                 //        pos, perfline->time ); 
                 emethods[obj_pos][iproc][method_id].em_nmeas++;
             }
          }
       }
       fclose( infd );
       printf(" OK\n");
    }

#ifdef DEBUG
    for ( obj_id=0; obj_id<commentline->nobjects; obj_id++) {
        object = &commentline->objects[obj_id];
        if ( ! object->ignore ) { 
            for ( iproc=0; iproc<commentline->nprocs; iproc++){
                for ( method_id=0; method_id<object->nimpl; method_id++) {
                    for ( i=0; i<emethods[obj_id][iproc][method_id].em_nmeas; i++) {
                        printf("obj. %d, proc %d, method %d, pos %d: time %lf\n", obj_id, iproc, method_id, 
                                i, emethods[obj_id][iproc][method_id].em_time[i]); 

                    }
                    printf("obj. %d, proc %d, method %d, nmeas %d\n", obj_id, iproc, method_id, 
                            emethods[obj_id][iproc][method_id].em_nmeas ) ;
                }
            }
        }
    }
#endif
#ifdef MUELL
    double max, sum; 
    for ( obj_id=0; obj_id<commentline->nobjects; obj_id++) {
        object = &commentline->objects[obj_id];
        if ( ! object->ignore ) { 
                for ( method_id=0; method_id<8; method_id++) {
                    for ( i=0; i<20; i++) {
                      max = 0.0;
                      sum = 0.0; 
                      for ( iproc=0; iproc<32; iproc++){
                        sum += emethods[obj_id][iproc][method_id].em_time[i]; 
                        if ( max < emethods[obj_id][iproc][method_id].em_time[i]) {
                            max = emethods[obj_id][iproc][method_id].em_time[i];
                        } 
                        //printf("obj. %d, proc %d, method %d, pos %d: time %lf\n", obj_id, iproc, method_id, 
                        //        i, emethods[obj_id][iproc][method_id].em_time[i]); 

                    }
                    printf("%lf %lf\n", max, sum/20); 
                    //printf("method %d, pos %d: time %lf\n", method_id, i, max); 
                }
            }
        }
    }
#endif 
    ////commentline->objects[0].ignore=0;
    //for ( obj_id=0; obj_id<commentline->nobjects; obj_id++) {
    //    printf("obj\n"); 
    //    object = &commentline->objects[obj_id];
    //    if ( ! object->ignore ) {
    //        for ( method_id=0; method_id<object->nimpl; method_id++) {
    //        printf("method\n"); 
    //            printf("emethod\n"); 
    //            for ( i=0; i<emethods[obj_id][0][method_id].em_nmeas; i++) {
    //                double max = 0.0;
    //                for ( iproc=0; iproc<commentline->nprocs; iproc++){
    //                    printf("method %d, pos %d: time %lf\n", iproc, method_id, emethods[obj_id][iproc][method_id].em_time[i]); 
    //                    if ( max < emethods[obj_id][iproc][method_id].em_time[i]) {
    //                        max = emethods[obj_id][iproc][method_id].em_time[i];
    //                    }
    //                }
    //                printf("method %d, pos %d: time %lf\n", method_id, i, max); 
    //                //    printf("obj. %d, proc %d, method %d, pos %d: time %lf\n", obj_id, iproc, method_id,
    //                //             i, emethods[obj_id][iproc][method_id].em_time[i]);
    //            }
    //        }
    //    }
    //}

    for ( obj_id=0; obj_id<commentline->nobjects; obj_id++) {
        object = &commentline->objects[obj_id]; 
        //if ( ! object->ignore ) { 
            printf( "\ncomputing winner for %s %d\n", object->objstr, object->obj_id ); 
            minmax_heuristic_local ( object->nimpl, -1, object->idx_range[0], emethods[object->obj_pos],  
                    commentline->nprocs, outlier_fraction ); 
            minmax_heuristic_collective ( object->nimpl, -1, object->idx_range[0], emethods[object->obj_pos],  
                    commentline->nprocs, outlier_fraction ); 
        //}
    }

    /* free memory */
    free(perfline);
    free(commentline);
    free(commentline_new);

    return 0;
}

/********************************************************************************************************************************/
int minmax_get_lines (char *filename){ 
/********************************************************************************************************************************/
/* determines number of lines of file "filename" */
    FILE *infd=NULL;
    int nlines=-1; 
    char cmd[MAXLINE], line[MAXLINE];
 

    sprintf(cmd, "wc -l %s > lines.out", filename);
    system(cmd);
    infd = fopen ( "lines.out", "r" );
    if (NULL == infd) {
        printf("Could not open input file %s for reading\n", filename);
        exit (-1);
    }
    fscanf ( infd, "%[^\n]\n", line );
    sscanf ( line, "%d", &nlines );
    system("rm -f lines.out");
    //printf("Detected %s with %d lines\n", filename, nlines);

    return nlines;
}

/********************************************************************************************************************************/
int minmax_get_nb_files (char *file_pattern){ 
/********************************************************************************************************************************/
/* determines number of lines with pattern <filen_pattern> */
    FILE *infd=NULL;
    int nprocs=-1; 
    char cmd[MAXLINE], line[MAXLINE];
 

    sprintf(cmd, "ls %s | grep ^[0-9]*\\.out | wc -l > tmp1234", file_pattern);
    system(cmd);
    infd = fopen ( "tmp1234", "r" );
    if (NULL == infd) {
        printf("Could not open input file 0.out for reading\n");
        exit (-1);
    }
    fscanf ( infd, "%[^\n]\n", line );
    sscanf ( line, "%d", &nprocs );
    system("rm -f tmp1234");
    printf("Detected %d files with %s\n", nprocs, file_pattern );

    return nprocs;
}


/********************************************************************************************************************************/
void minmax_read_perfline( char* line, PERFLINE* perfline, COMMLINE* commentline ) {
/* reads one line of out-file                                         */
/********************************************************************************************************************************/
   char str[MAXLINE], colon[1];
   char *basestr;
   OBJECT* object;

   //printf("line %s\n", line);
   
   sscanf ( line, "%d", &perfline->proc );

   basestr = strstr ( line, ":" );
   sscanf ( basestr, "%s %s %d", colon, perfline->objstr, &perfline->obj_id );

   basestr = strstr ( line, "request" );
   if ( NULL != basestr ) {
      sscanf ( basestr, "%s %d", str, &perfline->req_id );
   }
   else {
      perfline->req_id = -1;
   }

   basestr = strstr ( line, " method" );
   sscanf ( basestr, " %s %d", str, &perfline->method_id );

   basestr = strstr ( line, "(" );
   sscanf ( basestr, "%1s%s)", str, perfline->fnctstr );
   if ( NULL != strstr ( perfline->fnctstr, "Allreduce") ) {
      perfline->cpat = 1;
   }
   else if ( NULL != strstr ( perfline->fnctstr, "AllGatherV") ) {
      perfline->cpat = 2;
   }
   else { 
      perfline->cpat = 0;
   }

   basestr = strstr ( line, ")" ); 
   sscanf ( basestr, "%1s %lf", str, &perfline->time );
   if ( 0 >= perfline->time ) {
      printf( "Invalid time: extracted '%lf' from %s. Exiting.\n", perfline->time, line );
      exit(-1);
   }

   //printf("%d: %10s %d request %d method %d (%s) time %lf, cpat=%d, ignore=%d\n", 
   //   perfline->proc, perfline->objstr, perfline->obj_id, perfline->req_id, perfline->method_id, 
   //   perfline->fnctstr, perfline->time, perfline->cpat);

   minmax_get_object ( perfline->objstr, &perfline->obj_id, perfline->cpat, commentline, &object ); 
   /* update range */
   if ( perfline->method_id < object->idx_range[0]) 
      object->idx_range[0] = perfline->method_id;
   if ( perfline->method_id > object->idx_range[1]) 
      object->idx_range[1] = perfline->method_id;

   if ( object->winnerfound ) {
      object->meas[1]++;
   }
   else {
      object->meas[0]++; 
   }
}

/********************************************************************************************************************************/
int minmax_get_object ( char* objstr, int* obj_id, int cpat, COMMLINE* commentline, OBJECT** object ){ 
/********************************************************************************************************************************/
   int i; 
   OBJECT* tobject;

   /* does object with the same objstr and the same obj_id exist? */
   for ( i=0; i<commentline->nobjects; i++ ) {
       tobject = &commentline->objects[i];
       if ( strcmp( objstr, tobject->objstr ) == 0 && *obj_id == tobject->obj_id ) {
          *object = tobject;
          return 0; 
       }
   }

   if ( commentline->nobjects == MAXOBJ ) {
      printf("Out of objects. Increase MAXOBJ\n");
      exit(-1);
   }

   /* no luck so far: init new object */
   commentline->nobjects++;
   tobject = &commentline->objects[i];
   tobject->obj_pos = i;
   strcpy ( tobject->objstr, objstr );
   tobject->obj_id = *obj_id;
   if ( commentline->ignore_pattern[cpat] == 1 ) {
      tobject->winnerfound = 1;
   }
   if ( commentline->ignore_pattern[cpat] ) {
      tobject->ignore = 1; 
   }

   *object = tobject;
   return 0;
}


/********************************************************************************************************************************/
int minmax_read_commentline ( char* line, COMMLINE* commentline) {
/********************************************************************************************************************************/
   /* reads line <line> (in) of outfile starting with # and eventually modifies
      contents of <commentline> */
   char* basestr; 
   char colon[2]; /* for ":\0" */
   char objstr[MAXLINE];
   int  obj_id; 
   OBJECT* object; 

   if ( NULL != strstr( line, "#ADCL_EMETHOD_SELECTION" ) ) {
      commentline->ignore_pattern[0] = 1;
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%s %s", colon, &(commentline->fixed_algo[0][0]) );
      //printf("Ignoring function %s\n", &(commentline->fixed_algo[0][0]));
   }
   else if ( NULL != strstr( line, "#ADCL_EMETHOD_ALLREDUCE_SELECTION" ) ) {
      commentline->ignore_pattern[1] = 1;
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%s %s", colon, &(commentline->fixed_algo[1][0]) );
      //printf("Ignoring function %s\n", &(commentline->fixed_algo[1][0]) );
   }
   else if ( NULL != strstr( line, "#ADCL_EMETHOD_ALLGATHERV_SELECTION" ) ) {
      commentline->ignore_pattern[2] = 1;
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%s %s", colon, &(commentline->fixed_algo[2][0]) );
      //printf("Ignoring function %s\n", &(commentline->fixed_algo[2][0]) );
   }  
   else if ( NULL != strstr( line, "#ADCL_EMETHOD_NUMTESTS" ) ) {
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%1s %d", colon, &commentline->numtests );
      //printf("Numtests =  %d\n", commentline->numtests);
   }  
   else if ( NULL != strstr ( line, "winner is") ) { 
       basestr = strstr ( line, ":" );
       sscanf ( basestr, "%1s %s %d", colon, objstr, &obj_id );
       if ( NULL != strstr ( objstr, "req" ) ) { /* some one uses the old format */
          printf("Old file format. Exiting ...\n"); 
          exit(-1); 
       }
       minmax_get_object ( objstr, &obj_id, 1, commentline, &object ); 
       object->winnerfound = 1; 
   }

   return 0;
}

/********************************************************************************************************************************/
int minmax_compare_commentlines ( COMMLINE* commline1, COMMLINE* commline2 ) {
/********************************************************************************************************************************/
   /* reads line <line> (in) of outfile starting with # and eventually modifies
      contents of <commentline> */
   int i;
   OBJECT *object1, *object2; 

   for ( i=0; i<3; i++) {
      if ( commline1->ignore_pattern[i] != commline2->ignore_pattern[i] || 
           strcmp ( commline1->fixed_algo[i], commline2->fixed_algo[i] ) != 0 ) 
         return 1;
   }

   if ( commline1->numtests != commline2->numtests ||
        commline1->nobjects != commline2->nobjects )
        return 1;

   for ( i=0; i<commline1->nobjects; i++ ) {
       object1 = &commline1->objects[i];
       object2 = &commline2->objects[i];

       if ( strcmp ( object1->objstr, object2->objstr ) != 0 ||
            object1->idx_range[0] != object2->idx_range[0]   ||  
            object1->idx_range[1] != object2->idx_range[1]   ||  
            object1->winnerfound  != object2->winnerfound    ||
            object1->meas[0]      != object2->meas[0]        ||  
            object1->meas[1]      != object2->meas[1]        ||  
            object1->winnerfound  != object2->winnerfound )
       return 1;
   }

   /* everything OK */
   return 0;
}

/********************************************************************************************************************************/
void minmax_print_object ( OBJECT* object, int idx) {
/********************************************************************************************************************************/

   printf("object %d: %s %d   methods: %d -- %d    measurements: %d for perf., %d total\n",
       idx, object->objstr, object->obj_id, object->idx_range[0], object->idx_range[1], 
       object->meas[0], object->meas[1] ); 
}

/********************************************************************************************************************************/
void minmax_print_info ( COMMLINE *commentline ) {
/********************************************************************************************************************************/
   int i; 

   printf("\nSummary information for %d objects\n", commentline->nobjects);
   for ( i=0; i<commentline->nobjects; i++) {
       minmax_print_object ( &commentline->objects[i], i); 
   }

   if ( commentline->ignore_pattern[0] == 1 ) {
      printf("Ignoring Next-Neighbor Communications: implementation is set to %s\n", commentline->fixed_algo[0] ); 
   }
   else if ( commentline->ignore_pattern[1] == 1 ) {
      printf("Ignoring Allreduce Communications: implementation is set to %s\n", commentline->fixed_algo[1] ); 
   }
   else if ( commentline->ignore_pattern[2] == 1 ) {
      printf("Ignoring AllGatherV Communications: implementation is set to %s\n", commentline->fixed_algo[2] ); 
   }
}


/********************************************************************************************************************************/
int minmax_compare_perflines ( PERFLINE *perfline1, PERFLINE *perfline2 ) {
/********************************************************************************************************************************/
   int ret = 0;

   if ( strcmp (perfline1->objstr, perfline2->objstr ) != 0 ||
        ( perfline1->obj_id  != perfline2->obj_id )         ||
        ( perfline1->req_id  != perfline2->req_id )         ||
        ( perfline1->method_id !=  perfline2->method_id )   ||
        strcmp (perfline1->fnctstr, perfline2->fnctstr ) != 0  ) {
      ret = 1;
   }

   return ret; 
}

/********************************************************************************************************************************/
void minmax_parse_args (int argc, char ** argv) {
/********************************************************************************************************************************/
    int tmpargc=0;

    while ( tmpargc < argc ) {
       if ( strncmp ( argv[tmpargc], "-h", strlen("-h") )== 0 ) {
          printf(" Usage : minmax <options>\n\n");
          printf("   minmax takes the output files generated by ADCL and \n");
          printf("   determines for each individual measurement the minimal and \n");
          printf("   maximual value across all processes and the according locations\n");
          printf(" Options: \n");
          printf("   -ofiles      : write output files \n");
          printf("   -ofraction   : outlier fraction to be used \n");
          printf("   -ofactor     : outlier factor to be used \n");

          exit ( 1 ) ;
       }
       else if ( strncmp(argv[tmpargc], "-ofiles", strlen("-ofiles") )== 0 ) {
           deconly=1;
           output_files = 1;
       }
       else if ( strncmp(argv[tmpargc], "-ofactor", strlen("-ofactor") )== 0 ) {
           tmpargc++;
           outlier_factor = atoi ( argv[tmpargc] );
       }
       else if ( strncmp(argv[tmpargc], "-ofraction", strlen("-ofraction") )== 0 ) {
           tmpargc++;
           outlier_fraction = atoi ( argv[tmpargc] );
       }
       tmpargc++;
    }
}

/********************************************************************************************************************************/
void minmax_init ( COMMLINE *commline, struct emethod ****emethods) {
/********************************************************************************************************************************/
    struct emethod ***em;
    int obj, iproc, iImpl; 

    em = (struct emethod ***) malloc ( commline->nobjects * sizeof(struct emethod**) );
    if ( NULL != emethods ) {
        for ( obj=0; obj<commline->nobjects; obj++){
            if ( commline->objects[obj].ignore ) continue;  
            /* Allocate the required emethods array to hold the overall data */
            em[obj] = ( struct emethod **) malloc ( commline->nprocs * sizeof ( struct emethod *));
            if ( NULL == em[obj] ) {
                printf("minax_init: undefined pointer for emethod object no. %d. Exiting ...", obj);
                exit (-1);
            }

            for ( iproc=0; iproc< commline->nprocs; iproc++ ) {
                em[obj][iproc] = (struct emethod *) calloc (1, commline->objects[obj].nimpl*sizeof(struct emethod));
                if  ( NULL == em[obj][iproc] ) {
                    printf("minax_init: undefined pointer for proc %d of emethod object %d. Exiting ...", iproc, obj);
                    exit (-1);
                }

                for (iImpl=0; iImpl<commline->objects[obj].nimpl; iImpl++ ) {
                    em[obj][iproc][iImpl].em_time = (double *) calloc (1, commline->numtests * sizeof(double));
                    if ( NULL == em[obj][iproc][iImpl].em_time ) {
                        printf("minax_init: implementation %d of proc %d of emethod object %d has no timings. Exiting ...", 
                                iImpl, iproc, obj);
                        exit (-1);
                    }
                    em[obj][iproc][iImpl].em_poison = (int *) calloc (1, commline->numtests * sizeof(int));
                    if ( NULL == em[obj][iproc][iImpl].em_poison ) {
                        printf("minax_init: implementation %d of proc %d of emethod object %d has no poison array. Exiting ...", 
                                iImpl, iproc, obj);
                        exit (-1);
                    }
                    em[obj][iproc][iImpl].em_count    = commline->numtests;
                    em[obj][iproc][iImpl].em_nmeas = 0;
                }
           }
        }
    }
    *emethods = em;
    return;
}

/**********************************************************************/
/**********************************************************************/
void integrety_check (int nlines, COMMLINE *commentline, PERFLINE *perfline)
/* use information from above (nlines, commentline, perfline) and check if the other out-files 
   - have the same size 
   - each perfline has the same obj_id and method_id 
   - the commentline information is the same */
{
    int nlines_new=-1; //, nperflines = 0; 
    int ret, i, proc, iproc; 
    char line[MAXLINE], inname[MAXLINE];

    FILE *infd=NULL;
    COMMLINE *commentline_new = NULL; 
    PERFLINE *perfline_new = NULL; 
    
    perfline_new = ( PERFLINE* ) malloc ( sizeof (PERFLINE) ); 
    commentline_new = ( COMMLINE* ) malloc ( sizeof (COMMLINE) ); 

    for ( iproc=1; iproc<commentline->nprocs; iproc++ ) {
        sprintf( inname, "%d.out", iproc);
        printf("Comparing to file %s...", inname);
        infd = fopen ( inname, "r" );
        if (NULL == infd ) {
            printf("Could not open input file %s for reading\n", inname );
            exit (-1);
        }

        nlines_new = minmax_get_lines( inname );
        if ( nlines != nlines_new ) {
           printf("proc %d: Output file sizes differ. Exiting.", iproc);
           exit(-1);
        }

        for (i=0; i<nlines; i++) {
           ret = fscanf ( infd, "%[^\n]\n", line );
           if ( line[0] == '#') { 
              minmax_read_commentline ( line, commentline_new );
           }    
           else { 
              minmax_init_perfline( perfline_new );
              minmax_read_perfline( line, perfline_new, commentline_new ); 
              ret = minmax_compare_perflines ( &perfline[i], perfline_new);
              if ( ret != 0 ) {
                 printf("proc %d: line %d: Content of output file does not match. Exiting.\n", iproc, i);
                 exit(-1);
              }
           }
        }
        fclose( infd );
        minmax_compare_commentlines( commentline, commentline_new);
        if ( ret != 0 ) {
           printf("proc %d: Meta information of output file does not match. Exiting.\n", iproc);
           exit(-1);
        }
        printf(" OK\n");
    }

    free(perfline_new);
    free(commentline_new);
}

/**************************************************************************************************/
void filter_timings ( int nmeas_tot, int nmeas_use, int outlier_factor, double *time, 
   double *avg, double *avg_filtered, double *perc_filtered )
/**************************************************************************************************/
/* determines average, filtered average and percentage of filtered measurements for array of      */
/* measurements time                                                                              */ 
/* IN                                                                                             */
/* nmeas_tot      - total #measurements, length of array time                                     */
/* nmeas_max      - -1 or #measurements used for evaluation (optional)                            */
/* outlier_factor - measurements larger than outlier_factor * min are considered as outliers      */
/* time           - array of measurements                                                         */
/* OUT                                                                                            */
/* avg            - computed mean                                                                 */
/* avg_filtered   - filtered mean                                                                 */
/* perc_filtered  - percentage of filtered data                                                   */
/**************************************************************************************************/
/* based on em_avg, em_avg_filtered, and em_perc_filtered, the maximum on each proc and the  */
/* total minimum are computed */
{
   int k, nmeas, cnt_outliers;
   double sum, sum_filtered, min;

   if ( 0 < nmeas_use  && nmeas_use < nmeas_tot) {
      nmeas = nmeas_use;
   }
   else {
      nmeas = nmeas_tot;
   }

   sum = 0.0;
   sum_filtered = 0.0;
   cnt_outliers= 0;

   /* Determine the min value for */
   for ( min=1.0E15, k=0; k<nmeas; k++ ) {
      if ( time[k] < min ) { min = time[k]; }
   }

   /* Count how many values are N times larger than the min and
      mark those as outliers, sum up execution times of other values */
   for ( k=0; k<nmeas; k++ ) {
      sum += time[k]; 
      if ( time[k] >= (outlier_factor * min) ) {
          //em[iproc][method_id].em_poison[k] = 1;   // set to use minmax_calc_per_iteration */
          cnt_outliers++;
#ifdef DEBUG
//            printf("#%d: request %d method %d meas. %d is outlier %lf min %lf\n",
//                   iproc, r_id, method_id, k,  time[method_id], min );
#endif
      }
      else {
         sum_filtered += time[k];
      }
   }

   /* calculate average (filtered) time and outlier percentage */
   *avg           = sum / nmeas; 
   *avg_filtered  = sum_filtered / (nmeas - cnt_outliers);
   *perc_filtered = 100 * cnt_outliers / nmeas;

   return; 
}

/**************************************************************************************************/
void minmax_heuristic_local ( int nimpl, int nmeas_max, int idx_start, struct emethod **em, 
   int nprocs, int outlier_factor ) 
/**************************************************************************************************/
/* applies local flavor heuristic                                                                 */
/* - computes local (filtered) averages                                                           */
/* - maximizes over processors                                                                    */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas_max      - -1 or #measurements used for evaluation (optional)                            */
/* idx_start      - offset of implementation                                                      */
/* em             - emethod object,  em[nobjects][nprocs][nimpl]                                  */
/* nprocs         - number of processes                                                           */
/* outlier_factor - measurements larger than outlier_factor * min are considered as outliers      */
/**************************************************************************************************/
{
   int iproc, method_id, impl; // , nmeas;
   double *filt, *unf, *perc;
   double timpl, tmin, tmin_heuristic;
   int  minloc, minloc_heuristic;
   char isfilt_str[2], isfilt_min[2];

   unf   = (double *) malloc ( nimpl * sizeof(double) );
   filt  = (double *) malloc ( nimpl * sizeof(double) );
   perc  = (double *) malloc ( nimpl * sizeof(double) );

   /* compute (filtered) averages for each method on each process */
   for (iproc=0; iproc<nprocs; iproc++ ) {
      for ( method_id=0; method_id<nimpl; method_id++ ) {
         filter_timings ( em[iproc][method_id].em_nmeas, nmeas_max, outlier_factor, em[iproc][method_id].em_time,  
             &em[iproc][method_id].em_avg, &em[iproc][method_id].em_avg_filtered, &em[iproc][method_id].em_perc_filtered );
      }
   }
         
   /* calculate for each implementation max over all procs */
   for (method_id=0; method_id<nimpl; method_id++ ) {
       perc[method_id] = 0.0;
       unf[method_id]  = 0.0;
       filt[method_id] = 0.0;
       for (iproc=0; iproc<nprocs; iproc++ ) {
           MAX ( unf[method_id],  em[iproc][method_id].em_avg           );
           MAX ( filt[method_id], em[iproc][method_id].em_avg_filtered  );
           MAX ( perc[method_id], em[iproc][method_id].em_perc_filtered );
       }
   }

   printf("Local heuristic:\n"); 
   calc_decision ( nimpl, idx_start, unf, filt, perc, outlier_factor );

   free (unf);
   free (filt);
   free (perc);

   return;
}

/**************************************************************************************************/
void minmax_heuristic_collective ( int nimpl, int nmeas_max, int idx_start, struct emethod **em, 
   int nprocs, int outlier_factor ) 
/**************************************************************************************************/
/* applies collective flavor heuristic                                                            */
/* - maximizes over processes                                                                     */
/* - computes (filtered) averages                                                                 */
/* - minimizes to find best implementation                                                        */
/*                                                                                                */
/* nimpl          - #implementations / methods                                                    */
/* nmeas_max      - -1 or #measurements used for evaluation (optional)                            */
/* idx_start      - offset of implementation                                                      */
/* em             - emethod object,  em[nobjects][nprocs][nimpl]                                  */
/* nprocs         - number of processes                                                           */
/* outlier_factor - measurements larger than outlier_factor * min are considered as outliers      */
/**************************************************************************************************/
{
   int iproc, method_id, impl, imeas, nmeas;
   double *filt, *unf, *perc;
   double *timings;

   timings  = (double *) malloc ( nmeas * sizeof(double) );
   unf      = (double *) malloc ( nimpl * sizeof(double) );
   filt     = (double *) malloc ( nimpl * sizeof(double) );
   perc     = (double *) malloc ( nimpl * sizeof(double) );

   nmeas = em[0][0].em_nmeas;
   for ( method_id=0; method_id<nimpl; method_id++ ) {
      /* calculate for each measurement max over all procs */
      for ( imeas=0; imeas<nmeas; imeas++ ) { /* does not hurt to compute max forall measurements */
         timings[imeas] = 0.0;
         for (iproc=0; iproc<nprocs; iproc++ ) {
             MAX (timings[imeas], em[iproc][method_id].em_time[imeas]);
         }
      }

      /* compute (filtered) averages for each method */
      filter_timings ( nmeas,  nmeas_max, outlier_factor, timings,  
         &unf[method_id], &filt[method_id], &perc[method_id] );
   }

   printf("Collective heuristic:\n"); 
   calc_decision ( nimpl, idx_start, unf, filt, perc, outlier_factor );

   free (timings);
   free (unf);
   free(filt);
   free (perc);

   return;
}

/**************************************************************************************************/
void  calc_decision ( int nimpl, int idx_start, double *unf, double *filt, double *perc,  
   int outlier_factor ) 
/**************************************************************************************************/
/* does the last step of the heuristic (finding the best implementation out of an array with      */
/* estimated execution times for each implementation) and prints results                          */
/**************************************************************************************************/
{
   int iproc, method_id, impl, imeas, nmeas;
   double timpl, tmin, tmin_heuristic;
   int  minloc, minloc_heuristic;
   char isfilt_str[2], isfilt_min[2];

   tmin           = 1.E15;
   tmin_heuristic = 1.E15;
   /* calculate minimum communication time over all implementations */
   for ( method_id=0; method_id<nimpl; method_id++ ) {
      /* min without heuristic */
      if ( tmin > unf[method_id] ) {
           tmin   = unf[method_id];  
           minloc = method_id;
      }

      /* min  with heuristic */
      if ( perc[method_id] > outlier_fraction ) {
         strcpy(isfilt_str, "uf");
         timpl = unf[method_id];
      }
      else {
         strcpy(isfilt_str, "f");
         timpl = filt[method_id]; 
      }

      if ( tmin_heuristic >  timpl ) {
         tmin_heuristic = timpl;
         minloc_heuristic = method_id;
         strcpy(isfilt_min, isfilt_str);
      } 

      printf ("%d: result: %lf : %s :  unfiltered: %lf filtered: %lf perc: %lf\n",
              method_id, timpl, isfilt_str,  unf[method_id], filt[method_id], perc[method_id] );
   }

   printf("Without filtering: winner is %d\n", minloc + idx_start ); 
   printf("With    filtering: winner is %d (%s)\n\n", minloc_heuristic + idx_start, isfilt_str);

   return;
}

