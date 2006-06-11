include Makefile.defs

HEADER = $(ADCL_INC_DIR)/ADCL_array.h      \
	 $(ADCL_INC_DIR)/ADCL_change.h     \
	 $(ADCL_INC_DIR)/ADCL_config.h	   \
	 $(ADCL_INC_DIR)/ADCL_emethod.h    \
	 $(ADCL_INC_DIR)/ADCL_fprototypes.h \
	 $(ADCL_INC_DIR)/ADCL.h            \
	 $(ADCL_INC_DIR)/ADCL_internal.h   \
	 $(ADCL_INC_DIR)/ADCL_memory.h     \
	 $(ADCL_INC_DIR)/ADCL_method.h     \
	 $(ADCL_INC_DIR)/ADCL_packunpack.h \
	 $(ADCL_INC_DIR)/ADCL_request.h    \
	 $(ADCL_INC_DIR)/ADCL_subarray.h   \
	 $(ADCL_INC_DIR)/ADCL_vector.h    


all:
	cd src/framework ; make 
	cd src/memory ; make
	cd src/communication ; make 
	cd src/adcl/C ; make all 
	cd src/adcl/f90 ; make all 

clean:
	cd src/framework ; make clean
	cd src/memory ; make clean
	cd src/communication ; make clean
	cd src/adcl/C ; make clean 
	cd src/adcl/f90 ; make clean 
	rm -rf *~ ./include/*~ 
	rm -rf ./lib/libadcl.a

