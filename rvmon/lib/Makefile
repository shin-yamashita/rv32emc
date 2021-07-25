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

SRCS1    = ulib.c uartdrv.c fprint.c strtok.c strlen.c memset.c memclr.c ff.c mmcbb.c memcpy32.c readline.c
OBJS1    = $(SRCS1:.c=.o)
DEPS    = $(SRCS1:.c=.dep)

all:	libmc.a fprintf

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
	@rm -f *.abs *.o core* a.out *~ \#*\#  *.map *.lst *.dep *.bak libmc.a fprintf

depend:	$(DEPS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	cat *.dep >>Makefile
	rm -f *.dep

#----- dependences -----

ff.o: ff.c ../include/stdio.h ../include/types.h ../include/stdint.h \
 ../include/ff.h ../include/integer.h ../include/ffconf.h diskio.h \
 ../include/integer.h
fprint.o: fprint.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/time.h ../include/ulib.h \
 ../include/uartdrv.h ../include/ff.h ../include/integer.h \
 ../include/ffconf.h
memclr.o: memclr.c
memset.o: memset.c
mmcbb.o: mmcbb.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h diskio.h ../include/integer.h ../include/ulib.h \
 ../include/uartdrv.h
strlen.o: strlen.c
strtok.o: strtok.c
uartdrv.o: uartdrv.c ../include/stdio.h ../include/types.h \
 ../include/stdint.h ../include/ulib.h ../include/uartdrv.h
ulib.o: ulib.c ../include/stdio.h ../include/types.h ../include/stdint.h \
 ../include/ulib.h ../include/uartdrv.h ../include/time.h
