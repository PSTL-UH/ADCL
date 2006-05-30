include Makefile.defs

all:
	cd src; make

clean:
	cd src; make clean
	rm -rf ./lib/libadcl.a

