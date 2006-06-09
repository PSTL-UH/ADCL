include ../../Makefile.defs


OBJ =  	ADCL.o		        \
	ADCL_Vector.o           \
	ADCL_Request.o           


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



$(ADCL_LIB): $(HEADER) $(OBJ)
	$(AR) $(AR_FLAGS) $(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
