#include <stdio.h> 
#include <unistd.h> 
#include <mpi.h> 
#include "ADCL.h" 

void test_func_0 ( ADCL_Request req );
#define TIME0 0
void test_func_1 ( ADCL_Request req );
#define TIME1 1000
void test_func_2 ( ADCL_Request req );
#define TIME2 2000
void test_func_3 ( ADCL_Request req );
#define TIME3 3000
void test_func_4 ( ADCL_Request req );
#define TIME4 4000
void test_func_5 ( ADCL_Request req );
#define TIME5 5000
void test_func_6 ( ADCL_Request req );
#define TIME6 6000
void test_func_7 ( ADCL_Request req );
#define TIME7 7000
void test_func_8 ( ADCL_Request req );
#define TIME8 8000
void test_func_9 ( ADCL_Request req );
#define TIME9 9000
void test_func_10 ( ADCL_Request req );
#define TIME10 10000
void test_func_11 ( ADCL_Request req );
#define TIME11 11000
void test_func_12 ( ADCL_Request req );
#define TIME12 12000
void test_func_13 ( ADCL_Request req );
#define TIME13 13000
void test_func_14 ( ADCL_Request req );
#define TIME14 14000
void test_func_15 ( ADCL_Request req );
#define TIME15 15000
void test_func_16 ( ADCL_Request req );
#define TIME16 16000
void test_func_17 ( ADCL_Request req );
#define TIME17 17000
void test_func_18 ( ADCL_Request req );
#define TIME18 18000
void test_func_19 ( ADCL_Request req );
#define TIME19 19000
void test_func_20 ( ADCL_Request req );
#define TIME20 20000
void test_func_21 ( ADCL_Request req );
#define TIME21 21000
void test_func_22 ( ADCL_Request req );
#define TIME22 22000
void test_func_23 ( ADCL_Request req );
#define TIME23 23000
void test_func_24 ( ADCL_Request req );
#define TIME24 24000
void test_func_25 ( ADCL_Request req );
#define TIME25 25000
void test_func_26 ( ADCL_Request req );
#define TIME26 26000
void test_func_27 ( ADCL_Request req );
#define TIME27 27000
void test_func_28 ( ADCL_Request req );
#define TIME28 28000
void test_func_29 ( ADCL_Request req );
#define TIME29 29000
void test_func_30 ( ADCL_Request req );
#define TIME30 30000
void test_func_31 ( ADCL_Request req );
#define TIME31 31000
void test_func_32 ( ADCL_Request req );
#define TIME32 32000
void test_func_33 ( ADCL_Request req );
#define TIME33 33000
void test_func_34 ( ADCL_Request req );
#define TIME34 34000
void test_func_35 ( ADCL_Request req );
#define TIME35 35000
void test_func_36 ( ADCL_Request req );
#define TIME36 36000
void test_func_37 ( ADCL_Request req );
#define TIME37 37000
void test_func_38 ( ADCL_Request req );
#define TIME38 38000
void test_func_39 ( ADCL_Request req );
#define TIME39 39000
void test_func_40 ( ADCL_Request req );
#define TIME40 40000
void test_func_41 ( ADCL_Request req );
#define TIME41 41000
void test_func_42 ( ADCL_Request req );
#define TIME42 42000
void test_func_43 ( ADCL_Request req );
#define TIME43 43000
void test_func_44 ( ADCL_Request req );
#define TIME44 44000
void test_func_45 ( ADCL_Request req );
#define TIME45 45000
void test_func_46 ( ADCL_Request req );
#define TIME46 46000
void test_func_47 ( ADCL_Request req );
#define TIME47 47000

int main ( int argc, char ** argv )
{
    ADCL_Topology topo;
    ADCL_Request req; 
    ADCL_Attrset attrset;
    ADCL_Fnctset fnctset;
    ADCL_Attribute attrs[3];
    ADCL_Function funcs[48];
    int rank, size, i;
    int numfuncs=48, numattrs=3;
    int attrvals[6];
    int funcvals[3];
    

    MPI_Init ( &argc, &argv );
    MPI_Comm_rank ( MPI_COMM_WORLD,  &rank );
    MPI_Comm_size ( MPI_COMM_WORLD,  &size );
    ADCL_Init ( );
    ADCL_Topology_create_generic ( 0, NULL, NULL, NULL, ADCL_DIRECTION_BOTH,  MPI_COMM_WORLD, &topo ); 

    /* Generate attribute 0 */
    attrvals[0]=0;
    attrvals[1]=1;
    ADCL_Attribute_create ( 2, attrvals, NULL, NULL, &(attrs[0]));

    /* Generate attribute 1 */
    attrvals[0]=2;
    attrvals[1]=3;
    attrvals[2]=4;
    attrvals[3]=5;
    ADCL_Attribute_create ( 4, attrvals, NULL, NULL, &(attrs[1]));

    /* Generate attribute 2 */
    attrvals[0]=6;
    attrvals[1]=7;
    attrvals[2]=8;
    attrvals[3]=9;
    attrvals[4]=10;
    attrvals[5]=11;
    ADCL_Attribute_create ( 6, attrvals, NULL, NULL, &(attrs[2]));

    /* Create the attribute set */
    ADCL_Attrset_create ( 3, attrs, &attrset );

    /* Generating ADCL function 0 */
    funcvals[0] = 0;
    funcvals[1] = 2;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_0,  attrset, funcvals, "test_func_0", &(funcs[0]));

    /* Generating ADCL function 1 */
    funcvals[0] = 1;
    funcvals[1] = 2;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_1,  attrset, funcvals, "test_func_1", &(funcs[1]));

    /* Generating ADCL function 2 */
    funcvals[0] = 0;
    funcvals[1] = 3;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_2,  attrset, funcvals, "test_func_2", &(funcs[2]));

    /* Generating ADCL function 3 */
    funcvals[0] = 1;
    funcvals[1] = 3;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_3,  attrset, funcvals, "test_func_3", &(funcs[3]));

    /* Generating ADCL function 4 */
    funcvals[0] = 0;
    funcvals[1] = 4;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_4,  attrset, funcvals, "test_func_4", &(funcs[4]));

    /* Generating ADCL function 5 */
    funcvals[0] = 1;
    funcvals[1] = 4;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_5,  attrset, funcvals, "test_func_5", &(funcs[5]));

    /* Generating ADCL function 6 */
    funcvals[0] = 0;
    funcvals[1] = 5;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_6,  attrset, funcvals, "test_func_6", &(funcs[6]));

    /* Generating ADCL function 7 */
    funcvals[0] = 1;
    funcvals[1] = 5;
    funcvals[2] = 6;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_7,  attrset, funcvals, "test_func_7", &(funcs[7]));

    /* Generating ADCL function 8 */
    funcvals[0] = 0;
    funcvals[1] = 2;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_8,  attrset, funcvals, "test_func_8", &(funcs[8]));

    /* Generating ADCL function 9 */
    funcvals[0] = 1;
    funcvals[1] = 2;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_9,  attrset, funcvals, "test_func_9", &(funcs[9]));

    /* Generating ADCL function 10 */
    funcvals[0] = 0;
    funcvals[1] = 3;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_10,  attrset, funcvals, "test_func_10", &(funcs[10]));

    /* Generating ADCL function 11 */
    funcvals[0] = 1;
    funcvals[1] = 3;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_11,  attrset, funcvals, "test_func_11", &(funcs[11]));

    /* Generating ADCL function 12 */
    funcvals[0] = 0;
    funcvals[1] = 4;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_12,  attrset, funcvals, "test_func_12", &(funcs[12]));

    /* Generating ADCL function 13 */
    funcvals[0] = 1;
    funcvals[1] = 4;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_13,  attrset, funcvals, "test_func_13", &(funcs[13]));

    /* Generating ADCL function 14 */
    funcvals[0] = 0;
    funcvals[1] = 5;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_14,  attrset, funcvals, "test_func_14", &(funcs[14]));

    /* Generating ADCL function 15 */
    funcvals[0] = 1;
    funcvals[1] = 5;
    funcvals[2] = 7;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_15,  attrset, funcvals, "test_func_15", &(funcs[15]));

    /* Generating ADCL function 16 */
    funcvals[0] = 0;
    funcvals[1] = 2;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_16,  attrset, funcvals, "test_func_16", &(funcs[16]));

    /* Generating ADCL function 17 */
    funcvals[0] = 1;
    funcvals[1] = 2;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_17,  attrset, funcvals, "test_func_17", &(funcs[17]));

    /* Generating ADCL function 18 */
    funcvals[0] = 0;
    funcvals[1] = 3;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_18,  attrset, funcvals, "test_func_18", &(funcs[18]));

    /* Generating ADCL function 19 */
    funcvals[0] = 1;
    funcvals[1] = 3;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_19,  attrset, funcvals, "test_func_19", &(funcs[19]));

    /* Generating ADCL function 20 */
    funcvals[0] = 0;
    funcvals[1] = 4;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_20,  attrset, funcvals, "test_func_20", &(funcs[20]));

    /* Generating ADCL function 21 */
    funcvals[0] = 1;
    funcvals[1] = 4;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_21,  attrset, funcvals, "test_func_21", &(funcs[21]));

    /* Generating ADCL function 22 */
    funcvals[0] = 0;
    funcvals[1] = 5;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_22,  attrset, funcvals, "test_func_22", &(funcs[22]));

    /* Generating ADCL function 23 */
    funcvals[0] = 1;
    funcvals[1] = 5;
    funcvals[2] = 8;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_23,  attrset, funcvals, "test_func_23", &(funcs[23]));

    /* Generating ADCL function 24 */
    funcvals[0] = 0;
    funcvals[1] = 2;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_24,  attrset, funcvals, "test_func_24", &(funcs[24]));

    /* Generating ADCL function 25 */
    funcvals[0] = 1;
    funcvals[1] = 2;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_25,  attrset, funcvals, "test_func_25", &(funcs[25]));

    /* Generating ADCL function 26 */
    funcvals[0] = 0;
    funcvals[1] = 3;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_26,  attrset, funcvals, "test_func_26", &(funcs[26]));

    /* Generating ADCL function 27 */
    funcvals[0] = 1;
    funcvals[1] = 3;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_27,  attrset, funcvals, "test_func_27", &(funcs[27]));

    /* Generating ADCL function 28 */
    funcvals[0] = 0;
    funcvals[1] = 4;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_28,  attrset, funcvals, "test_func_28", &(funcs[28]));

    /* Generating ADCL function 29 */
    funcvals[0] = 1;
    funcvals[1] = 4;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_29,  attrset, funcvals, "test_func_29", &(funcs[29]));

    /* Generating ADCL function 30 */
    funcvals[0] = 0;
    funcvals[1] = 5;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_30,  attrset, funcvals, "test_func_30", &(funcs[30]));

    /* Generating ADCL function 31 */
    funcvals[0] = 1;
    funcvals[1] = 5;
    funcvals[2] = 9;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_31,  attrset, funcvals, "test_func_31", &(funcs[31]));

    /* Generating ADCL function 32 */
    funcvals[0] = 0;
    funcvals[1] = 2;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_32,  attrset, funcvals, "test_func_32", &(funcs[32]));

    /* Generating ADCL function 33 */
    funcvals[0] = 1;
    funcvals[1] = 2;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_33,  attrset, funcvals, "test_func_33", &(funcs[33]));

    /* Generating ADCL function 34 */
    funcvals[0] = 0;
    funcvals[1] = 3;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_34,  attrset, funcvals, "test_func_34", &(funcs[34]));

    /* Generating ADCL function 35 */
    funcvals[0] = 1;
    funcvals[1] = 3;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_35,  attrset, funcvals, "test_func_35", &(funcs[35]));

    /* Generating ADCL function 36 */
    funcvals[0] = 0;
    funcvals[1] = 4;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_36,  attrset, funcvals, "test_func_36", &(funcs[36]));

    /* Generating ADCL function 37 */
    funcvals[0] = 1;
    funcvals[1] = 4;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_37,  attrset, funcvals, "test_func_37", &(funcs[37]));

    /* Generating ADCL function 38 */
    funcvals[0] = 0;
    funcvals[1] = 5;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_38,  attrset, funcvals, "test_func_38", &(funcs[38]));

    /* Generating ADCL function 39 */
    funcvals[0] = 1;
    funcvals[1] = 5;
    funcvals[2] = 10;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_39,  attrset, funcvals, "test_func_39", &(funcs[39]));

    /* Generating ADCL function 40 */
    funcvals[0] = 0;
    funcvals[1] = 2;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_40,  attrset, funcvals, "test_func_40", &(funcs[40]));

    /* Generating ADCL function 41 */
    funcvals[0] = 1;
    funcvals[1] = 2;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_41,  attrset, funcvals, "test_func_41", &(funcs[41]));

    /* Generating ADCL function 42 */
    funcvals[0] = 0;
    funcvals[1] = 3;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_42,  attrset, funcvals, "test_func_42", &(funcs[42]));

    /* Generating ADCL function 43 */
    funcvals[0] = 1;
    funcvals[1] = 3;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_43,  attrset, funcvals, "test_func_43", &(funcs[43]));

    /* Generating ADCL function 44 */
    funcvals[0] = 0;
    funcvals[1] = 4;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_44,  attrset, funcvals, "test_func_44", &(funcs[44]));

    /* Generating ADCL function 45 */
    funcvals[0] = 1;
    funcvals[1] = 4;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_45,  attrset, funcvals, "test_func_45", &(funcs[45]));

    /* Generating ADCL function 46 */
    funcvals[0] = 0;
    funcvals[1] = 5;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_46,  attrset, funcvals, "test_func_46", &(funcs[46]));

    /* Generating ADCL function 47 */
    funcvals[0] = 1;
    funcvals[1] = 5;
    funcvals[2] = 11;
    ADCL_Function_create ( (ADCL_work_fnct_ptr *)test_func_47,  attrset, funcvals, "test_func_47", &(funcs[47]));

    /* Generate the ADCL function set */
    ADCL_Fnctset_create ( 48, funcs, "trivial functions", &fnctset);

    /* Generate the ADCL request */
    ADCL_Request_create ( ADCL_VECTOR_NULL, topo, fnctset, &req );

    for ( i=0; i<480; i++ ) { 
        ADCL_Request_start ( req );
    }
    ADCL_Request_free ( &req );
    ADCL_Fnctset_free ( &fnctset );
    for (i=0; i<numfuncs; i++ ) { 
        ADCL_Function_free ( &funcs[i]);
    }
    ADCL_Attrset_free ( &attrset );
    for (i=0; i<numattrs; i++ ) { 
        ADCL_Attribute_free ( &attrs[i]);
    }
    ADCL_Topology_free ( &topo );
    ADCL_Finalize();
    MPI_Finalize();
    return 0;
}

void test_func_0 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_0 size=%d\n", rank, size);
#endif
    usleep ( TIME0 );
    return;
}

void test_func_1 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_1 size=%d\n", rank, size);
#endif
    usleep ( TIME1 );
    return;
}

void test_func_2 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_2 size=%d\n", rank, size);
#endif
    usleep ( TIME2 );
    return;
}

void test_func_3 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_3 size=%d\n", rank, size);
#endif
    usleep ( TIME3 );
    return;
}

void test_func_4 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_4 size=%d\n", rank, size);
#endif
    usleep ( TIME4 );
    return;
}

void test_func_5 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_5 size=%d\n", rank, size);
#endif
    usleep ( TIME5 );
    return;
}

void test_func_6 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_6 size=%d\n", rank, size);
#endif
    usleep ( TIME6 );
    return;
}

void test_func_7 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_7 size=%d\n", rank, size);
#endif
    usleep ( TIME7 );
    return;
}

void test_func_8 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_8 size=%d\n", rank, size);
#endif
    usleep ( TIME8 );
    return;
}

void test_func_9 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_9 size=%d\n", rank, size);
#endif
    usleep ( TIME9 );
    return;
}

void test_func_10 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_10 size=%d\n", rank, size);
#endif
    usleep ( TIME10 );
    return;
}

void test_func_11 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_11 size=%d\n", rank, size);
#endif
    usleep ( TIME11 );
    return;
}

void test_func_12 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_12 size=%d\n", rank, size);
#endif
    usleep ( TIME12 );
    return;
}

void test_func_13 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_13 size=%d\n", rank, size);
#endif
    usleep ( TIME13 );
    return;
}

void test_func_14 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_14 size=%d\n", rank, size);
#endif
    usleep ( TIME14 );
    return;
}

void test_func_15 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_15 size=%d\n", rank, size);
#endif
    usleep ( TIME15 );
    return;
}

void test_func_16 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_16 size=%d\n", rank, size);
#endif
    usleep ( TIME16 );
    return;
}

void test_func_17 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_17 size=%d\n", rank, size);
#endif
    usleep ( TIME17 );
    return;
}

void test_func_18 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_18 size=%d\n", rank, size);
#endif
    usleep ( TIME18 );
    return;
}

void test_func_19 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_19 size=%d\n", rank, size);
#endif
    usleep ( TIME19 );
    return;
}

void test_func_20 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_20 size=%d\n", rank, size);
#endif
    usleep ( TIME20 );
    return;
}

void test_func_21 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_21 size=%d\n", rank, size);
#endif
    usleep ( TIME21 );
    return;
}

void test_func_22 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_22 size=%d\n", rank, size);
#endif
    usleep ( TIME22 );
    return;
}

void test_func_23 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_23 size=%d\n", rank, size);
#endif
    usleep ( TIME23 );
    return;
}

void test_func_24 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_24 size=%d\n", rank, size);
#endif
    usleep ( TIME24 );
    return;
}

void test_func_25 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_25 size=%d\n", rank, size);
#endif
    usleep ( TIME25 );
    return;
}

void test_func_26 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_26 size=%d\n", rank, size);
#endif
    usleep ( TIME26 );
    return;
}

void test_func_27 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_27 size=%d\n", rank, size);
#endif
    usleep ( TIME27 );
    return;
}

void test_func_28 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_28 size=%d\n", rank, size);
#endif
    usleep ( TIME28 );
    return;
}

void test_func_29 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_29 size=%d\n", rank, size);
#endif
    usleep ( TIME29 );
    return;
}

void test_func_30 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_30 size=%d\n", rank, size);
#endif
    usleep ( TIME30 );
    return;
}

void test_func_31 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_31 size=%d\n", rank, size);
#endif
    usleep ( TIME31 );
    return;
}

void test_func_32 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_32 size=%d\n", rank, size);
#endif
    usleep ( TIME32 );
    return;
}

void test_func_33 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_33 size=%d\n", rank, size);
#endif
    usleep ( TIME33 );
    return;
}

void test_func_34 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_34 size=%d\n", rank, size);
#endif
    usleep ( TIME34 );
    return;
}

void test_func_35 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_35 size=%d\n", rank, size);
#endif
    usleep ( TIME35 );
    return;
}

void test_func_36 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_36 size=%d\n", rank, size);
#endif
    usleep ( TIME36 );
    return;
}

void test_func_37 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_37 size=%d\n", rank, size);
#endif
    usleep ( TIME37 );
    return;
}

void test_func_38 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_38 size=%d\n", rank, size);
#endif
    usleep ( TIME38 );
    return;
}

void test_func_39 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_39 size=%d\n", rank, size);
#endif
    usleep ( TIME39 );
    return;
}

void test_func_40 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_40 size=%d\n", rank, size);
#endif
    usleep ( TIME40 );
    return;
}

void test_func_41 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_41 size=%d\n", rank, size);
#endif
    usleep ( TIME41 );
    return;
}

void test_func_42 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_42 size=%d\n", rank, size);
#endif
    usleep ( TIME42 );
    return;
}

void test_func_43 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_43 size=%d\n", rank, size);
#endif
    usleep ( TIME43 );
    return;
}

void test_func_44 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_44 size=%d\n", rank, size);
#endif
    usleep ( TIME44 );
    return;
}

void test_func_45 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_45 size=%d\n", rank, size);
#endif
    usleep ( TIME45 );
    return;
}

void test_func_46 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_46 size=%d\n", rank, size);
#endif
    usleep ( TIME46 );
    return;
}

void test_func_47 ( ADCL_Request req )
{
    MPI_Comm comm;
    int rank, size;

    ADCL_Request_get_comm ( req, &comm, &rank, &size );
#ifdef VERBOSE
    printf(" %d: In  test_func_47 size=%d\n", rank, size);
#endif
    usleep ( TIME47 );
    return;
}
