include ../../../Makefile.defs

OBJ =  	ADCL.o		        \
	ADCL_Vector.o           \
	ADCL_Topology.o         \
	ADCL_Request.o           


.SUFFIXES: .o.c

all:    $(HEADER) $(OBJ)
	$(AR) $(AR_FLAGS) $(ADCL_LIB_DIR)/$(ADCL_LIB) $(OBJ)


$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
