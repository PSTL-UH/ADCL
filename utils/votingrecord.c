#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXLINE 120
static char rest[MAXLINE];

int getproc ( char *line );
int getmethod ( char *line );
int getmethod_assign ( char *line, int *emethod );
int getnumoutliers ( char *line );
double getvote ( char *line );

int allocate_3D_double_matrix (double ****matrix,int dims[3]);
int allocate_3D_int_matrix (int ****matrix,int dims[3]);

int main (int argc, char **argv )
{
    FILE *infd=NULL,  *outfd=NULL;
    int numprocs=-1, numrequests=-1,nummethods=-1;
    int proc, numoutliers, method;
    double vote;

    int i, j, k;
    int voterequest, dims[3];
    char inname[50], outname[50];
    char line[MAXLINE];
    char *basestr;
    char basefname[50];

    int ***outliers=NULL;
    double ***votes=NULL;

    if (argc < 5 )
    {
	printf(" Usage : votingrecord <numprocs> <numrequests> <nummethods> <outfilename> \n\n");
	printf(" This program determines the overall voting record \n"
	       " per proc and method using ADCL output files.\n");
	exit ( 1 ) ;
    }
    
    numprocs = atoi( argv[1] );
    numrequests = atoi( argv[2] );
    nummethods = atoi( argv[3] );
    strcpy ( basefname, argv[4] );

    dims[0] = numprocs;
    dims[1] = numrequests;
    dims[2] = nummethods;

    allocate_3D_double_matrix ( &votes, dims );
    allocate_3D_int_matrix ( &outliers, dims );

    tmpoutliers = ( int *) malloc ( nummethods * sizeof (int ));

    for ( j = 0; j < numprocs; j++ ) {
	sprintf(inname, "%d.out", j);
	infd = fopen ( inname, "r" );
	

	/* Read infile and store the values in the according list */
	while ( fscanf ( infd, "%[^\n]\n", line ) != EOF ) 
	{
	    if ( line[0] != '#' ) {
		/* Skip non-comment lines */
		continue;
	    }
	    
	    /*read the parameters of the run */
	    basestr = strstr ( line, "num. of outliers" );
	    if ( NULL != basestr ) {
		proc = getproc(line);
		method = getmethod(line);
		numoutliers = getnumoutliers ( line);
		tmpoutliers[method] = numoutliers;
	    }

	    basestr = strstr ( line, "assigning" );
	    if ( NULL != basestr ) {
		proc = getproc(line);
		method = getmethod_assign(line, &emethod );
		vote = getvote ( line);
		if ( votes[proc][voterequest][method] != 0 )  
		    voterequest++;
		votes[proc][voterequest][method] = vote;
	    }
	}

	fclose ( infd );
    }

    sprintf(outname, "%s.rec", basefname );
    outfd = fopen ( outname, "w" );
    if ( NULL == outfd ) {
	exit ( -1);
    }
    
    for ( j = 0; j < numrequests; j ++ ) {
	fprintf(outfd, "Voting behaviour for request %d\n", j);
	fprintf(outfd, "=================================\n");
	
	for ( i=0; i< numprocs; i++ ) {
	    fprintf(outfd, "%d: ", i);
	    for ( k=0; k< nummethods; k++ ) {
		fprintf(outfd, " %5.3lf", votes[i][j][k] );
	    }
	    fprintf(outfd, "\n");
	}
	fprintf(outfd, "\n");
    }

    for ( j = 0; j < numrequests; j ++ ) {
	fprintf(outfd, "Outlier behaviour for request %d\n", j);
	fprintf(outfd, "=================================\n");
	
	for ( i=0; i< numprocs; i++ ) {
	    fprintf(outfd, "%d: ", i);
	    for ( k=0; k< nummethods; k++ ) {
		fprintf(outfd, " %d", outliers[i][j][k] );
	    }
	    fprintf(outfd, "\n");
	}
	fprintf(outfd, "\n");
    }

    fclose ( outfd );
    return ( 0 );
}

int getproc ( char *line )
{
    int proc=-1;
    sscanf (line, "#%d:%s\n", &proc, rest);
/*    printf("proc is %d\n", proc ); */

    return proc;
}

int getmethod ( char *line )
{
    int method=-1;
    char *start=NULL;
    start = strstr(line, ":");
    sscanf (start, ": method %d%s\n", &method, rest);
/*    printf("emethod = %d\n", emethod ); */

    return method;
}

int getmethod_assign ( char *line, int *emethod )
{
    int  method=-1;
    char *start=NULL;
    start = strstr(line, "emethod");

    sscanf (start, "emethod %d method %d\n", &emethod, &method);
/*    printf("emethod = %d method = %d\n", emethod, method );  */

    return method;
}

int getnumoutliers ( char *line )
{
    int numoutliers=-1;
    char *start=NULL;
    start = strstr(line, "num. of outliers");

    sscanf (start, "num. of outliers %d%s\n", &numoutliers, rest);
/*    printf ("num. of outliers = %d\n", numoutliers ); */

    return numoutliers;
}

double getvote ( char *line ) 
{
    double vote=-1;
    char *start=NULL;
    start = strstr(line, ":");
    sscanf (start, ": assigning %lf%s\n", &vote, rest);
/*    printf("emethod = %lf\n", vote ); */

    return vote;
}



int allocate_3D_double_matrix (double ****matrix,int dims[3])
{
  int i;
  double ***tmp_field1;
  double **tmp_field0;
  double *data;
  
  data = (double *) malloc(dims[0]*dims[1]*dims[2]*sizeof(double));
  if (data == NULL ) {
      printf("%d @ %s, Could not allocate memory\n",  __LINE__,__FILE__);
      return 1;
  }
  
  tmp_field0 = (double **) malloc (dims[0]*dims[1]*sizeof(double *));
  if (tmp_field0 == NULL ) {
      printf("%d @ %s, Could not allocate memory\n",  __LINE__,__FILE__);
      return 1;
  }
  for (i=0; i<(dims[0]*dims[1]); i++) {
    tmp_field0[i] = &(data[i*dims[2]]);
  }
  tmp_field1 = (double ***) malloc (dims[0]*sizeof(double **));
  if (tmp_field1 == NULL ) {
      printf("%d @ %s, Could not allocate memory\n",  __LINE__,__FILE__);
      return 1;
  }
  for (i=0; i<dims[0]; i++) {
    tmp_field1[i] = &(tmp_field0[i*dims[1]]);
  }
  
  *matrix=tmp_field1;
  return 0;
}


int allocate_3D_int_matrix (int ****matrix,int dims[3])
{
  int i;
  int ***tmp_field1;
  int **tmp_field0;
  int *data;
  
  data = (int *) malloc(dims[0]*dims[1]*dims[2]*sizeof(int));
  if (data == NULL ) {
      printf("%d @ %s, Could not allocate memory\n",  __LINE__,__FILE__);
      return 1;
  }
  
  tmp_field0 = (int **) malloc (dims[0]*dims[1]*sizeof(int *));
  if (tmp_field0 == NULL ) {
      printf("%d @ %s, Could not allocate memory\n",  __LINE__,__FILE__);
      return 1;
  }
  for (i=0; i<(dims[0]*dims[1]); i++) {
    tmp_field0[i] = &(data[i*dims[2]]);
  }
  tmp_field1 = (int ***) malloc (dims[0]*sizeof(int **));
  if (tmp_field1 == NULL ) {
      printf("%d @ %s, Could not allocate memory\n",  __LINE__,__FILE__);
      return 1;
  }
  for (i=0; i<dims[0]; i++) {
    tmp_field1[i] = &(tmp_field0[i*dims[1]]);
  }
  
  *matrix=tmp_field1;
  return 0;
}
