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

CFLAGS	= -g -Wall $(MFLAG) -nostartfiles -O2 -I../include
# -DTXIRQ

LDFLAGS = -g -Wall $(MFLAG) -nostdlib
ARFLAGS	= crs

SRCS1    = ulib.c uartdrv.c fprint.c rcp.c sincos.c fsqrt.c strtok.c strlen.c memset.c memclr.c
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
	$(CPP) $(CFLAGS) -M $< > $@

clean:
	@rm -f *.abs *.o core* a.out *~ \#*\#  *.map *.lst *.dep libmc.a

depend:	$(DEPS)
	cp Makefile Makefile.bak
	sed '/^#----- dependences -----/q' Makefile.bak >Makefile
	echo >>Makefile
	cat *.dep | \
	  sed 's/\/usr\/[^ ]* *//g;/^  \\$$/d' |\
	  grep -v '^ \\$$' >>Makefile
	rm -f *.dep
#       $(CPP) $(CFLAGS) -M $(SRCS) | \

#----- dependences -----

fprint.o: fprint.c ../include/stdio.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/newlib.h \
 /opt/rv32e/riscv32-unknown-elf/include/_newlib_version.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/config.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/ieeefp.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/features.h ../include/types.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stdarg.h \
 /opt/rv32e/riscv32-unknown-elf/include/string.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/reent.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stddef.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/_default_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/lock.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/cdefs.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/_locale.h \
 /opt/rv32e/riscv32-unknown-elf/include/strings.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/string.h ../include/time.h \
 ../include/ulib.h ../include/uartdrv.h
fsqrt.o: fsqrt.c ../include/stdio.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/newlib.h \
 /opt/rv32e/riscv32-unknown-elf/include/_newlib_version.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/config.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/ieeefp.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/features.h ../include/types.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stdarg.h \
 ../include/types.h
rcp.o: rcp.c
sincos.o: sincos.c /opt/rv32e/riscv32-unknown-elf/include/math.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/reent.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/newlib.h \
 /opt/rv32e/riscv32-unknown-elf/include/_newlib_version.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/config.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/ieeefp.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/features.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stddef.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/_default_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/lock.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/cdefs.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h
strtok.o: strtok.c /opt/rv32e/riscv32-unknown-elf/include/string.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/newlib.h \
 /opt/rv32e/riscv32-unknown-elf/include/_newlib_version.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/config.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/ieeefp.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/features.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/reent.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stddef.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/_default_types.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/lock.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/cdefs.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/_locale.h \
 /opt/rv32e/riscv32-unknown-elf/include/strings.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/string.h
uartdrv.o: uartdrv.c ../include/stdio.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/newlib.h \
 /opt/rv32e/riscv32-unknown-elf/include/_newlib_version.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/config.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/ieeefp.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/features.h ../include/types.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stdarg.h \
 ../include/ulib.h ../include/uartdrv.h
ulib.o: ulib.c ../include/stdio.h \
 /opt/rv32e/riscv32-unknown-elf/include/_ansi.h \
 /opt/rv32e/riscv32-unknown-elf/include/newlib.h \
 /opt/rv32e/riscv32-unknown-elf/include/_newlib_version.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/config.h \
 /opt/rv32e/riscv32-unknown-elf/include/machine/ieeefp.h \
 /opt/rv32e/riscv32-unknown-elf/include/sys/features.h ../include/types.h \
 /opt/rv32e/lib/gcc/riscv32-unknown-elf/9.2.0/include/stdarg.h \
 ../include/ulib.h ../include/uartdrv.h ../include/time.h
