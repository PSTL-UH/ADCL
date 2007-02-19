include ../../../Makefile.defs


OBJ =  	ADCL_f.o		  \
	ADCL_Attribute_f.o        \
	ADCL_Fnctgrp_f.o          \
	ADCL_Vector_f.o           \
	ADCL_Topology_f.o         \
	ADCL_Request_f.o           


.SUFFIXES: .o.c

all:    $(HEADER) $(OBJ)
	$(AR) $(AR_FLAGS) $(ADCL_LIB_DIR)/$(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
