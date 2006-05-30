include ../Makefile.defs


OBJ =  	ADCL_Main.o        	\
	ADCL.o		        \
	ADCL_vector.o           \
	ADCL_memory.o 		\
	ADCL_method.o 		\
	ADCL_emethod.o 		\
	ADCL_request.o 		\
	ADCL_subarray.o         \
	ADCL_packunpack.o       \
	ADCL_change.o


HEADER = $(ADCL_INC_DIR)/ADCL.h        \
	 $(ADCL_INC_DIR)/ADCL_method.h  \
	 $(ADCL_INC_DIR)/ADCL_emethod.h \
	 $(ADCL_INC_DIR)/ADCL_request.h \
	 $(ADCL_INC_DIR)/ADCL_vector.h  \
	 $(ADCL_INC_DIR)/ADCL_memory.h  \
	 $(ADCL_INC_DIR)/ADCL_request.h \
	 $(ADCL_INC_DIR)/ADCL_subarray.h 

ADCL_LIB = $(ADCL_LIB_DIR)/libadcl.a

.SUFFIXES: .o.c

all:
	make $(ADCL_LIB) 

$(ADCL_LIB): $(HEADER) $(OBJ)
	    $(CC) -o ADCL $(LDFLAGS) $(OBJ) 
#	$(AR) $(AR_FLAGS) $(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
	rm -f ADCL
	rm -rf $(ADCL_LIB)
