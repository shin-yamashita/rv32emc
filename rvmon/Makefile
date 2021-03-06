#
# 2007/04/09  sr_test compile for sr cpu
# 2014/12/09  gcc-4.6.3 xgcc test
# 2017/03/20  risc-v test
#

#ARCH = RV32I
#ARCH = RV32IM
#ARCH = RV32IMC
ARCH = RV32EMC

ifeq ($(ARCH), RV32I)
 PREFIX  = /opt/riscv/bin/riscv64-unknown-elf-
 PREFIX32  = /opt/riscv/bin/riscv32-unknown-elf-
 MFLAG   = -march=rv32i -mabi=ilp32
endif
ifeq ($(ARCH), RV32IM)
 PREFIX  = /opt/riscv/bin/riscv64-unknown-elf-
 PREFIX32  = /opt/riscv/bin/riscv32-unknown-elf-
 MFLAG   = -march=rv32im -mabi=ilp32
endif
ifeq ($(ARCH), RV32IMC)
 PREFIX  = /opt/riscv/bin/riscv64-unknown-elf-
 PREFIX32  = /opt/riscv/bin/riscv32-unknown-elf-
 MFLAG   = -march=rv32imc -mabi=ilp32
endif
ifeq ($(ARCH), RV32EMC)
 PREFIX  = /opt/rv32e/bin/riscv32-unknown-elf-
 PREFIX32  = $(PREFIX)
 MFLAG   = -march=rv32emc -mabi=ilp32e
endif


CC	= $(PREFIX)gcc
CPP	= $(PREFIX)g++
LD	= $(PREFIX)ld
RUN	= $(PREFIX32)run
OBJCP	= $(PREFIX)objcopy
OBJDUMP	= $(PREFIX)objdump

#CFLAGS	= -g -Wall -nostartfiles -O2
CFLAGS	= -g -Wall -O2 $(MFLAG)  -nostartfiles -I./include 

#LDFLAGS =  -nostdlib -Wl,-Map,$(TARGET).map
LDFLAGS = $(MFLAG) -Wl,-Map,$@.map,-T,rv32ldscr.x  -nostdlib -L./lib
LDLIBS  = -lmc -lm -lc -lgcc

SRCS	= rvmon.c

PRGS    = $(SRCS:.c=)
OBJS    = $(SRCS:.c=.o)
DEPS	= $(SRCS:.c=.dep) crt0.dep
MEM	= $(SRCS:.c=.mem)
REF	= $(SRCS:.c=.ref)

all:	$(PRGS)

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

%.lst:	%
	$(OBJDUMP) -D -S -h $< > $@
#	$(OBJDUMP) -D -h $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

%.o:	%.cpp
	$(CPP) $(CFLAGS) -c $<

%.o:	%.S
	$(CC) $(CFLAGS) -c $<

install:	rvmon.mem rvmon
	cp -p rvmon.mem ../rv_core/syn-mm
	cp -p rvmon ../rv_core/syn-mm/rvmon.elf

clean:
	@rm -f $(PRGS) *.o core* a.out *~ \#*\# *.map *.lst *.ref *.gcc *.out *.bin *.mot *.mem

%.dep:	%.c
	$(CC) $(CFLAGS) -MM $< > $@

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
rvmon.o: rvmon.c include/stdio.h include/types.h include/stdint.h \
 include/ulib.h include/uartdrv.h
