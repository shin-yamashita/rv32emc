

CC      = gcc
LD      = $(CC)

DEST	= /usr/local

CFLAGS  = -g -Wall -O2 -I./include
LDFLAGS = -g -Wall -O2 

LDLIBS	= -lbfd -lreadline -ltinfo

SRCS	= rvsim.c  monlib.c simcore.c consio.c
OBJS	= $(SRCS:.c=.o)

all:	rvsim

rvsim:	$(OBJS)
	$(CC) -o rvsim $(LDFLAGS) $(OBJS) $(LDLIBS)

RV-insn.csv:	RV-insn.ods
	soffice --convert-to csv $<

optab.h:	RV-insn.csv
	./insntab.py $^ > optab.h

c-optab.h:	c-insn.csv
	./c-insntab.py $^ > c-optab.h

csrtab.h:	CSR.csv
	./csrtab.py $^ > $@

install:	rvsim
	cp $^ $(DEST)/bin/

hdl:	optab.h c-optab.h
	cp rv_dec_insn.sv ../rv_core/hdl/
	cp rv_exp_cinsn.sv ../rv_core/hdl/

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
simcore.o: simcore.c simcore.h optab.h c-optab.h consio.h syscall.h csrtab.h
consio.o: consio.c consio.h
