include Makefile.defs

all:
	cd src ; make ; cd ..
	cd adcl/C ; make all ; cd ..
	cd adcl/f90 ; make all ; cd ..

clean:
	cd src ; make clean ; cd ..
	cd adcl/C ; make clean ; cd ..
	cd adcl/f90 ; make clean ; cd ..
	rm -rf ./lib/libadcl.a

