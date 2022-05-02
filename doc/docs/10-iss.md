
# 命令セットシミュレータ

RTL 実装を行う前に、命令セットシミュレータ(ISS)を開発し、ハードウェアのリファレンスに用いた。  
CPU コア開発の流れは以下のようになる。  

1. CPUのハード構成を設計し、パイプラインまで正確にエミュレートした ISS をＣ言語で作成  
   → ISS でテストプログラムを実行  
  risc-v のクロス gdb でのテストプログラム実行結果(実行トレース)をリファレンスとして ISS をデバッグ  
2. 作成した ISS をリファレンスとして HDL(SystemVerilog) を記述、ISS の実行トレースと HDL の論理シミュレーション(xsim) 結果をつき合わせてHDLをデバッグ  
   ISS の実行トレースと突き合わせしやすい形のトレースを HDL の $display で出力しバグの追跡を行った   
   また、ISS 用に作成した命令のテーブルを使用して HDL の命令テーブルを自動生成するようにして記述ミスなどを防いだ  
3. テストプログラムで正しい実行結果が得られるようになったところで、割り込み機構を実装  
   uart でターミナルを接続、タイマー(mtime) でタイマー割り込みができるようにして、Vivado で論理合成を行いFPGAにロード、実機でターミナルをつないで動作確認  
4. モニタープログラムを組んで、テストプログラムをロード、実行できるようにして更に長時間の動作検証を行う  

## 開発した ISS (rvsim)  

[source(github)](https://github.com/shin-yamashita/rv32emc/tree/main/rvsim)

```
rvsim
├── RV-insn.ods   : RV32 instruction table source 
│      ├─ RV-insn tab → RV-insn.csv  
│      └─ c-insn tab  → c-insn.csv  
├── insntab.py    : RV-insn.csv to optab.h / rv_dec_insn.sv converter
├── c-insntab.py  : c-insn.csv to c-optab.h / rv_exp_cinsn.sv converter
├── simcore.c     : ISS core  
├── syscall.h     : rv32 gcc system call deffs  
├── rvsim.c       : main() console command processor  
├── consio.c      : console interface 
└── monlib.c      : command processor utility  
(generated files) 
├── optab.h       : RV32 instruction table
├── c-optab.h     : Compressed instruction table
├── rv_dec_insn.sv  : decoder RTL
└── rv_exp_cinsn.sv : c-insn decoder RTL
```

simcore.c が rv_core のエミュレータ本体である。  
レジスタ(flip flop)の入力 d と、出力 q を持つ構造体 `reg32` などを定義し、論理演算結果を d にセットし、 `clock()` 関数で全レジスタの d→q を一斉に行うことで、回路動作をエミュレートした。  
`typedef struct _reg32 { u32 d; u32 q; } reg32;`  

rvsim.c が ISS のコマンドラインインターフェースである。  
GNU binutil の libbfd.so を用いて elf ファイルのハンドリングを行っている。  

[rvsim の compile install 手順](x_env.html#rvsim-iss)  

https://github.com/shin-yamashita/rv32emc/tree/main/rv-test

## usage

rvsim では write system call を実装しているので、rv32e のバイナリを実行し、printf() などで標準出力に表示することができる。  

- newlib/libc の 標準出力は system call を用いているので、そのまま表示できる。  
- logic simulation では system call (ecall ソフト割り込み) を実装していないが、デバッグ用キャラクタ出力 I/O port (DBG_PUTC) を設けており、下位関数の _write() の出力先を DBG_PUTC に切り替えることでメッセージをファイルに出力することができる。  
- rvsim は DBG_PUTC ポートに対応しているので、logic simulation と同じバイナリで実行、検証ができる。  

→ [rv-test(github)](https://github.com/shin-yamashita/rv32emc/tree/main/rv-test)参照  

```
$ rvsim {rv32 executable (elf)}    # elf ファイルを読み込む。または起動後 load コマンドで読み込む。
======= rvsim ==============================================
   rv32 processor simulator.
 load ELF file 'xxx' : RISCV RVC RVE   # elf file から arch を判別
 number_of_symbols = 155
 vaddr:0 memsize:10000 _end:333c start:0
rvsim-E>                               # prompt : rv32e は 'rvsim-E>' その他は 'rvsim-I>' 
  load       <file (rv32 executable)>
  dump       <-stk> <addr|label>       # メモリダンプ　(PgUp/PgDn/Up/Down キーでアドレス移動、'q' で終了)
  reg                                  # register file dump
  run        <N cyc|-all>              # N cycle(or 無制限) 実行 'q' キーで中断
  cont       <N cyc|-all>              # 継続実行
  trace      <N cyc|-all> <-r> <| tee (fn)>  # 実行トレース （| tee でファイルにダンプ)
  break      <addr|label> <-d|-e (n)>  # break point 設定/表示 (-d/-e で disable/enable)
  info                                 # 読み込んだ elf のメモリマップなど表示
  header                               # elf header 表示
  symbol                               # symbol list (address : symbol name) 表示
  dis        <addr|lable>              # addr/label 以降の disassemble list 表示 {space:next-page q:quit}
  debug      <-r (fn)> <-m (fn)> <-c>  # Register / Memory write log を指定のファイルに出力
  help       <cmd>
  quit       
  exit       

rvsim-E> 
```

### trace command 実行例  
   ++enter++ : 1 cycle 実行  
   ++space++ : 1 page 実行  
   10進数値 : 指定サイクル実行  
   ++q++ : quit  

```
rvsim-E> trace
 ra(      2a)  sp(    8000)  gp(    33f8)  tp(       0)  t0(      50)  t1(       0)  t2(       0)  s0(       0)  s1(       0)  a0(    2c14)  a1(     728) 
 a2(       0)  a3(       0)  a4(     728)  a5(    2c14) 
   cnt     pc       ir opc      opr                  mar      mdr      mdw     rrd1     rrd2  alu rwa rwd    rwdat     ra(1)    sp(2)    gp(3)   a0(10)   a1(11)   a2(12)   a3(13)    s0(8)    s1(9)
     0      0 ffffffff auipc    gp,12288               0        0       --        0        0   --  --  xx        0         0  3ffffd0        0        0        0        0        0        0        0  
     1      0 00003197 auipc    gp,12288               0        0       --        0        0   --  --  xx        0         0  3ffffd0        0        0        0        0        0        0        0  
     2      4 3f818193 addi     gp,gp,1016             0        0       --        0     3000  ADD  gp ALU        0         0  3ffffd0        0        0        0        0        0        0        0  
     3      8 00008117 auipc    sp,32768               0        0       --     3000      3f8  ADD  gp ALU     3000         0  3ffffd0        0        0        0        0        0        0        0  
     4      c ff810113 addi     sp,sp,-8               0        0       --        8     8000  ADD  sp ALU     33f8         0  3ffffd0        0        0        0        0        0        0        0  
     5     10 00000297 auipc    t0,0                   0        0       --     8008 fffffff8  ADD  sp ALU     8008         0  3ffffd0     3000        0        0        0        0        0        0  
     6     14 04028293 addi     t0,t0,64               0        0       --       10        0  ADD  t0 ALU     8000         0  3ffffd0     33f8        0        0        0        0        0        0  
     7     18 30529073 csrrw    x0,t0,mtvec            0        0       --       10       40  ADD  t0 ALU       10         0     8008     33f8        0        0        0        0        0        0  
     8     1c 81c18513 addi     a0,gp,-2020            0        0       --       50        0  FLD  x0 ALU       50         0     8000     33f8        0        0        0        0        0        0  
     9     20 f4418593 addi     a1,gp,-188             0        0       --     33f8 fffff81c  ADD  a0 ALU        0         0     8000     33f8        0        0        0        0        0        0  
    10     24     8d89 c.sub    a1,a1,a0               0        0       --     33f8 ffffff44  ADD  a1 ALU     2c14         0     8000     33f8        0        0        0        0        0        0  
    11     26 607010ef jal      ra,(1e2c)              0        0       --     333c     2c14  SUB  a1 ALU     333c         0     8000     33f8        0        0        0        0        0        0  
    12     2a     4502 -b-      --                     0        0       --        0       2a   S2  ra ALU      728         0     8000     33f8     2c14        0        0        0        0        0  
    13   1e2c 00357793 andi     a5,a0,3                0        0       --        0       2a   S2  --  xx       2a         0     8000     33f8     2c14     333c        0        0        0        0  
    14   1e30     c7b9 c.beqz   a5,x0,(1e7e)           0        0       --     2c14        3  AND  a5 ALU        0         0     8000     33f8     2c14      728        0        0        0        0  
    15   1e32 fff58713 -b-      --                     0        0       --        0        0   xx  x0  --        0        2a     8000     33f8     2c14      728        0        0        0        0  
    16   1e7e     87aa c.mv     a5,x0,a0               0        0       --        0        0   xx  --  xx        0        2a     8000     33f8     2c14      728        0        0        0        0  
    17   1e80     872e c.mv     a4,x0,a1               0        0       --        0     2c14  ADD  a5 ALU        0        2a     8000     33f8     2c14      728        0        0        0        0  
    18   1e82     b7f9 c.j      x0,(1e50)              0        0       --        0      728  ADD  a4 ALU     2c14        2a     8000     33f8     2c14      728        0        0        0        0  
    19   1e84     577d -b-      --                     0        0       --        0     1e84   S2  x0 ALU      728        2a     8000     33f8     2c14      728        0        0        0        0  
    20   1e50     460d c.li     a2,x0,3                0        0       --        0     1e84   S2  --  xx     1e84        2a     8000     33f8     2c14      728        0        0        0        0  
    21   1e52     86be c.mv     a3,x0,a5               0        0       --        0        3  ADD  a2 ALU        0        2a     8000     33f8     2c14      728        0        0        0        0  
   cnt     pc       ir opc      opr                  mar      mdr      mdw     rrd1     rrd2  alu rwa rwd    rwdat     ra(1)    sp(2)    gp(3)   a0(10)   a1(11)   a2(12)   a3(13)    s0(8)    s1(9)
rvsim-E>   ('q' キーでプロンプトに戻る)
```

### run command 実行例 (ecc)  

```
rvsim-E> r -all
reset_pc()
*** GF(2^8) Reed Solomon Coding. 12 parity, 6 error correction.
   :
**** No error.
** exit() timer: 0
exit(): 19

Total clock cycles: 1021788   stall - bra: 108623 [10.6%]  data: 72042 [ 7.1%]  exec: 1768 [ 0.2%]
rvsim-E> 
```

### 直接実行例  

```
$ rvsim -r ecc
reset_pc()
*** GF(2^8) Reed Solomon Coding. 12 parity, 6 error correction.
   :
**** No error.
** exit() timer: 0
exit(): 19

Total clock cycles: 1021788   stall - bra: 108623 [10.6%]  data: 72042 [ 7.1%]  exec: 1768 [ 0.2%]
```






