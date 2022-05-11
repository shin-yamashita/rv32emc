
# rvsim RV32 Instruction set simulator

Target architecture : RV32IMC / RV32EMC

## usage

```
$ rvsim {rv32 executable (elf)}
======= rvsim ==============================================
   rv32 processor simulator.
rvsim-I> 
  load       <file (rv32 executable)>
  dump       <-stk> <addr|label>
  reg        
  viewreg    
  run        <N cyc|-all>
  cont       <N cyc|-all>
  trace      <N cyc|-all> <-r> <| tee (fn)>
  break      <addr|label> <-d|-e (n)>
  info       
  header     
  symbol     
  dis        <addr|lable>
  debug      <-r (fn)> <-m (fn)> <-c>
  help       <cmd>
  quit       
  exit       
```

```
rvsim-E> trace
 ra(   121fa)  sp( 3ffffd0)  gp(   24210)  tp(       0)  t0(   1037e)  t1(       f)  t2(       0)  s0(       0)  s1(       0)  a0(   23aa4)  a1(       0) 
 a2(    120c)  a3(   121fe)  a4(   23aa4)  a5(       4) 
   cnt     pc:       ir                               mar      mdr      mdw     rrd1     rrd2  alu rwa rwd    rwdat        ra       sp       gp       a0       a1       a2       a3       s4       s5
     0  10362: ffffffff auipc    gp,81920               0        0       --        0        0  CMP  --  xx        0         0  3ffffd0        0        0        0        0        0        0        0  
     1  10362: 00014197 auipc    gp,81920               0        0       --        0        0  CMP  --  xx        0         0  3ffffd0        0        0        0        0        0        0        0  
     2  10366: eae18193 addi     gp,gp,-338             0        0       --    10362    14000  ADD  gp ALU        0         0  3ffffd0        0        0        0        0        0        0        0  
     3  1036a: 89418513 addi     a0,gp,-1900            0        0       --    24362 fffffeae  ADD  gp ALU    24362         0  3ffffd0        0        0        0        0        0        0        0  
     4  1036e: 00015617 auipc    a2,86016               0        0       --    24210 fffff894  ADD  a0 ALU    24210         0  3ffffd0        0        0        0        0        0        0        0  
     5  10372: 94260613 addi     a2,a2,-1726            0        0       --    1036e    15000  ADD  a2 ALU    23aa4         0  3ffffd0    24362        0        0        0        0        0        0  
     6  10376:     8e09 c.sub    a2,a2,a0               0        0       --    2536e fffff942  ADD  a2 ALU    2536e         0  3ffffd0    24210        0        0        0        0        0        0  
     7  10378:     4581 c.li     a1,x0,0                0        0       --    24cb0    23aa4  SUB  a2 ALU    24cb0         0  3ffffd0    24210    23aa4        0        0        0        0        0  
     8  1037a: 5e7010ef jal      ra,(12160)             0        0       --        0        0  ADD  a1 ALU     120c         0  3ffffd0    24210    23aa4        0    2536e        0        0        0  
     9  1037e: 0000c517 -b-                             0        0       --        0    1037e   S2  ra ALU        0         0  3ffffd0    24210    23aa4        0    24cb0        0        0        0  
    10  12160:     433d c.li     t1,x0,15               0        0       --        0    1037e   S2  --  xx    1037e         0  3ffffd0    24210    23aa4        0     120c        0        0        0  
    11  12162:     872a c.mv     a4,x0,a0               0        0       --        0        f  ADD  t1 ALU        0         0  3ffffd0    24210    23aa4        0     120c        0        0        0  
	:
    24  121ae: 00b704a3 sb       a4,a1,9            23aae        0        0    23aa4        0   --  x0  --        0     121fa  3ffffd0    24210    23aa4        0     120c    121fe        0        0  
   cnt     pc:       ir                               mar      mdr      mdw     rrd1     rrd2  alu rwa rwd    rwdat        ra       sp       gp       a0       a1       a2       a3       s4       s5
```


## build

Ubuntu 20.04  

$ apt install binutils-dev libreadline8 libtinfo6  

```
$ make
$ sudo make install
```

## source

```
rvsim.c      : main() console command processor  
simcore.c    : ISS core  

monlib.c     : command processor utility  
consio.c     : console  

optab.h      : RV32 instruction table (generated file)  
c-optab.h    : Compressed instruction table (generated file)  

syscall.h    : rv32 gcc system call deffs  

RV-insn.ods  : RV32 instruction table source  
  RV-insn tab -> RV-insn.csv  
  c-insn tab  -> c-insn.csv  

insntab.py   : RV-insn.csv to optab.h converter  
c-insntab.py : c-insn.csv to c-optab.h converter  
```

