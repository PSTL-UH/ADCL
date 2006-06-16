include ../../Makefile.defs

OBJ =  	ATF_Main.o 		\
	ATF_Read_config.o 	\
	ATF_Init.o 		\
	ATF_Init_matrix.o 	\
	ATF_Memory.o 		\
	ATF_Set_Datatypes.o     \
	ATF_Get.o               \
	ATF_Set.bsp2.o          \
	ATF_Precon.o            \
	ATF_Change_fcfs.o       \
	ATF_Matmul.o            \
	ATF_Solver.o            \
	ATF_Solver_tfqmr.o      \
	ATF_tfqmr.Cal.o         \
	ATF_Display_result.o    \
	ATF_Free_matrix.o

HEADER = ../include/ATF.h

ATF_LIB = $(ATF_LIB_DIR)/libatf.a

.SUFFIXES: .o.c

all:
	make $(ATF_LIB) 

$(ATF_LIB): $(HEADER) $(OBJ)
	    $(CC) -o ATF $(LDFLAGS) $(OBJ) 
#	$(AR) $(AR_FLAGS) $(ATF_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
	rm -f ATF
	rm -rf $(ATF_LIB)
