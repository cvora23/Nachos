# use normal make for this Makefile
#
# Makefile for building user programs to run on top of Nachos
#
# Several things to be aware of:
#
#    Nachos assumes that the location of the program startup routine (the
# 	location the kernel jumps to when the program initially starts up)
#       is at location 0.  This means: start.o must be the first .o passed 
# 	to ld, in order for the routine "Start" to be loaded at location 0
#

# If you are running on SunOS 5.x machines, uncomment the next line
GCCDIR = /auto/home-scf-22/csci402/testgnu/decstation-ultrix/bin/
LDFLAGS = -T newscript -N
ASFLAGS = -mips1
CPPFLAGS = $(INCDIR)

# if you aren't cross-compiling:
# GCCDIR =
# LDFLAGS = -N -T 0
# ASFLAGS =
# CPPFLAGS = -P $(INCDIR)


CC = $(GCCDIR)gcc
AS = $(GCCDIR)as
LD = $(GCCDIR)ld
CPP = $(GCCDIR)cpp

INCDIR =-I../userprog -I../threads
CFLAGS = -G 0 -c $(INCDIR)

all: halt matmult sort testfiles lockTest conditionTest Proj2Test exec WalmartSimulation Problem2 forkMatmult forkSort Proj3test

start.o: start.s ../userprog/syscall.h
	$(CPP) $(CPPFLAGS) start.s > strt.s
	$(AS) $(ASFLAGS) -o start.o strt.s
	rm strt.s

halt.o: halt.c
	$(CC) $(CFLAGS) -c halt.c
halt: halt.o start.o
	$(LD) $(LDFLAGS) start.o halt.o -o halt.coff
	../bin/coff2noff halt.coff halt

sort.o: sort.c
	$(CC) $(CFLAGS) -c sort.c
sort: sort.o start.o
	$(LD) $(LDFLAGS) start.o sort.o -o sort.coff
	../bin/coff2noff sort.coff sort

matmult.o: matmult.c
	$(CC) $(CFLAGS) -c matmult.c
matmult: matmult.o start.o
	$(LD) $(LDFLAGS) start.o matmult.o -o matmult.coff
	../bin/coff2noff matmult.coff matmult

testfiles.o: testfiles.c
	$(CC) $(CFLAGS) -c testfiles.c
testfiles: testfiles.o start.o
	$(LD) $(LDFLAGS) start.o testfiles.o -o testfiles.coff
	../bin/coff2noff testfiles.coff testfiles

lockTest.o: lockTest.c
	$(CC) $(CFLAGS) -c lockTest.c
lockTest: lockTest.o start.o
	$(LD) $(LDFLAGS) start.o lockTest.o -o lockTest.coff
	../bin/coff2noff lockTest.coff lockTest

conditionTest.o: conditionTest.c
	$(CC) $(CFLAGS) -c conditionTest.c
conditionTest: conditionTest.o start.o
	$(LD) $(LDFLAGS) start.o conditionTest.o -o conditionTest.coff
	../bin/coff2noff conditionTest.coff conditionTest
	
Proj2Test.o: Proj2Test.c
	$(CC) $(CFLAGS) -c Proj2Test.c
Proj2Test: Proj2Test.o start.o
	$(LD) $(LDFLAGS) start.o Proj2Test.o -o Proj2Test.coff
	../bin/coff2noff Proj2Test.coff Proj2Test	

exec.o: exec.c
	$(CC) $(CFLAGS) -c exec.c
exec: exec.o start.o
	$(LD) $(LDFLAGS) start.o exec.o -o exec.coff
	../bin/coff2noff exec.coff exec
	
WalmartSimulation.o: WalmartSimulation.c
	$(CC) $(CFLAGS) -c WalmartSimulation.c
WalmartSimulation: WalmartSimulation.o start.o
	$(LD) $(LDFLAGS) start.o WalmartSimulation.o -o WalmartSimulation.coff
	../bin/coff2noff WalmartSimulation.coff WalmartSimulation		

Problem2.o: Problem2.c
	$(CC) $(CFLAGS) -c Problem2.c
Problem2: Problem2.o start.o
	$(LD) $(LDFLAGS) start.o Problem2.o -o Problem2.coff
	../bin/coff2noff Problem2.coff Problem2

forkMatmult.o: forkMatmult.c
	$(CC) $(CFLAGS) -c forkMatmult.c
forkMatmult: forkMatmult.o start.o
	$(LD) $(LDFLAGS) start.o forkMatmult.o -o forkMatmult.coff
	../bin/coff2noff forkMatmult.coff forkMatmult	
	
forkSort.o: forkSort.c
	$(CC) $(CFLAGS) -c forkSort.c
forkSort: forkSort.o start.o
	$(LD) $(LDFLAGS) start.o forkSort.o -o forkSort.coff
	../bin/coff2noff forkSort.coff forkSort

Proj3test.o: Proj3test.c
	$(CC) $(CFLAGS) -c Proj3test.c
Proj3test: Proj3test.o start.o
	$(LD) $(LDFLAGS) start.o Proj3test.o -o Proj3test.coff
	../bin/coff2noff Proj3test.coff Proj3test

	
clean:
	rm -f *.o *.coff
