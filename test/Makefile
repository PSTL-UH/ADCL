include ../Makefile.defs

all: first second third fourth first_f second_f

first: 	first.c
	$(CC) $(CFLAGS) -o $(ADCL_BIN_DIR)/first first.c $(LDFLAGS) 

second:	second.c
	$(CC) $(CFLAGS) -o $(ADCL_BIN_DIR)/second second.c $(LDFLAGS)

third:	third.c
	$(CC) $(CFLAGS) -o $(ADCL_BIN_DIR)/third third.c $(LDFLAGS)

fourth:	fourth.c
	$(CC) $(CFLAGS) -o $(ADCL_BIN_DIR)/fourth fourth.c $(LDFLAGS)

first_f: first_f.f90
	$(FC) $(FFLAGS) -o $(ADCL_BIN_DIR)/first_f first_f.f90 $(FLDFLAGS)

second_f: second_f.f90
	$(FC) $(FFLAGS) -o $(ADCL_BIN_DIR)/second_f second_f.f90 $(FLDFLAGS)


clean:
	rm -rf *.o *~ *.out
	rm -f $(ADCL_BIN_DIR)/first $(ADCL_BIN_DIR)/second 
	rm -f $(ADCL_BIN_DIR)/third $(ADCL_BIN_DIR)/fourth
	rm -f $(ADCL_BIN_DIR)/first_f $(ADCL_BIN_DIR)/second_f

