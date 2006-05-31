include ../Makefile.defs

all: first second

first: 	first.c
	$(CC) $(CFLAGS) -o $(ADCL_BIN_DIR)/first first.c $(LDFLAGS) \
	      $(ADCL_LIB)


second:	second.c
	$(CC) $(CFLAGS) -o $(ADCL_BIN_DIR)/second second.c $(LDFLAGS) \
	      $(ADCL_LIB)

clean:
	rm -rf *.o *~
	rm -f $(ADCL_BIN_DIR)/first $(ADCL_BIN_DIR)/second
