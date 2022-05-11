
# FPGA 実装

rv_core の動作を Xilinx Artix7 FPGA で確認するために example design を用意した。  
[Taget board : Arty-A7](x_env.html#arty-a7-35t)  

[source(github)](https://github.com/shin-yamashita/rv32emc/tree/main/rv_core)  rv32emc/rv_core/  

## example design  

rv_core とメモリ dpram、シリアル通信回路 rv_sio 等を接続した回路 rvc.sv を用意し、Arty-A7-35 ボードのターミナルで動作を確認する。
```
├── rv_core
│   ├── hdl/            # rv_core HDL source
│   └── syn
│     ├── build.sh              # 論理合成スクリプト
│     ├── build.tcl             # vivado tcl scripts
│     ├── read_hdl.tcl          # HDL source を読み込む
│     ├── write_mmi.tcl         # BRAM mapping file(prog.mmi) 生成
│     ├── arty-a7-pinassign.xdc # pin assign 制約
│     ├── timing.xdc            # timing 制約
│     ├── chgmem.sh             # rev/rvc.bit ファイルの RAM の初期値を書き換える
│     ├── config.sh             # Arty-A7 FPGAに rev/rvc.bit をコンフィグレーション
│     ├── program.sh            # Arty-A7 の spi flash にプログラム
│     └── rvc.sv                # Example design top module
├── ip                  # xilinx IP
│   ├── clk_gen.xcix     # clock 生成 PLL
│   └── xadcif.xcix      # XADC
├── rv_io/              # peripheral HDL source
│   ├── rv_pwm.sv        # LED control pwm unit
│   ├── rv_sio.sv        # UART 
│   └── rv_xadcif.sv     # XADC controler
└── rvmon               # デバッグ用モニタープログラム
     ├── Makefile
     ├── convmem.py        # メモリ初期パターン変換スクリプト
     ├── crt0.S            # スタートアップルーチン、割り込みハンドラ
     ├── rvmon.c           # モニタープログラム
     ├── lnkscr.x          # linker script
     ├── include/
     ├── lib/              # mini stdio (printf etc.)
     ├── app/              # テストプログラム例
     └── term              # シリアルターミナルプログラム
       ├── Makefile
       └── term.c
```

```verilog title="rvc.sv"
module rvc #( parameter debug = 0 ) (
  input  logic clk,     // Arty-A7 のシステムクロック(100MHz)を入力、rvc内部のPLLでCPUクロックを生成
  input  u8_t  pin,     // 8bit pararell 入力 SW に接続
  output u8_t  pout,    // 8bit pararell 出力 LED に接続
  input  logic rxd,     // シリアルターミナル
  output logic txd,
  input vauxp1,         // analog signal connection
   :
  input vauxn10,
  output u12_t led      // LED drive pwm 
  );
```
入出力ポートの接続先は、`arty-a7-pinassign.xdc` を参照。  

## 処理の流れ  

1. rv_core で実行する program を用意  
   モニタープログラム rvmon  
   クロスコンパイル環境を用意し、rvmon をコンパイル → 実行バイナリ(rvmon)を作成し、メモリの初期パターンに読み込む形式 (prog_u.mem, prog_l.mem) に変換する。  
   ```bash
   $ cd rv32emc/rvmon
   $ make         # rvmon を作成
   $ make install # ../rv_core/syn/ にメモリ初期パターンをインストール
   # /opt/rv32e/bin/riscv32-unknown-elf-objcopy -O binary -S -R .stack rvmon rvmon.bin
   # od -An -tx4 -v rvmon.bin > rvmon.mem
   # ./convmem.py rvmon.mem
   # cp -p prog_u.mem prog_l.mem ../rv_core/syn/
   # cp -p rvmon ../rv_core/syn/rvmon.elf
   ```
   rvmon.mem は 論理シミュレーション時、論理合成時に verilog の $readmemh() で読み込む形式。  
   dpram では 32bit ワードの 上位、下位 16bit に分けた初期化ファイルを読み込むので、スクリプトで変換する。  
   ```verilog title="dpram.sv"
      dpram #(.ADDR_WIDTH(13),
              .init_file_u("prog_u.mem"), // upper 16bit (31:16) initial data
              .init_file_l("prog_l.mem")  // lower 16bit  (15:0) initial data
              ) u_dpram (
   ```

2. 論理合成、コンフィグレーション  
   Vivado (2020.2) で論理合成。  
   build.sh で合成時、prog_u.mem  prog_l.mem  ファイルで RAM を初期化。  
   コンフィグレーション完了後、0x0 番地から実行を開始する。  
   ```bash
   $ cd rv32emc/rv_core/syn
   $ ./build.sh  # vivado で hdl ソースを読み込み、論理合成
   # FPGAのプログラム rev/rvc.bit を生成
   # rev/ の下に各種レポートを生成
   $ ./config.sh   # bit file を FPGA にロード  (script 中の hw_server, hw_target を環境に合わせて変更)  
   $ ./program.sh  # bit file を Arty-A7 の ROM にプログラム  
   ```
3. 合成済み bit file 中の RAM の内容を書き換える  
   合成時、write_mmi.tcl で block RAM の配置情報をもとに、メモリマッピング情報ファイル `prog.mmi` を出力している。  
   Vivado の updatemem ツールで bit file を読み込み、`prog.mmi` にしたがって BRAM の初期値をクロスコンパイルで生成した実行ファイル(elf 形式)の内容に書き換えて出力することができる。  
   ```bash
   # chgmem.sh
   $ updatemem -meminfo prog.mmi -data $elf -bit rev/rvc.bit -proc u_dpram -force -out rvcchg.bit
   ```  
   再合成することなく BRAM の初期値を書き換えることができる。  
   write_mmi.tcl は dpram の構成に対応して記述される必要がある。  

## プログラム実行  

Arty-A7 にモニタープログラム(rvmon) をロードし、USB-UART 経由でターミナル接続する。  
rvmon 用のターミナルプログラム(term)を用意した。 Host から S-format 形式のプログラムをメモリにロードするのが容易になる。  
rvmon で 'l *cmd*' コマンドと打つと、term に対して '\033<*cmd*\n'  を送出する。term は '\033<' を検出すると、'*cmd*.mot' ファイルを open し、*cmd*.mot の内容を rvmon にダウンロードする。  
*cmd*.mot の終了時、EOT ('\004') を送出し、ダウンロードを終了する。  

```bash
$ cd rv32emc/rvmon/term  
$ make
$ ./term
0 : usb-Xilinx_JTAG+Serial_1234-oj1-if01-port0
1 : usb-Digilent_Digilent_USB_Device_210319B26865-if01-port0
select port (0~1) : 1
*** open '/dev/serial/by-id/usb-Digilent_Digilent_USB_Device_210319B26865-if01-port0'

rvmon$ help
    d  {addr}      : dump memory
    l  {cmd(.mot)} : load S-record file # cmd.mot ファイルをメモリにダウンロード。0x4000 から実行する。
    go             : exec cmd (@0x4000)

rvmon$     # Ctrl-Alt-'C' で term 終了
```

```
lrwxrwxrwx 1 root root 13 Apr 23 08:45 /dev/serial/by-id/usb-Digilent_Digilent_USB_Device_210319B26865-if01-port0 -> ../../ttyUSB2  
crw-rw---- 1 root dialout 188, 2 Apr 23 22:45 /dev/ttyUSB2  
** 'dialout' group をサブグループに加えておくと、ユーザー権限でターミナルにアクセスできる。  
```

## サンプルプログラム

rv32emc/rvmon/app にいくつかのサンプルプログラムを用意した。  
rvmon でロードし、FPGA 上で実行する。  

name  |descrip
----  |----
pi    |多倍長演算で多数桁の $\pi$ を求める
ecc   |Reed Solomon エラー訂正
gauss |正規分布ランダム発生(soft float のテスト)  
adcpwm|Arty-A7 の XADC で電圧電流測定、カラー LED の点灯、btn で明るさ等変更  

```bash
$ cd rv32emc/rvmon
$ make apps
make -C app install
make[1]: ディレクトリ 'xxx/rv32emc/rvmon/app' に入ります
/opt/rv32e/bin/riscv32-unknown-elf-gcc -g -Wall -O2 -march=rv32emc -mabi=ilp32e -nostartfiles -I../include  -c gauss.c
/opt/rv32e/bin/riscv32-unknown-elf-gcc -g -Wall -O2 -march=rv32emc -mabi=ilp32e -nostartfiles -I../include  -c crt0.c
/opt/rv32e/bin/riscv32-unknown-elf-gcc -o gauss.elf crt0.o gauss.o -march=rv32emc -mabi=ilp32e -Wl,-Map,gauss.map,-T,lnkscr.x -nostdlib -L../lib -Xlinker -R../rvmon -lmc -lm -lc -lgcc
/opt/rv32e/bin/riscv32-unknown-elf-objcopy -O srec --srec-forceS3 -S -R .stack gauss.elf gauss.mot
/opt/rv32e/bin/riscv32-unknown-elf-gcc -g -Wall -O2 -march=rv32emc -mabi=ilp32e -nostartfiles -I../include  -c pi.c
/opt/rv32e/bin/riscv32-unknown-elf-gcc -o pi.elf crt0.o pi.o -march=rv32emc -mabi=ilp32e -Wl,-Map,pi.map,-T,lnkscr.x -nostdlib -L../lib -Xlinker -R../rvmon -lmc -lm -lc -lgcc
/opt/rv32e/bin/riscv32-unknown-elf-objcopy -O srec --srec-forceS3 -S -R .stack pi.elf pi.mot
/opt/rv32e/bin/riscv32-unknown-elf-gcc -g -Wall -O2 -march=rv32emc -mabi=ilp32e -nostartfiles -I../include  -c ecc.c
/opt/rv32e/bin/riscv32-unknown-elf-gcc -o ecc.elf crt0.o ecc.o -march=rv32emc -mabi=ilp32e -Wl,-Map,ecc.map,-T,lnkscr.x -nostdlib -L../lib -Xlinker -R../rvmon -lmc -lm -lc -lgcc
/opt/rv32e/bin/riscv32-unknown-elf-objcopy -O srec --srec-forceS3 -S -R .stack ecc.elf ecc.mot
cp -p gauss.mot pi.mot ecc.mot ../term/     # term のディレクトリにインストールする
rm crt0.o
make[1]: ディレクトリ 'xxx/rv32emc/rvmon/app' から出ます
```
サンプルプログラムは 0x4000 番地から実行するようにリンクされる。(lnkscr.x 参照)  
また、リンクオプション `-Xlinker -R../rvmon` により、モニタプログラム rvmon に含まれるライブラリをリンクする(memory 節約)。  

```bash title="term でのサンプルプログラム実行"
rvmon$ l pi     # pi.mot(円周率計算プログラム) をメモリにロード、0x4000 番地から実行
................................................................................................
..................................................................................
[13062 bytes tfr    cs:691498]
rvmon$ go       # ロードしたプログラムを call 0x4000 
run user func : 4000
iter:679
3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211
  :
2620522489407726719478268482601476990902640136394437455305068203496252451749399651431429809190659
2509372216964615157098583874105978859597729754989301617539284681382686838689427741559918559252459
53959431049972524680845987273644695848653836736222626099124608057
cs : 2069485033 OK
elapsed : 2288.07ms
0
rvmon$  l adcpwm  # XADC LED テスト
.................................................................................................
[24392 bytes tfr    cs:1301413]
rvmon$ go
run user func : 4000

'q' for quit.
 LED amp: 1.0  rate:1.0  sw:00   # Arty-A7 の btn0~3 で LED の明るさ(amp)、色の更新レート(rate) を up/down
temp: 38.5 deg|*********|*********|*********|*********    # XADC により、chip 温度、各種電圧、電流を測定し表示
V5v :  4.93 V |*********|*********|*********|*********|*********
VU  : 11.92 V |*********|*********|*********|*********|*********|*********|
Va4 : 1.259 V |*********|***
Va5 : 0.006 V |
IU  : 0.241 A |*********|*********|****
Iint: 0.119 A |*********|**
0
rvmon$ 
```

coremark をコンパイルするスクリプト coremark.sh を用意した。  

```bash
$ cd rv32emc/rvmon/app
$ sh coremark.sh
Cloning into 'coremark'...  # https://github.com/eembc/coremark.git から clone  
Receiving objects: 100% (338/338), 494.88 KiB | 4.16 MiB/s, done.
make clean -C coremark
make PORT_DIR=`pwd`/rv32port -C coremark
cp coremark/coremark.exe coremark.elf
/opt/rv32e/bin/riscv32-unknown-elf-objcopy -O srec --srec-forceS3 -S -R .stack  coremark.elf coremark.mot
/opt/rv32e/bin/riscv32-unknown-elf-objdump -D -S -h coremark.elf > coremark.lst
cp coremark.mot ../term/
```
```bash title="term での coremark 実行"
rvmon$ l coremark
.............................................................................
[25054 bytes tfr    cs:1321594]

rvmon$ go 0 0 0x66        # 引数を渡す。 0 0 0x66 -> performance run / 0x3415 0x3415 0x66 -> validation run
run user func : 4000
_bss:6088 _end:60a8 sp:ffc8 gp:3190
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 829247167
Total time (secs): 13
Iterations/Sec   : 153
Iterations       : 2000
Compiler version : GCC9.2.0
Compiler flags   : -O2 -march=rv32emc -mabi=ilp32e   -Wl,-Map,coremark.map,-T,/home/xxx/rv32emc/rvmon/app/rv32port/lnkscr.x -nostdlib -L/home/xxx/rv32emc/rvmon/app/rv32port/../..//lib -Xlinker -R/home/xxx/rv32emc/rvmon/app/rv32port/../..//rvmon -lmc -lm -lc -lgcc
Memory location  : STACK
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x4983
Correct operation validated. See README.md for run and reporting rules.
0
rvmon$ 
```
clock = 60MHz での結果  
CoreMark/MHz = 2.55 / GCC9.2.0 -O2 / Stack  
