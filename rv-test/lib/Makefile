#
# 2010/04/20	mini-stdio library
#

PREFIX  = /opt/rv32e/bin/riscv32-unknown-elf-
MFLAG   = -march=rv32emc -mabi=ilp32e

CC	= $(PREFIX)gcc
LD	= $(PREFIX)ld
AR	= $(PREFIX)ar
OBJCP	= $(PREFIX)objcopy
OBJDUMP	= $(PREFIX)objdump

CFLAGS	= -g -Wall $(MFLAG) -nostartfiles -O2 -I../include -DTXIRQ

LDFLAGS = -g -Wall $(MFLAG) -nostdlib
ARFLAGS	= crs

SRCS1    = ulib.c uartdrv.c fprint.c rcp.c sincos.c fsqrt.c strtok.c strlen.c memset.c memclr.c
OBJS1    = $(SRCS1:.c=.o)
DEPS    = $(SRCS1:.c=.dep)

all:	libmc.a

fprintf:	fprintf.c
	gcc -O2 -Wall fprintf.c -o fprintf

libmc.a:	$(OBJS1)
	$(AR) $(ARFLAGS) libmc.a $(OBJS1)

%.lst:	%.o
	$(OBJDUMP) -D -S -h $< > $@

%.o:	%.c
	$(CC) $(CFLAGS) -c $<

%.dep:	%.c
	$(CPP) $(CFLAGS) -MM $< > $@

clean:
	@rm -f *.abs *.o core* a.out *.bak *~ \#*\#  *.map *.lst *.dep libmc.a fprintf

depend:	$(DEPS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	cat *.dep >>Makefile
	rm -f *.dep
#       $(CPP) $(CFLAGS) -M $(SRCS) | \

#----- dependences -----

fprint.o: fprint.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/time.h ../include/ulib.h \
 ../include/uartdrv.h
fsqrt.o: fsqrt.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/types.h
memclr.o: memclr.c
memset.o: memset.c
rcp.o: rcp.c
sincos.o: sincos.c
strlen.o: strlen.c
strtok.o: strtok.c
uartdrv.o: uartdrv.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/ulib.h ../include/uartdrv.h
ulib.o: ulib.c ../include/stdio.h ../include/types.h ../include/stdint.h \
 ../include/ulib.h ../include/uartdrv.h ../include/time.h
