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


//int deconly; 
//extern int output_files;
static int local_outlier_factor=3;
static int collective_outlier_factor=2;
static int outlier_fraction=20;
int nmeas_use;
 
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



 
/**********************************************************************/
int main (int argc, char **argv )
/* reads *.out files and inits em_time and em_avg */
{
    int nlines=-1, nperflines = 0; 
    int ret, i, iproc, imeas;
    int obj_id, method_id, obj_pos; 
    int pos, count; 
    char line[MAXLINE], inname[MAXLINE];
    int nmeas;
    double **timings; 

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
  
    /* apply filtering algorithms */
    for ( obj_id=0; obj_id<commentline->nobjects; obj_id++) {
        object = &commentline->objects[obj_id]; 

        /* how many measurements are taken into account? */        
        if ( 0 < nmeas_use  && nmeas_use < emethods[obj_id][0][0].em_nmeas ) {
           nmeas = nmeas_use;
        }
        else {
           nmeas = emethods[obj_id][0][0].em_nmeas;
        }


        /* initialize array of timings for collective-flavor methods */
        timings = (double **) malloc ( object->nimpl * sizeof(double*) );
        //thelp  = (double *) malloc ( object->nimpl * nmeas * sizeof(double) );
        for ( method_id = 0; method_id<object->nimpl; method_id++) {
            timings[method_id] = (double *) malloc ( nmeas * sizeof(double) ); 
            //timings[method_id] = &thelp[method_id * object->nimpl]; 
        } 
     
        /* calculate for each measurement max over all procs */
        for ( method_id=0; method_id<object->nimpl; method_id++ ) {
           for ( imeas=0; imeas<nmeas; imeas++ ) { 
              timings[method_id][imeas] = 0.0;
              for (iproc=0; iproc<commentline->nprocs; iproc++ ) {
                  MAX (timings[method_id][imeas], emethods[object->obj_pos][iproc][method_id].em_time[imeas]);
              }
           }
        }

        //if ( ! object->ignore ) { 
        printf( "\ncomputing winner for %s %d\n", object->objstr, object->obj_id ); 
        /* heuristic */
        minmax_heuristic_local ( object->nimpl, nmeas, object->idx_range[0], emethods[object->obj_pos],  
                commentline->nprocs, local_outlier_factor, outlier_fraction ); 
        minmax_heuristic_collective ( object->nimpl, nmeas, object->idx_range[0], timings,  
                commentline->nprocs, collective_outlier_factor, outlier_fraction ); 

        /* iqr */
        minmax_iqr_local ( object->nimpl, nmeas, object->idx_range[0], emethods[object->obj_pos],  
                commentline->nprocs ); 
        minmax_iqr_collective ( object->nimpl, nmeas, object->idx_range[0], timings,  
                commentline->nprocs ); 

        /* cluster analysis */
        minmax_cluster_local ( object->nimpl, nmeas, object->idx_range[0], emethods[object->obj_pos],  
                commentline->nprocs, outlier_fraction ); 
        minmax_cluster_collective ( object->nimpl, nmeas, object->idx_range[0], timings,  
                commentline->nprocs, outlier_fraction ); 

#if defined(GSL) || defined(NR)
        /* robust statistics */
        minmax_robust_local ( object->nimpl, nmeas, object->idx_range[0], emethods[object->obj_pos],  
                commentline->nprocs ); 
        minmax_robust_collective ( object->nimpl, nmeas, object->idx_range[0], timings,  
                commentline->nprocs ); 
#endif

        free (timings);
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
    int tmpargc=1;  /* ignore program name */
  
    while ( tmpargc < argc ) {
       if ( strncmp ( argv[tmpargc], "-h", strlen("-h") )== 0 ) {
          printf(" Usage : minmax <options>\n\n");
          printf("   minmax takes the output files generated by ADCL and \n");
          printf("   determines for each individual measurement the minimal and \n");
          printf("   maximual value across all processes and the according locations\n");
          printf(" Options: \n");
          printf("   -ofiles       : write output files \n");
          printf("   -ofraction    : outlier fraction to be used \n");
          printf("   -ofactor_loc  : outlier factor for local flavor statistics \n");
          printf("   -ofactor_coll : outlier factor for collective flavor statistics \n");
          printf("   -nmeas        : number of measurements taken into account for the evaluation \n");

          exit ( 1 ) ;
       }
       //else if ( strncmp(argv[tmpargc], "-ofiles", strlen("-ofiles") )== 0 ) {
       //    deconly=1;
       //    output_files = 1;
       //}
       else if ( strncmp(argv[tmpargc], "-ofactor_coll", strlen("-ofactor_coll") )== 0 ) {
           tmpargc++;
           collective_outlier_factor = atoi ( argv[tmpargc] );
       }
       else if ( strncmp(argv[tmpargc], "-ofactor_loc", strlen("-ofactor_loc") )== 0 ) {
           tmpargc++;
           local_outlier_factor = atoi ( argv[tmpargc] );
       }
       else if ( strncmp(argv[tmpargc], "-ofraction", strlen("-ofraction") )== 0 ) {
           tmpargc++;
           outlier_fraction = atoi ( argv[tmpargc] );
       }
       else if ( strncmp(argv[tmpargc], "-nmeas", strlen("-nmeas") )== 0 ) {
           tmpargc++;
           nmeas_use  = atoi ( argv[tmpargc] );
       }
       else { 
            printf("Unkown argument %s. Exiting ...\n", argv[tmpargc]); 
            exit (-1);
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
    int ret, i, iproc; 
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

