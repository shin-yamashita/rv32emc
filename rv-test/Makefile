#
# 2007/04/09  sr_test compile for sr cpu
# 2014/12/09  gcc-4.6.3 xgcc test
# 2017/03/20  risc-v test
#

#ARCH = rv32ec
#ARCH = rv32em
#ARCH = rv32emc
#ARCH = rv32emzfinx
ARCH = rv32emczfinx

PREFIX  = /opt/rv32e/bin/riscv32-unknown-elf-
MFLAG   = -march=$(ARCH) -mabi=ilp32e

CC	= $(PREFIX)gcc
CPP	= $(PREFIX)g++
LD	= $(PREFIX)ld
RUN	= $(PREFIX)run
OBJCP	= $(PREFIX)objcopy
OBJDUMP	= $(PREFIX)objdump

#CFLAGS	= -g -Wall -nostartfiles -O2
CFLAGS	= -g -Wall -O2 $(MFLAG) -nostartfiles -I./include 

#LDFLAGS =  -nostdlib -Wl,-Map,$(TARGET).map
LDFLAGS = $(MFLAG) -Wl,-Map,$@.map,-T,rv32ldscr.x -nostdlib -L./lib

#,--verbose
# -L$(LDSCRIPT)
LDLIBS  = -lmc -lm -lc -lgcc

SRCS	= hello.c fptest.c gauss.c pi.c ecc.c mul.c
# endian.c
# rvmon.c
#fplib.c sr_test.c

PRGS    = $(SRCS:.c=)
OBJS    = $(SRCS:.c=.o)
DEPS	= $(SRCS:.c=.dep) crt0.dep
MEM	= $(SRCS:.c=.mem)
REF	= $(SRCS:.c=.ref)

all:	$(PRGS)

mem:	$(MEM)

ref:	$(REF)

list:	$(SRCS:.c=.lst)

lib/libmc.a:
	make -C lib

#gauss:	gauss.o crt0.o
#	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)
#	$(RUN) $@ > $@.out
#	gcc $@.c -o $@.gcc -lm
#	./$@.gcc > $@.ref
#	diff $@.ref $@.out

%.mot:	%
	$(OBJCP) -O srec --srec-forceS3 -S -R .stack $< $@

%.bin:	%
	$(OBJCP) -O binary -S -R .stack $< $@

%.mem:	%.bin
	od -An -tx4 -v $< > $@

init.o:	init.c
	$(CC) $(CFLAGS) -c $<

%:	crt0.o  %.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.ref:	%
	rvsim -r $< 2> $@

%.lst:	%
	$(OBJDUMP) -D -S -h $< > $@
#	$(OBJDUMP) -D -h $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

%.o:	%.cpp
	$(CPP) $(CFLAGS) -c $<

%.o:	%.S
	$(CC) $(CFLAGS) -c $<

install:	$(MEM) $(REF)
	cp -p $^ ../rv_core/sim/

clean:
	make clean -C lib
	@rm -f $(PRGS) *.o core* a.out *~ \#*\# *.map *.lst *.ref *.gcc *.out *.bin *.mot *.mem *.bak

%.dep:	%.c
	$(CC) $(CFLAGS) -MM $< | head -c -1 - > $@
	echo " lib/libmc.a " >> $@

%.dep:	%.S
	$(CC) $(CFLAGS) -MM $< > $@

depend:	$(DEPS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	cat *.dep >>Makefile
	rm -f *.dep

#----- dependences -----

crt0.o: crt0.S
ecc.o: ecc.c include/stdio.h include/types.h lib/libmc.a 
fptest.o: fptest.c include/stdio.h include/types.h include/ulib.h \
 include/uartdrv.h lib/libmc.a 
gauss.o: gauss.c include/stdio.h include/types.h lib/libmc.a 
hello.o: hello.c include/stdio.h include/types.h include/ulib.h \
 include/uartdrv.h lib/libmc.a 
mul.o: mul.c include/stdio.h include/types.h include/ulib.h \
 include/uartdrv.h lib/libmc.a 
pi.o: pi.c include/stdio.h include/types.h include/time.h include/ulib.h \
 include/uartdrv.h lib/libmc.a 
