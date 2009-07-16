#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/*#include "minmax_generic.h"*/
#define MAXLINE 1024
#define MAXOBJ  20
 
struct s_perfline {         /* information of lines with performance information */
   int  proc;               /* processor number */
   char objstr[MAXLINE];    /* currently "emethod" or "timer" */
   int  obj_id;             /* id of object */
   int  req_id;             /* if request present, id of request, else -1 */
   int  method_id;          /* id of method */ 
   char fnctstr[MAXLINE];   /* name of the implementation */
   int  type;               /* communication pattern: 
                               0 - next neighbor
                               1 - allreduce
                               2 - allgatherv */
   int  ignore_line;        /* 1, if user selected implementation for communication pattern 
                               0  if not */
   double time;             /* execution time */
};
typedef struct s_perfline PERFLINE;

/********************************************************************************************************************************/
int minmax_init_perfline( PERFLINE *perfline ) {
/********************************************************************************************************************************/
   perfline->proc = -1;           
   perfline->objstr[0] = '\0'; 
   perfline->obj_id = -1;         
   perfline->req_id = -1;         
   perfline->method_id = -1;      
   perfline->fnctstr[0] = '\0'; 
   perfline->type = -1;           
   perfline->ignore_line = -1;    
   perfline->time = -999999.0;    

   return 0; 
}

/********************************************************************************************************************************/
struct s_object{
   char objstr[MAXLINE];   /* currently "emethod" or "timer" */
   int  obj_id;            /* id of object */
   int  idx_range[2];      /* range of method ids */
   int  winnerfound;       /* flag */
   int  meas[2];           /* measurements until and after winner is found */ 
}; 
typedef struct s_object OBJECT;

/********************************************************************************************************************************/
int minmax_init_object( OBJECT* object ) {
/********************************************************************************************************************************/
   object->objstr[0] = '\0';
   object->obj_id = -1;
   object->idx_range[0] = 9999;
   object->idx_range[1] = -1;
   object->winnerfound = 0;
   object->meas[0] = 0; 
   object->meas[1] = 0; 

   return 0; 
}; 

/********************************************************************************************************************************/
struct s_commentline { /* informations of lines starting with # */
   int ignore_pattern[3];      
   char ignore_nn_str[MAXLINE];
   char ignore_allreduce_str[MAXLINE];
   char ignore_allgatherv_str[MAXLINE];
   int  numtests;
   int  nobjects;         /* number of different objects */
   OBJECT objects[MAXOBJ];     /* object is identified by an object string and a number */
};
typedef struct s_commentline COMMLINE;

/********************************************************************************************************************************/
int minmax_init_commentline ( COMMLINE* commentline ) {
/********************************************************************************************************************************/
   int i; 
   for ( i=0; i<3; i++) {
     commentline->ignore_pattern[i] = 0; 
   }
   commentline->ignore_nn_str[0] = '\0';
   commentline->ignore_allreduce_str[0] = '\0'; 
   commentline->ignore_allgatherv_str[0] = '\0'; 
   commentline->numtests = -1;
   commentline->nobjects  = 0;     
   for ( i=0; i<MAXOBJ; i++) {
      minmax_init_object ( &commentline->objects[i] );
   }

   return 0; 
};

/********************************************************************************************************************************/
void minmax_read_perfline_all( char* line, PERFLINE* perfline, 
   COMMLINE* commentline ) ;
int minmax_read_commentline ( char* line, COMMLINE* commentline) ;
int minmax_get_lines (char *filename); 
int minmax_get_object ( char* objstr, int* obj_id, int type, COMMLINE* commentline, OBJECT** object ); 
void minmax_print_info ( COMMLINE *commentline );
void minmax_print_object ( OBJECT* object, int idx);

 
//void minmax_parse_file ( struct emethod ***emethods )
/**********************************************************************/
int main (int argc, char **argv )
/* reads *.out files and inits em_time and em_avg */
{
    
    FILE *infd=NULL;
    int nlines=-1, nperflines = 0; 
    int ret, i;
    COMMLINE commentline; 
    PERFLINE* perfline = NULL; 
    char line[MAXLINE];
    //int proc, obj_id, req_id, method_id, type, ignore_line, ignore_type, numtest;
    //double time;

    /* Determine #lines in 0.out */
    nlines = minmax_get_lines("0.out");
    if ( 0 > nlines ) {
       printf("Negative number of lines in 0.out");
       exit(-1);
    }

    minmax_init_commentline( &commentline );
    perfline = ( PERFLINE* ) malloc ( nlines * sizeof (PERFLINE) ); 
    nperflines = 0; 

    /* Parse 0.out */
    infd = fopen ( "0.out", "r" );
    for (i=0; i<nlines; i++) {
       ret = fscanf ( infd, "%[^\n]\n", line );
       if ( line[0] == '#') { 
          minmax_read_commentline ( line, &commentline );
       }    
       else { 
          minmax_init_perfline( &perfline[nperflines] );
          minmax_read_perfline_all( line, &perfline[i], &commentline ); 
          //printf("%d: %s %d request %d method %d (%s) time %lf, type=%d, ignore=%d\n", proc, objstr, obj_id,
          //   req_id, method_id, fnctstr, time, type, ignore_line);
          nperflines++;
       }
    }

    minmax_print_info ( &commentline ); 


 //exit:
    if ( NULL != infd ) fclose ( infd );

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
        printf("Could not open input file 0.out for reading\n");
        exit (-1);
    }
    fscanf ( infd, "%[^\n]\n", line );
    sscanf ( line, "%d", &nlines );
    system("rm -f lines.out");
    printf("Detected 0.out with %d lines\n", nlines);

    return nlines;
}


/********************************************************************************************************************************/
void minmax_read_perfline_all( char* line, PERFLINE* perfline, COMMLINE* commentline ) {
//int* proc, char* objstr, int *obj_id,  
//   int* req_id, int* method_id, char* fnctstr, int* type, int* ignore_line, double* time ){
/* reads one line of out-file                                         */
/********************************************************************************************************************************/
   char str[MAXLINE], colon[1];
   char *basestr;
   OBJECT* object;

   printf("line %s\n", line);
   
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
      perfline->type = 1;
   }
   else if ( NULL != strstr ( perfline->fnctstr, "AllGatherV") ) {
      perfline->type = 2;
   }
   else { 
      perfline->type = 0;
   }

   if ( commentline->ignore_pattern[perfline->type] ) {
      perfline->ignore_line = 1; 
   }
   else {
      perfline->ignore_line = 0;
   }

   basestr = strstr ( line, ")" );
   sscanf ( basestr, "%1s %lf\n", str, &perfline->time );

   printf("%d: %10s %d request %d method %d (%s) time %lf, type=%d, ignore=%d\n", 
      perfline->proc, perfline->objstr, perfline->obj_id, perfline->req_id, perfline->method_id, 
      perfline->fnctstr, perfline->time, perfline->type, perfline->ignore_line);

   minmax_get_object ( perfline->objstr, &perfline->obj_id, perfline->type, commentline, &object ); 
   /* update range */
   if ( perfline->method_id < object->idx_range[0]) 
      object->idx_range[0] = perfline->method_id;
   if ( perfline->method_id > object->idx_range[1]) 
      object->idx_range[1] = perfline->method_id;

   if ( object->winnerfound ) 
      object->meas[1]++;
   else
      object->meas[0]++; 
}

/********************************************************************************************************************************/
int minmax_get_object ( char* objstr, int* obj_id, int type, COMMLINE* commentline, OBJECT** object ){ 
/********************************************************************************************************************************/
   int i; 
   OBJECT* tobject;

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
   strcpy ( tobject->objstr, objstr );
   tobject->obj_id = *obj_id;
   if ( commentline->ignore_pattern[type] == 1 ) {
      tobject->winnerfound = 1;
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
      sscanf ( basestr, "%s %s", colon, commentline->ignore_nn_str );
      printf("Ignoring function %s\n", commentline->ignore_nn_str);
   }
   else if ( NULL != strstr( line, "#ADCL_EMETHOD_ALLREDUCE_SELECTION" ) ) {
      commentline->ignore_pattern[1] = 1;
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%s %s", colon, commentline->ignore_allreduce_str );
      printf("Ignoring function %s\n", commentline->ignore_allreduce_str);
   }
   else if ( NULL != strstr( line, "#ADCL_EMETHOD_ALLGATHERV_SELECTION" ) ) {
      commentline->ignore_pattern[2] = 1;
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%s %s", colon, commentline->ignore_allgatherv_str );
      printf("Ignoring function %s\n", commentline->ignore_allgatherv_str);
   }  
   else if ( NULL != strstr( line, "#ADCL_EMETHOD_NUMTESTS" ) ) {
      basestr = strstr ( line, ":" );
      sscanf ( basestr, "%1s %d", colon, &commentline->numtests );
      printf("Numtests =  %d\n", commentline->numtests);
   }  
   else if ( NULL != strstr ( line, "winner is") ) { 
       basestr = strstr ( line, ":" );
       sscanf ( basestr, "%1s %s %d", colon, objstr, &obj_id );
       minmax_get_object ( objstr, &obj_id, 1, commentline, &object ); 
       object->winnerfound = 1; 
   }

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

   printf("Summary information for %d objects\n", commentline->nobjects);
   for ( i=0; i<commentline->nobjects; i++) {
       minmax_print_object ( &commentline->objects[i], i); 
   }

   if ( commentline->ignore_pattern[0] == 1 ) {
      printf("Ignoring Next-Neighbor Communications: set to %s\n", commentline->ignore_nn_str); 
   }
   else if ( commentline->ignore_pattern[1] == 1 ) {
      printf("Ignoring Allreduce Communications: set to %s\n", commentline->ignore_allreduce_str); 
   }
   else if ( commentline->ignore_pattern[2] == 1 ) {
      printf("Ignoring AllGatherV Communications: set to %s\n", commentline->ignore_allgatherv_str); 
   }
}


