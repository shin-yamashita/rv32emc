
# rv_core/syn FPGA 合成環境

## example design  

**Taget board : Arty-A7-35**  

rv_core の動作を Xilinx Artix7 で確認する。  
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
```verilog
// rvc.sv
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
    ```verilog title=dpram.sv
    // dpram.sv
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
   ```bash title=chgmem.sh
   # chgmem.sh
   $ updatemem -meminfo prog.mmi -data $elf -bit rev/rvc.bit -proc u_dpram -force -out rvcchg.bit
   ```
   再合成することなく BRAM の初期値を書き換えることができる。  
   write_mmi.tcl は dpram の構成に対応して記述される必要がある。  



