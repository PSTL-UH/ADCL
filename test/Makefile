#
# Copyright (c) 2006-2007      University of Houston. All rights reserved.
# Copyright (c) 2009           HLRS. All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#
include ../Makefile.defs

CEXECS = $(ADCL_BIN_DIR)/first $(ADCL_BIN_DIR)/second $(ADCL_BIN_DIR)/third $(ADCL_BIN_DIR)/fourth \
	 $(ADCL_BIN_DIR)/one-dim $(ADCL_BIN_DIR)/two-dim $(ADCL_BIN_DIR)/three-dim \
	 $(ADCL_BIN_DIR)/test-fnctset-noattrs $(ADCL_BIN_DIR)/test-fnctset-single $(ADCL_BIN_DIR)/test-fnctset-single-2 \
	 $(ADCL_BIN_DIR)/test-fnctset-allgatherv-1d  $(ADCL_BIN_DIR)/test-fnctset-alltoallv-1d \
	 $(ADCL_BIN_DIR)/test-fnctset-allreduce \
	 $(ADCL_BIN_DIR)/attrs_1 $(ADCL_BIN_DIR)/attrs_2 $(ADCL_BIN_DIR)/attrs_3 $(ADCL_BIN_DIR)/attrs_3a $(ADCL_BIN_DIR)/attrs_4 \
	 $(ADCL_BIN_DIR)/data_1 $(ADCL_BIN_DIR)/PbSizes_1D $(ADCL_BIN_DIR)/PbSizes_2D $(ADCL_BIN_DIR)/PbSizes_3D

FEXECS = $(ADCL_BIN_DIR)/first_f $(ADCL_BIN_DIR)/second_f \
	 $(ADCL_BIN_DIR)/one-dim-f $(ADCL_BIN_DIR)/two-dim-f $(ADCL_BIN_DIR)/three-dim-f $(ADCL_BIN_DIR)/three-dim-large \
	 $(ADCL_BIN_DIR)/test-fnctset-noattrs_f $(ADCL_BIN_DIR)/test-fnctset-single_f     \
	 $(ADCL_BIN_DIR)/test-fnctset-allgatherv_f $(ADCL_BIN_DIR)/test-fnctset-allreduce_f  \
	 $(ADCL_BIN_DIR)/test-prometheus

ADCL_LIB_ = $(ADCL_LIB_DIR)/$(ADCL_LIB)

.SUFFIXES: .c .h .f90

###################################################################################################

all: ctests ftests

ctests: $(CEXECS)

ftests:	$(FEXECS)


###################################################################################################
### CTESTS                                                                                      ###
###################################################################################################

$(ADCL_BIN_DIR)/first: 	first.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/second:	second.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/third:	third.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/fourth:	fourth.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/one-dim: one-dim.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/two-dim: two-dim.o aux-data-2D.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/three-dim: three-dim.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/test-fnctset-noattrs: test-fnctset-noattrs.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-single: test-fnctset-single.o 
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-single-2: test-fnctset-single-2.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-allgatherv-1d: test-fnctset-allgatherv-1d.o aux-data-1D.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-alltoallv-1d: test-fnctset-alltoallv-1d.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-allreduce: test-fnctset-allreduce.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/attrs_1: attrs_1.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/attrs_2: attrs_2.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/attrs_3: attrs_3.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/attrs_3a: attrs_3a.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/attrs_4: attrs_4.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/data_1: data_1.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/PbSizes_3D: PbSizes_3D.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/PbSizes_2D: PbSizes_2D.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(ADCL_BIN_DIR)/PbSizes_1D: PbSizes_1D.o
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

###################################################################################################
### FORTRAN TESTS                                                                               ###
###################################################################################################

$(ADCL_BIN_DIR)/first_f: first_f.o
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/second_f: second_f.o
	$(FC) $^ $(FLDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/one-dim-f: one-dim-f.o
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/two-dim-f: two-dim-f.o aux-data-2D-dp-f.o
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/three-dim-f: three-dim-f.o
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/three-dim-large: three-dim-large.o
	$(FC) $^ $(FLDFLAGS) -o $@

###################################################################################################

$(ADCL_BIN_DIR)/test-fnctset-noattrs_f:  test-fnctset-noattrs_f.o 	
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-single_f:  test-fnctset-single_f.o 	
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-allgatherv_f:  test-fnctset-allgatherv_f.o 	
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-fnctset-allreduce_f:  test-fnctset-allreduce_f.o
	$(FC) $^ $(FLDFLAGS) -o $@

$(ADCL_BIN_DIR)/test-prometheus:  test-prometheus.o
	$(FC) $^ $(FLDFLAGS) -o $@

###################################################################################################
###################################################################################################

aux-data-2D-dp-f.o: aux-data-2D-f.f90
	$(FC) $(FFLAGS) -I../include -c -DSET_DATA=set_data_2D_dp -DDUMP_VECTOR=dump_vector_2D_dp \
	      -DCHECK_DATA=check_data_2D_dp -DDATATYPE="doubleprecision" -o aux-data-2D-dp-f.o $<

aux-data-2D-int-f.o: aux-data-2D-f.f90
	$(FC) $(FFLAGS) -I../include -c -DSET_DATA=set_data_2D_int -DDUMP_VECTOR=dump_vector_2D_int \
	      -DCHECK_DATA=check_data_2D_int -DDATATYPE="integer" -o aux-data-2D-int-f.o $<

%.o: %.c $(ADCL_LIB_)
	$(CC) $(CFLAGS) -c $<

%.o: %.f90 $(ADCL_LIB_)
	$(FC) $(FFLAGS) -I../include -c $<

###################################################################################################

clean:
	rm -rf *.o *~ *.out
	rm -f $(ADCL_BIN_DIR)/*.out
	rm -f $(CEXECS) $(FEXECS)
