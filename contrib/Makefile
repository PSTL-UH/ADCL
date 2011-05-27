#
# Copyright (c) 2006-2007      University of Houston. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#
include ../Makefile.defs


OBJ = cluster.o svm.o 		


.SUFFIXES: .o.c.cpp

all:    
	make $(ADCL_LIB) 


$(ADCL_LIB): $(HEADER) $(OBJ) 
	$(AR) $(AR_FLAGS) $(ADCL_LIB_DIR)/$(ADCL_LIB) $(OBJ)

$(OBJ):	$(HEADER)
.c.o:	$(HEADER)
	$(CC) $(CFLAGS) -c $<

$(OBJ): $(HEADER)
.cpp.o: $(HEADER)
	$(CXX) $(CFLAGS) -c $<

clean:
	rm -rf *.o *~
