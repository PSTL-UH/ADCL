include ../Makefile.defs

all: first

first: 	first.c
	$(CC) $(CFLAGS) -o first first.c $(LDFLAGS) $(ADCL_LIB)
	mv first ../$(ADCL_BIN_DIR)
clean:
	rm -rf *.o *~
	rm -f first
