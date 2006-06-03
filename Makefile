include Makefile.defs

all:
	cd src; make
	cd adcl/C ; make 
	cd adcl/f90 ; make 

clean:
	cd src; make clean
	rm -rf ./lib/libadcl.a

