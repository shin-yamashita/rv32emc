#!/bin/bash
#
# coremark
#

if [ ! -d coremark ]; then
  git clone https://github.com/eembc/coremark.git

  cd coremark
  mv coremark.md5 coremark.md5.sav  # update checksum (2022/04 repo not updated)
  md5sum core_list_join.c core_main.c core_matrix.c core_state.c core_util.c coremark.h > coremark.md5
  cd ..
fi

make clean -C coremark
make compile PORT_DIR=`pwd`/rv32port -C coremark
cp coremark/coremark.exe coremark.elf

/opt/rv32e/bin/riscv32-unknown-elf-objcopy -O srec --srec-forceS3 -S -R .stack  coremark.elf coremark.mot
/opt/rv32e/bin/riscv32-unknown-elf-objdump -D -S -h coremark.elf > coremark.lst
cp coremark.mot ../term/

#
#
