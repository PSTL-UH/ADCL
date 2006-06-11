include ../../Makefile.defs


OBJ =	ADCL_memory.o 		\
	ADCL_memory_backend_double.o \
	ADCL_memory_backend_float.o  \
	ADCL_memory_backend_int.o 


HEADER = $(ADCL_INC_DIR)/ADCL.h           \
	 $(ADCL_INC_DIR)/ADCL_internal.h  \
	 $(ADCL_INC_DIR)/ADCL_method.h    \
	 $(ADCL_INC_DIR)/ADCL_emethod.h   \
	 $(ADCL_INC_DIR)/ADCL_request.h   \
	 $(ADCL_INC_DIR)/ADCL_vector.h    \
	 $(ADCL_INC_DIR)/ADCL_memory.h    \
	 $(ADCL_INC_DIR)/ADCL_request.h   \
	 $(ADCL_INC_DIR)/ADCL_subarray.h 


.SUFFIXES: .o.c

all:    
	make $(ADCL_LIB) 


$(ADCL_LIB): $(HEADER) $(OBJ) memory
	$(AR) $(AR_FLAGS) $(ADCL_LIB_DIR)/$(ADCL_LIB) $(OBJ)

memory:	$(HEADER) 
	$(CC) $(CFLAGS) -c ADCL_memory_backend.c -DTYPEMODE=1 \
	                -o ADCL_memory_backend_double.o
	$(CC) $(CFLAGS) -c ADCL_memory_backend.c -DTYPEMODE=2   \
	                -o ADCL_memory_backend_float.o
	$(CC) $(CFLAGS) -c ADCL_memory_backend.c -DTYPEMODE=3   \
	                -o ADCL_memory_backend_int.o

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
