/*System headers*/
#include <stdio.h>
#include <stdlib.h>

/* Project headers */

#include "ATF.h"
#include "ATF_Display_Result.h"
#include "ATF_Memory.h"
#include "ADCL.h"
#include "ATF_Adcl_global.h"

int main(int argc, char **argv)
{
    /* problem number, solver number, pattern numbers*/
    int nproblem,nsolver;

    /* maximum problem numbers, solver numbers and pattern numbers*/
    int MaxProblem, MaxSolver;
    
    /* problem number on x, y and z coordinate*/
    int px, py, pz;

    /*Temporary variables for solver and pattern */
    int solv;
    int pat;
   
    int size, rank;

    MaxProblem = 0;
    MaxSolver = 0;
    
    
    MPI_Init(&argc, &argv);
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    ADCL_Init();
    /*Read configure information from System.conf */
    if ( !ATF_Read_config(&MaxProblem, &MaxSolver)){
	fprintf(stderr, "%d: Error in read config@System_Read_config\n", rank);
	MPI_Abort ( MPI_COMM_WORLD, ATF_ERROR );
    }
    
    if (!ATF_Init()){
	fprintf(stderr, "%d: Error in Init@ATF_Init", rank);
    }
    
    /* Loop over problem size */

    for ( nproblem = 0; nproblem < MaxProblem; nproblem++){
	
        if( !ATF_Get_problemsize (nproblem, &px, &py, &pz)){
	    fprintf( stderr,"%d,Error in System_Get_problemsize",rank);
	}
	
	/*Nearly ok!*/
	if( !ATF_Init_matrix ( px, py, pz)){
	    fprintf(stderr, "%d,Error in ATF_Init_matrix", rank);
	}
	
        /* Loop over solvers */

	for( nsolver=0; nsolver < MaxSolver; nsolver++){
	    
            ATF_Get_solver(nsolver, &solv);
	    
/*...........Loop over communication patterns*/
/*	    for( npattern = 0; npattern < MaxPattern; npattern++){ */
		
		
/*..............Reset solution vectors*/
            ATF_Reset_dq ();
		
/*..............Preconditioning*/
            ATF_Precon ( );
		
/*..............Call the solvers*/
            ATF_Solver ( solv);
		
/*..............Display results*/
            ATF_Display_Result ();
	
	    
	}
        ATF_Free_matrix();
    }

    ADCL_Finalize();
    MPI_Finalize();
    return ATF_SUCCESS;
}

