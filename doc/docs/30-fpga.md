
# FPGA 実装

rv_core の動作を Xilinx Artix7 FPGA で確認するために example design を用意した。  
[Taget board : Arty-A7](x_env.html#arty-a7-35t)  

[source(github)](https://github.com/shin-yamashita/rv32emc/tree/main/rv_core)  rv32emc/rv_core/  

## example design  

rv_core とメモリ dpram、シリアル通信回路 rv_sio を接続した回路 rvc.sv を用意し、Arty-A7-35 ボードのターミナルで動作を確認する。
```
├── rv_core
│   ├── hdl/            # rv_core HDL source
│   └── syn
│     ├── build.sh              # 論理合成スクリプト
│     ├── build.tcl             # vivado tcl scripts
│     ├── read_hdl.tcl          # HDL source を読み込む
│     ├── write_mmi.tcl
│     ├── arty-a7-pinassign.xdc # 制約ファイル、timing / pin assign
│     ├── chgmem.sh             # rev/rvc.bit ファイルの RAM の初期値を書き換える
│     ├── config.sh             # Arty-A7 FPGAに rev/rvc.bit をコンフィグレーション
│     ├── program.sh            # Arty-A7 の spi flash にプログラム
│     ├── rvc.sv                # top module
│     └── clk_gen.xcix          # clock 生成 PLL
├── rv_io/              # rv_sio UART HDL source
└── rvmon               # デバッグ用モニタープログラム
    ├── Makefile
    ├── convmem.py      # メモリ初期パターン変換スクリプト
    ├── crt0.S          # スタートアップルーチン、割り込みハンドラ
    ├── rvmon.c         # モニタープログラム
    ├── lnkscr.x        # linker script
    ├── include/
    ├── lib/            # mini stdio (printf etc.)
    ├── app/            # テストプログラム例
    └── term            # シリアルターミナルプログラム
      ├── Makefile
      └── term.c
```
```verilog title="rvc.sv"
module rvc #( parameter debug = 0 ) (
  input  logic clk,     // Arty-A7 のシステムクロック(100MHz)を入力、rvc内部のPLLでCPUクロックを生成
  input  u8_t  pin,     // 8bit pararell 入力 SW に接続
  output u8_t  pout,    // 8bit pararell 出力 LED に接続
  input  logic rxd,     // シリアルターミナル
  output logic txd
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
   ```bash
   $ cd ../rv_core/syn
   $ ./build.sh  # vivado で hdl ソースを読み込み、論理合成
   # FPGAのプログラム rev/rvc.bit を生成
   # rev/ の下に各種レポートを生成
   $ ./config.sh   # bit file を FPGA にロード  
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
$ cd ../../rvmon/term  
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

name |descrip
---- |----
pi   |多倍長演算で多数桁の $\pi$ を求める
ecc  |Reed Solomon エラー訂正
gauss|正規分布ランダム発生(soft float のテスト)  

```bash
$ cd ../../rvmon
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
サンプルプログラムは 0x4000 番地から実行するようにリンクされる。
また、リンクオプション `-Xlinker -R../rvmon` により、モニタプログラム rvmon に含まれるライブラリをリンクする(memory 節約)。

```
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
rvmon$ 
```

