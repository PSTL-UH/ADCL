include ../Makefile.defs


OBJ =  	ADCL.o		        \
	ADCL_vector.o           \
	ADCL_memory.o 		\
	ADCL_memory_backend_double.o \
	ADCL_memory_backend_float.o  \
	ADCL_memory_backend_int.o    \
	ADCL_method.o 		\
	ADCL_emethod.o 		\
	ADCL_request.o 		\
	ADCL_subarray.o         \
	ADCL_packunpack.o       \
	ADCL_change.o           \
	ADCL_printf.o


HEADER = $(ADCL_INC_DIR)/ADCL.h        \
	 $(ADCL_INC_DIR)/ADCL_method.h  \
	 $(ADCL_INC_DIR)/ADCL_emethod.h \
	 $(ADCL_INC_DIR)/ADCL_request.h \
	 $(ADCL_INC_DIR)/ADCL_vector.h  \
	 $(ADCL_INC_DIR)/ADCL_memory.h  \
	 $(ADCL_INC_DIR)/ADCL_request.h \
	 $(ADCL_INC_DIR)/ADCL_subarray.h 


.SUFFIXES: .o.c

all:    
	make $(ADCL_LIB) 

memory:	$(HEADER) 
	$(CC) $(CFLAGS) -c ADCL_memory_backend.c -DTYPEMODE=1 \
	                -o ADCL_memory_backend_double.o
	$(CC) $(CFLAGS) -c ADCL_memory_backend.c -DTYPEMODE=2   \
	                -o ADCL_memory_backend_float.o
	$(CC) $(CFLAGS) -c ADCL_memory_backend.c -DTYPEMODE=3   \
	                -o ADCL_memory_backend_int.o

$(ADCL_LIB): $(HEADER) $(OBJ) memory
	$(AR) $(AR_FLAGS) $(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
	rm -f ADCL
	rm -rf $(ADCL_LIB)
