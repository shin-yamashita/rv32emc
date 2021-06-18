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
CFLAGS	= -g -Wall -O2 $(MFLAG) -fPIE -nostartfiles -I../include 

LNKROM   = -Xlinker -R../rvmon
#LDFLAGS =  -nostdlib -Wl,-Map,$(TARGET).map
#LDFLAGS = $(MFLAG) -Wl,-Map,$@.map,-T,rv32ldscr.x -nostdlib -L../lib $(LNKROM)
LDFLAGS = $(MFLAG) -Wl,-Map,$(@:.elf=.map),-T,lnkscr.x  -fpie -nostdlib -L../lib $(LNKROM)

#,--verbose
# -L$(LDSCRIPT)
LDLIBS  = -lmc -lm -lc -lgcc

SRCS	= hello.c fptest.c gauss.c pi.c ecc.c mul.c


PRGS    = $(SRCS:.c=.elf)
OBJS    = $(SRCS:.c=.o)
DEPS	= $(SRCS:.c=.dep)
MOT		= $(SRCS:.c=.mot)

all:	$(PRGS) $(MOT)

list:	$(SRCS:.c=.lst)

%.elf:	crt0.o %.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

%.mot:	%.elf
	$(OBJCP) -O srec --srec-forceS3 -S -R .stack $< $@

%.lst:	%.elf
	$(OBJDUMP) -D -S -h $< > $@
#	$(OBJDUMP) -D -h $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

%.o:	%.cpp
	$(CPP) $(CFLAGS) -c $<

%.o:	%.S
	$(CC) $(CFLAGS) -c $<

install:	$(MOT)
	cp -p $^ ../term/

clean:
	@rm -f $(PRGS) *.o core.* a.out *~ \#*\# *.map *.lst *.ref *.gcc *.out *.bin *.mot *.mem

%.dep:	%.c
	$(CC) $(CFLAGS) -MM $< > $@

depend:	$(DEPS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	cat *.dep >>Makefile
	rm -f *.dep

#----- dependences -----

ecc.o: ecc.c ../include/stdio.h ../include/types.h ../include/stdint.h
fptest.o: fptest.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/ulib.h ../include/uartdrv.h
gauss.o: gauss.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h
hello.o: hello.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/ulib.h ../include/uartdrv.h
mul.o: mul.c ../include/stdio.h ../include/types.h ../include/stdint.h \
 ../include/ulib.h ../include/uartdrv.h
pi.o: pi.c ../include/stdio.h ../include/types.h ../include/stdint.h
