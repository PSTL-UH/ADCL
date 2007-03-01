include ../../Makefile.defs


OBJ = 	ADCL_array.o 		\
	ADCL_attribute.o	\
	ADCL_vector.o           \
	ADCL_function.o		\
	ADCL_indexed.o          \
	ADCL_packunpack.o       \
	ADCL_printf.o		\
	ADCL_readenv.o          \
	ADCL_statistics.o       \
	ADCL_subarray.o         \
	ADCL_topology.o         \
	ADCL_emethod.o 		\
	ADCL_hypothesis.o       \
	ADCL_request.o 		

.SUFFIXES: .o.c

all:    
	make $(ADCL_LIB) 


$(ADCL_LIB): $(HEADER) $(OBJ) 
	$(AR) $(AR_FLAGS) $(ADCL_LIB_DIR)/$(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
