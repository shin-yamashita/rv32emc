
# rv-test テストプログラム

rv32emc の動作確認に用いたテストプログラム  
簡単化した printf() を含む lib/libmc.a を用いて実行結果を print 出力し、確認する  
printf() から呼ばれる文字列出力の下位関数 _write() を実行環境に合わせて用意する  

**lib/fprint.c**
```C 
#define DBG_PUTC        ((volatile u8*)0xffff0004)  // debug 用キャラクタ出力ポート

ssize_t _write (int fd, const char *ptr, size_t len)
{
#if 0   // system call を利用する場合 (for cross-gdb)
  asm("li t0,64");  // SYS_write
  asm("ecall");
  return len;
#else   // I/O レジスタ書き込みを利用する場合
  ssize_t c = 0;
  while(len-- && *ptr){
    *DBG_PUTC = *ptr++;
    c++;
  }
  return c;
#endif
}
```
_write() 関数を変えることで、ISS / RTL sim / FPGA実機 / cross-gdb、それぞれの環境で結果確認を行う。  

- rvsim での動作確認  
  printf() で rvsim へのコンソール出力、RTLに合わせた IO 出力を emulate  
  　(rvsim/simcore.c : IO_write(u32 adr, u16 mmd, u32 wd) 参照)  
  syscall も使える、cross-gdb と突き合わせてのデバッグ用  
  　(rvsim/simcore.c : syscall(u32 func, u32 a0, u32 a1, u32 a2) 参照)  

- RTL sim での動作確認  
  printf() で file 'stderr.out' へ出力  
  　(sim/rvc.sv : $fwrite(STDERR, "%c", d_dw[7:0]); 参照)  

- FPGA での実機確認  
  _write() 関数を UART への送信関数 uart_write() にして、シリアルターミナルに出力  
  　(rvmon/lib/fprint.c, uartdrv.c 参照)  

- riscv-gnu-toolchain の cross-gdb での動作確認  
  _write() 関数を、syscall にすることで、cross-gdb での動作確認ができる  


```
rv-test
├── Makefile
├── crt0.S          RISC-V startup routine, interrupt handler
├── ecc.c           RS error correction test
├── fptest.c        float(soft float) test
├── gauss.c         float test
├── pi.c            calc pi test
├── include
│   ├── stdint.h
│   ├── stdio.h
│   ├── time.h
│   ├── types.h
│   ├── uartdrv.h
│   └── ulib.h
├── lib             mini stdio library
│   ├── Makefile
│   ├── fprint.c
│   ├── uartdrv.c
│   └── ulib.c
└── rv32ldscr.x     linker script
```

## risc-v cross gcc の build
rv32emc のコンパイルには、riscv-gnu-toolchain の gcc を用いた。  
EMC 命令セットに対応するため、以下のように configure, build を行った  

```bash
$ sudo apt install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk \
        build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev  
                        # gcc の build で必要となるツール Prerequisites  
$ sudo mkdir /opt/rv32e # cross tool のインストール先  

$ git clone --branch rvv-0.9.x --recursive https://github.com/riscv/riscv-gnu-toolchain  
$ cd riscv-gnu-toolchain  

$ patch -p1 < riscv-gdb-sim.patch   # run の修正 patch

$ ./configure --prefix=/opt/rv32e --disable-linux --with-arch=rv32emac --with-abi=ilp32e  
$ make newlib   # ツール群を /opt/rv32e/ にインストール、書き込み権限が必要
```
gcc 9.2.0 で動作を確認した。  
(ただし、最新の gcc 11.1.0 では動作がおかしい、詳細は未確認)  　


