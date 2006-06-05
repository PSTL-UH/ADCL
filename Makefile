include Makefile.defs

all:
	cd src ; make
	cd adcl/C ; make 
	cd adcl/f90 ; make 

clean:
	cd src ; make clean
	cd adcl/C ; make clean
	cd adcl/f90 ; make clean
	rm -rf ./lib/libadcl.a

