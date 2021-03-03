
CFLAGS  = -g -Wall  -O2 -I./include
#LDFLAGS = -g -Wall -no-pie -O2 
LDFLAGS = -g -Wall -O2 

CC      = gcc
LD      = $(CC)
LDLIBS	= -lbfd -lreadline -lcurses 
#-lz -ldl -lm

#-liberty
# -lopcodes -lbfd -liberty

#SRCS	= objdump.c $(DEBUG_SRCS) 
# rddbg.c debug.c stabs.c ieee.c rdcoff.c

SRCS	= rvsim.c  monlib.c simcore.c consio.c
# filemode.c bucomm.c

OBJS	= $(SRCS:.c=.o)

all:	rvsim
# ophdlgen dbg_dump

rvsim:	$(OBJS)
	$(CC) -o rvsim $(LDFLAGS) $(OBJS) $(LDLIBS)

dbg_dump:	dbg_dump.o simcore.o consio.o
	$(CC) -o dbg_dump $(LDFLAGS) dbg_dump.o simcore.o consio.o $(LDLIBS)


optab.h:	RV-insn.csv
	./insntab.py $^ > optab.h

c-optab.h:	c-insn.csv
	./c-insntab.py $^ > c-optab.h

ophdlgen:	ophdlgen.o
	$(CC) -o ophdlgen $(LDFLAGS) ophdlgen.o

clean:
	rm -f *.o \#*\# *~ rvsim ophdlgen dbg_dump

#objdump:	$(OBJS)
#	$(CC) -o objdump $(LDFLAGS) $(OBJS) $(LDLIBS)

depend: $(SRCS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	$(CPP) $(CFLAGS) -MM $(SRCS) >>Makefile

##| \
##	  sed 's/\/usr\/[^ ]* *//g;/^  \\$$/d' |\
##	  grep -v '^ \\$$' >>Makefile

#----- dependences -----

rvsim.o: rvsim.c monlib.h simcore.h consio.h
monlib.o: monlib.c monlib.h
simcore.o: simcore.c simcore.h optab.h c-optab.h consio.h syscall.h
consio.o: consio.c consio.h
