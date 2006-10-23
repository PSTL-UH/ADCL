#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXLINE 120

int main (int argc, char **argv )
{
    FILE **infd=NULL,  *outfd=NULL;
    int numprocs=-1, numrequests=-1;

    int i, ret;
    int all_done, no_line;

    char inname[50];
    char line[MAXLINE];
    char *basestr, reqstr[50];
    int req, method;
    double time;
    struct lininf {
	int    req;
	int    method;
	double min;
	double max;
	int    minloc;
	int    maxloc;
    };
    struct lininf tline;

    struct procinf {
	int nummin;
	int nummax;
    };
    struct procinf *tproc;


    if (argc < 3 )
    {
	printf(" Usage : minmax <numprocs> <numrequests> \n\n");
	printf(" Desc to follow \n");
	exit ( 1 ) ;
    }
    
    numprocs = atoi( argv[1] );
    numrequests = atoi ( argv[2] );

    infd = (FILE **) malloc ( numprocs * sizeof(FILE *) );
    if ( NULL == infd ) {
	exit (-1);
    }

    outfd = fopen ("minmax.out", "w");
    if ( NULL == outfd ) {
	exit ( -1 );
    }

    for ( i = 0; i < numprocs; i++ ) {
	sprintf( inname, "%d.out", i);
	infd[i] = fopen ( inname, "r" );
    }	

    tproc = (struct procinf *) calloc (1, numprocs * sizeof(struct procinf));
    if ( NULL == tproc ) {
	exit ( -1 );
    }

    /* Read infile and store the values in the according list */
    tline.req    = -1;
    tline.min    = 9999999999.99;
    tline.max    = 0;
    tline.minloc = -1;
    tline.maxloc = -1;
    
    all_done = 0;
    while ( all_done < numprocs ) {
	for ( i=0; i< numprocs; i++ ) {
	    no_line = 0;
	    while ( !no_line ) {
		ret = fscanf ( infd[i], "%[^\n]\n", line );
		if ( EOF == ret ) {
		    all_done++;
		    break;
		}
		if ( line[0] == '#' ) {
		    /* Skip comment lines */
		    continue;
		}
		no_line = 1;
	    }	    
	    
	    if ( i == 0 ) {
		/*read the parameters of the run */
		basestr = strstr ( line, "request" );
		sscanf ( basestr, "%7s %d", reqstr, &tline.req );
		
		basestr = strstr ( line, "method" );
		sscanf ( basestr, "%6s %d", reqstr, &tline.method );
		
		basestr = strstr ( line, ")" );
		sscanf ( basestr, "%1s %lf\n", reqstr, &time );
	    }
	    else {
		/*read the parameters of the run */
		basestr = strstr ( line, "request" );
		sscanf ( basestr, "%7s %d", reqstr, &req );
		if ( req != tline.req ) {
		    printf("Request mismatch at process %d \n", i);
		}
		
		basestr = strstr ( line, "method" );
		sscanf ( basestr, "%6s %d", reqstr, &method );
		if ( method != tline.method ) {
		    printf ("Method mismatch at process %d\n", i);
		}		

		basestr = strstr ( line, ")" );
		sscanf ( basestr, "%1s %lf\n", reqstr, &time );
	    }		
	    
	    if ( time < tline.min ) {
		tline.min    = time;
		tline.minloc = i;
	    }
	    if ( time > tline.max ) {
		tline.max = time;
		tline.maxloc =i;
	    }

	}
	fprintf (outfd, 
		 "%3d %3d %8.4lf %3d %8.4lf %3d\n", 
		 tline.req, 
		 tline.method, 
		 tline.min, 
		 tline.minloc,
		 tline.max, 
		 tline.maxloc );

	tproc[tline.minloc].nummin++;
	tproc[tline.maxloc].nummax++;


	/* Reset the lininf structure */
	tline.req    = -1;
	tline.min    = 9999999999.99;
	tline.max    = 0;
	tline.minloc = -1;
	tline.maxloc = -1;

    }

    for ( i=0; i< numprocs; i++ ) {
	printf("Proc %d : num of min. %d num of max %d \n", i, 
	       tproc[i].nummin, tproc[i].nummax );
    }
    

    for ( i=0; i< numprocs; i++ ) {
	fclose ( infd[i]);
    }
    fclose ( outfd );

    free ( tproc );
    free ( infd );

    return ( 0 );
}
