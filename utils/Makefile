include ../Makefile.defs
LOCALCFLAGS = -O0 -g -Wall

all: eval-slurm minmax genscript gentex split attr_test_generator

split: 	split.c
	$(CC) $(LOCALCFLAGS) -o split split.c 

merge:	merge.c
	$(CC) $(LOCALCFLAGS) -o merge merge.c 

eval-slurm: eval-slurm.c
	$(CC) $(LOCALCFLAGS) -o eval-slurm eval-slurm.c 

genscript: genscript.c
	$(CC) $(LOCALCFLAGS) -o genscript genscript.c 

gentex: gentex.c
	$(CC) $(LOCALCFLAGS) -o gentex gentex.c 

minmax: minmax.c
	$(CC) $(LOCALCFLAGS) -o minmax minmax.c 

iterations: iterations.c
	$(CC) $(LOCALCFLAGS) -o iterations iterations.c 

attr_test_generator: attr_test_generator.c
	$(CC) $(LOCALCFLAGS) -o attr_test_generator attr_test_generator.c


clean:
	rm -rf *.o *~ *.out
	rm -f split merge iterations
	rm -f genscript gentex
	rm -f minmax eval-slurm

