
# 論理シミュレーション

rv_core の動作を論理シミュレーションで確認する。  
rvsim の動作と比較し、RTL のデバッグを行う。  

```bash
├── rv_core
│   ├── hdl/                # HDL source
│   └── sim/                # 論理シミュレーション環境
│      ├── convmem.py     # .mem ファイルを prog_u.mem/prog_l.mem に変換する
│      ├── compile.sh     # HDL を読み込んで vivado の xelab で elaboration → xsim.dir/tb_rvc
│      ├── run.sh         # シミュレーション実行 (xsim command line)
│      ├── sim.sh         # シミュレーション実行 (xsim GUI)
│      ├── regression.sh  # いくつかのプログラム(.mem)をシミュレーション実行し、結果を .ref と比較
│      ├── tb_rvc.prj     # HDL source list
│      ├── rvc.sv         # rv_core と dpram を実装した module
│      └── tb_rvc.sv      # rvc のテストベンチ
│
└── rv-test                  # テストプログラム
   ├── Makefile
   ├── crt0.S              # startup routine
   ├── *.c                 # program C source
   ├── rv32ldscr.x         # linker script
   ├── include/
   └── lib/                # mini stdio (printf etc)
```

## シミュレーション手順

### テストプログラムをクロスコンパイル

```bash
$ cd rv32emc/rv-test
$ make                  # クロスコンパイル → 実行バイナリ生成
    :
/opt/rv32e/bin/riscv32-unknown-elf-gcc -g -Wall -O2 -march=rv32emc -mabi=ilp32e -nostartfiles -I./include  -c pi.c
/opt/rv32e/bin/riscv32-unknown-elf-gcc -o pi crt0.o pi.o -march=rv32emc -mabi=ilp32e -Wl,-Map,pi.map,-T,rv32ldscr.x -nostdlib -L./lib -lmc -lm -lc -lgcc
    :
$ make install          # 実行バイナリから RAMの初期値ファイル(.mem)、ISS での実行結果(.ref) を作成
    :
/opt/rv32e/bin/riscv32-unknown-elf-objcopy -O binary -S -R .stack pi pi.bin     # 実行バイナリ pi をRAMの初期値ファイル pi.mem に変換
od -An -tx4 -v pi.bin > pi.mem
    :
rvsim -r pi 2> pi.ref   # ISS(rvsim) で実行バイナリ pi を実行し、printf() 出力を pi.ref にセーブ
reset_pc()
exit(): 19
Total clock cycles: 26717123   stall - bra: 1093043 [ 4.1%]  data: 346613 [ 1.3%]  exec: 10115680 [37.9%]
    :
cp -p hello.mem fptest.mem gauss.mem pi.mem ecc.mem mul.mem hello.ref fptest.ref gauss.ref pi.ref ecc.ref mul.ref ../rv_core/sim/
                        # .mem .ref を、シミュレーション実行 dir ../rv_core/sim/ にコピー
```

### 論理シミュレーション

```bash
$ cd rv32emc/rv_core/sim
$ ./convmem.py pi.mem   # 実行するプログラムの .mem ファイルを dpram の初期値ファイル prog_u.mem/prog_l.mem に変換
$ ./compile.sh          # elaboration
$ ./run.sh              # xsim 実行 (command line)
($ ./sim.sh)            # xsim 実行 (GUI)
            # printf() 出力は、"stderr.out" に出力される
```
