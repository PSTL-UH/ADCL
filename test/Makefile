include ../Makefile.defs

all: first second

first: 	first.c
	$(CC) $(CFLAGS) -o first first.c $(LDFLAGS) $(ADCL_LIB)
	mv first $(ADCL_BIN_DIR)

second:	second.c
	$(CC) $(CFLAGS) -o second second.c $(LDFLAGS) $(ADCL_LIB)
	mv second $(ADCL_BIN_DIR)

clean:
	rm -rf *.o *~
	rm -f $(ADCL_BIN_DIR)first $(ADCL_BIN_DIR)second
