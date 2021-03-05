

CC      = gcc
LD      = $(CC)

DEST	= /usr/local

CFLAGS  = -g -Wall -O2 -I./include
LDFLAGS = -g -Wall -O2 

LDLIBS	= -lbfd -lreadline -lcurses 

SRCS	= rvsim.c  monlib.c simcore.c consio.c
OBJS	= $(SRCS:.c=.o)

all:	rvsim

rvsim:	$(OBJS)
	$(CC) -o rvsim $(LDFLAGS) $(OBJS) $(LDLIBS)

optab.h:	RV-insn.csv
	./insntab.py $^ > optab.h

c-optab.h:	c-insn.csv
	./c-insntab.py $^ > c-optab.h

install:	rvsim
	cp $^ $(DEST)/bin/

clean:
	rm -f *.o \#*\# *~ rvsim

depend: $(SRCS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	$(CPP) $(CFLAGS) -MM $(SRCS) >>Makefile

#----- dependences -----

rvsim.o: rvsim.c monlib.h simcore.h consio.h
monlib.o: monlib.c monlib.h
simcore.o: simcore.c simcore.h optab.h c-optab.h consio.h syscall.h
consio.o: consio.c consio.h
