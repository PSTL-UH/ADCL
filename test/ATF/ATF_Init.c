#include "ATF.h"

/* Global variables; */
int ATF_np[3];
int ATF_coord[3];
	
bool ATF_rand_ab;
bool ATF_rand_zu;
bool ATF_rand_sing;
bool ATF_rand_festk;
bool ATF_rand_symo;
bool ATF_rand_symu;

/* Global variables for Set_id */
int ATF_tid_iu, ATF_tid_io;
int ATF_tid_ju, ATF_tid_jo;
int ATF_tid_ku, ATF_tid_ko;


/* Global varaibles for Init */

int *ATF_sendcount;
int *ATF_recvcount;

MPI_Datatype *ATF_sendtype;
MPI_Datatype *ATF_recvtype;
MPI_Aint *ATF_senddisps;
MPI_Aint *ATF_recvdisps;

/* global variables for Init_Comm */

bool ATF_nb_rand_ab;
bool ATF_nb_rand_sing;
bool ATF_nb_rand_zu;
bool ATF_nb_rand_festk;
bool ATF_nb_rand_symo;
bool ATF_nb_rand_symu;

/* global variables for map */

int ATF_nhosts ;
int *ATF_firstranks;

/* global variables for ATF_comm */

/* MPI_Comm ATF_comm_rand_ab;
MPI_Comm ATF_comm_rand_sing;
MPI_Comm ATF_comm_rand_zu;
MPI_Comm ATF_comm_rand_festk;
MPI_Comm ATF_comm_rand_symo;
MPI_Comm ATF_comm_rand_symu;
*/

int ATF_Set_logic(void)
{
    int size, rank;
    
    int reorder=0;
    int periods[3]={0,0,0};

    MPI_Comm comm_cart;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank (MPI_COMM_WORLD, &rank );
    
    /*Create a division of processors in cartesions dimensions*/
    /*3*/
    MPI_Dims_create ( size, 3, ATF_np);
    
    if ( size != (ATF_np[0] * ATF_np[1] * ATF_np[2]) ) {
	printf("Wrong processor distributions!\n");
	return ATF_ERROR;
    }
   
    /*Makes a new communicator to which topology information has been attached*/
    MPI_Cart_create (MPI_COMM_WORLD, 3, ATF_np, periods,  reorder, &comm_cart);
    
    /*Determin process coords in cartesian topology given rank in group*/

    MPI_Cart_coords ( comm_cart, rank, 3, ATF_coord); 
    
    /*Returns the shifted source and destination ranks, given a shift direction
    ** and amount*/

    MPI_Cart_shift ( comm_cart, 0, 1, &ATF_tid_iu, &ATF_tid_io);
    MPI_Cart_shift ( comm_cart, 1, 1, &ATF_tid_ju, &ATF_tid_jo);
    MPI_Cart_shift ( comm_cart, 2, 1, &ATF_tid_ku, &ATF_tid_ko);
    
    ATF_rand_ab    =  false;
    ATF_rand_zu    =  false;
    ATF_rand_sing  =  false;
    ATF_rand_festk =  false;
    ATF_rand_symo  =  false;
    ATF_rand_symu  =  false;
    
    /* What's the meaning of these sentenses? */

    if ( MPI_PROC_NULL==ATF_tid_iu ) {
	ATF_rand_sing = true;
    }
    if ( MPI_PROC_NULL == ATF_tid_io) {
	ATF_rand_ab = true;
    }
    if ( MPI_PROC_NULL == ATF_tid_ju ) {
	ATF_rand_festk = true;
    }
    if ( MPI_PROC_NULL == ATF_tid_jo ) {
	ATF_rand_zu = true;
    }
    if ( MPI_PROC_NULL == ATF_tid_ku ) {
	ATF_rand_symu = true;
    }
    if ( MPI_PROC_NULL == ATF_tid_ko ) {
	ATF_rand_symo = true;
    }

    MPI_Comm_free ( &comm_cart);
    return ATF_SUCCESS;
}

/* int ATF_Init */

int ATF_Init( void)
{
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Set logical variables indicating whether the process is
    ** at the boundary of the domain or not
    */
    if ( !ATF_Set_logic() ){
	printf("%d: Error in set logic@ATF_Set_Logic\n", rank);
	return ATF_ERROR;
    }
    
    ATF_sendcount = (int *)malloc(size * sizeof(int));
    ATF_recvcount = (int *)malloc(size * sizeof(int));
    
    ATF_sendtype  = (MPI_Datatype *)malloc(size * sizeof(MPI_Datatype))	;
    ATF_recvtype  = (MPI_Datatype *)malloc(size * sizeof(MPI_Datatype));
    
    ATF_senddisps = (MPI_Aint *)malloc(size * sizeof(MPI_Aint));
    ATF_recvdisps = (MPI_Aint *)malloc(size * sizeof(MPI_Aint));
    
    if((ATF_sendcount== NULL) ||(ATF_recvcount== NULL) ||
       (ATF_sendtype== NULL) ||(ATF_recvtype== NULL) ||
       (ATF_senddisps== NULL)||( ATF_recvdisps== NULL)) {
	printf("%d: Error in allocating memory at %s %d\n", rank, __FILE__, __LINE__);
	return ATF_ERROR;
    }
    
#if 0    

/* ...Generate sub-communicators for topology aware algorithms */
/*    Note: we are assuming, that the number of processes */
/*    is not changing.. */
    
    if ( !ATF_Init_comm()){
	printf("%d: Error in ATF_Init_comm at %s %d\n", rank, __FILE__, __LINE__);
	return ATF_ERROR;
    }

#endif
    
    return ATF_SUCCESS;
}

#if 0 
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/* NOT NEEDED RIGHT NOW! */
/* called by ATF_Init_comm */
int ATF_map_proc_to_host ( int lrank, int *host )
{
    int i;
    int rank, size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if ( (lrank < 0) || (lrank > size )){
    	printf("%d: Error in map_proc_to_host@ATF_Init_Comm, illegal rank nunmber\n", rank);
    }
    
    if ( ATF_nhosts == 1 ){
	*host = 1;
    }
    else{
	
	*host = ATF_nhosts;
	
	for(i=1;i<=ATF_nhosts; i++){
	    if ( lrank < ATF_firstranks[i] ){
		*host = i - 1;
		break;
	    }
	}
    }
    printf("%d rank, %d lrank, %d host\n", rank,lrank, *host);
    
    return ATF_SUCCESS;
}


int ATF_comm( int *hostid)
{
    int rank;
    
    MPI_Comm_rank ( MPI_COMM_WORLD, &rank);
    
    MPI_Comm_split ( MPI_COMM_WORLD, *(hostid), rank, &ATF_comm_rand_ab);
    MPI_Comm_split ( MPI_COMM_WORLD, *(hostid+1), rank, &ATF_comm_rand_sing);
    MPI_Comm_split ( MPI_COMM_WORLD, *(hostid+2), rank, &ATF_comm_rand_zu);
    MPI_Comm_split ( MPI_COMM_WORLD, *(hostid+3), rank, &ATF_comm_rand_festk);
    MPI_Comm_split ( MPI_COMM_WORLD, *(hostid+4), rank, &ATF_comm_rand_symo);
    MPI_Comm_split ( MPI_COMM_WORLD, *(hostid+5), rank, &ATF_comm_rand_symu);
    
    return ATF_SUCCESS;
}

int ATF_Init_comm(){
    
    int neighborhost[6];
    int host, myhostid;
    
    int size, rank;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    ATF_nb_rand_ab    = false;
    ATF_nb_rand_sing  = false;
    ATF_nb_rand_zu    = false;
    ATF_nb_rand_festk = false;
    ATF_nb_rand_symo  = false;
    ATF_nb_rand_symu  = false;
    
    if ( !ATF_map_proc_to_host ( rank, &myhostid )){
	printf("%d: Error in map_proc_to_host@ATF_Init_Comm\n", rank);
	
    }
    
    neighborhost[0] = MPI_UNDEFINED;
    if ( !ATF_rand_ab ){
	ATF_map_proc_to_host ( ATF_tid_io, &host);
	if ( host != myhostid ){
	    printf("%d ,setting nb_ATF_rand_ab\n", rank);
	    ATF_nb_rand_ab = true;
	    neighborhost[0] = 1000*host+myhostid;
	}
    }
    
    
    neighborhost[1] = MPI_UNDEFINED;
    if ( !ATF_rand_sing ){
	ATF_map_proc_to_host ( ATF_tid_iu, &host);
	if ( host != myhostid ){
	    printf("%d ,setting nb_ATF_rand_sing\n", rank);
	    ATF_nb_rand_ab = true;
	    neighborhost[1] = 1000*host+myhostid;
	}
    }
    
    
    neighborhost[2] = MPI_UNDEFINED;
    if ( !ATF_rand_zu ){
	ATF_map_proc_to_host ( ATF_tid_jo, &host);
	if ( host != myhostid ){
	    printf("%d ,setting nb_ATF_rand_zu\n", rank);
	    ATF_nb_rand_zu = true;
	    neighborhost[2] = 1000*host+myhostid;
	}
    }
    
    
    neighborhost[3] = MPI_UNDEFINED;
    if ( !ATF_rand_festk ){
	ATF_map_proc_to_host ( ATF_tid_ju, &host);
	if ( host != myhostid ){
	    printf("%d ,setting nb_ATF_rand_festk\n", rank);
	    ATF_nb_rand_festk = true;
	    neighborhost[3] = 1000*host+myhostid;
	}
    }
    
    
    neighborhost[4] = MPI_UNDEFINED;
    if ( !ATF_rand_symo ){
	ATF_map_proc_to_host ( ATF_tid_ko, &host);
	if ( host != myhostid ){
	    printf("%d ,setting nb_ATF_rand_symo\n", rank);
	    ATF_nb_rand_symo = true;
	    neighborhost[4] = 1000*host+myhostid;
	}
    }
    
    
    neighborhost[5] = MPI_UNDEFINED;
    if ( !ATF_rand_symu ){
	ATF_map_proc_to_host ( ATF_tid_ku, &host);
	if ( host != myhostid ){
	    printf("%d ,setting nb_ATF_rand_symu\n", rank);
	    ATF_nb_rand_symu = true;
	    neighborhost[4] = 1000*host+myhostid;
	}
    }
    
    if(!ATF_comm( neighborhost)){
	
	printf("%d, ATF_Comm error@%s, %d!\n", rank, __FILE__, __LINE__);	
    }
    
    return ATF_SUCCESS;
}

#endif
