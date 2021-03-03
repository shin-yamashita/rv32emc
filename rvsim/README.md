
# rvsim RV32 Instruction set simulator

Target architecture : RV32IMC / RV32EMC

$ ./rvsim {-rv32e}




source:

rvsim.c    main() console command processor
simcore.c  ISS core

monlib.c   command processor utility
consio.c   console 

optab.h    RV32 instruction table
c-optab.h  Compressed instruction table

syscall.h  rv32 gcc system call deffs

RV-insn.ods RV32 instruction table source
  -> RV-insn.csv 
  -> c-insn.csv  
insntab.py   RV-insn.csv to optab.h converter
c-insntab.py c-insn.csv to c-optab.h converter


