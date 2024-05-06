#
# 2017/03/20  risc-v test
# 2022/04     for Arty-A7 example
#

#ARCH = rv32ec
ARCH = rv32em
#ARCH = rv32emc
#ARCH = rv32emzfinx
#ARCH = rv32emczfinx

PREFIX  = /opt/rv32e/bin/riscv32-unknown-elf-
MFLAG   = -march=$(ARCH) -mabi=ilp32e

CC	= $(PREFIX)gcc
CPP	= $(PREFIX)g++
LD	= $(PREFIX)ld
RUN	= $(PREFIX)run
OBJCP	= $(PREFIX)objcopy
OBJDUMP	= $(PREFIX)objdump

#CFLAGS	= -g -Wall -nostartfiles -O2
CFLAGS	= -g -Wall -O2 $(MFLAG)  -nostartfiles -I./include 

#LDFLAGS =  -nostdlib -Wl,-Map,$(TARGET).map
#LDFLAGS = $(MFLAG) -Wl,-Map,$@.map,-T,rv32ldscr.x  -nostdlib -L./lib
LDFLAGS = $(MFLAG) -Wl,-Map,$@.map,-T,lnkscr.x  -nostdlib -L./lib

LDLIBS  = -lmc -lm -lc -lgcc

SRCS	= rvmon.c

PRGS    = $(SRCS:.c=)
OBJS    = $(SRCS:.c=.o)
DEPS	= $(SRCS:.c=.dep) crt0.dep
MEM	= $(SRCS:.c=.mem)
REF	= $(SRCS:.c=.ref)

all:	$(PRGS)

lib/libmc.a:
	make -C lib

mem:	$(MEM)

list:	$(SRCS:.c=.lst)

rvmon:	crt0.o $(OBJS) 
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.mot:	%
	$(OBJCP) -O srec --srec-forceS3 -S -R .stack $< $@

%.bin:	%
	$(OBJCP) -O binary -S -R .stack $< $@

%.mem:	%.bin
	od -An -tx4 -v $< > $@
	./convmem.py $@

%.lst:	%
	$(OBJDUMP) -D -S -h $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

%.o:	%.cpp
	$(CPP) $(CFLAGS) -c $<

%.o:	%.S
	$(CC) $(CFLAGS) -c $<

install:	rvmon.mem rvmon
	cp -p prog_u.mem prog_l.mem ../rv_core/syn/
	cp -p rvmon ../rv_core/syn/rvmon.elf

apps:
	make install -C app

clean:
	@rm -f $(PRGS) *.o core* a.out *~ \#*\# *.map *.lst *.ref *.gcc *.out *.bin *.mot *.mem
	make clean -C app

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
rvmon.o: rvmon.c include/stdio.h include/types.h include/ulib.h \
 include/uartdrv.h include/time.h include/ulib.h include/readline.h lib/libmc.a 
