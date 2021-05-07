
#include <stdint.h>

typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

typedef int64_t  s64;
typedef int32_t  s32;
typedef int16_t  s16;
typedef int8_t   s8;

#define XLEN	32	// word length
#define NREG	32	// # of register

// IO area
#define IOBEGIN 0xffff0000
#define IOEND   0xffffffff

// IO deffs
#define DBG_PUTC    0xffff0004
#define MTIME       0xffff8000
#define MTIMECMP    0xffff8008


u32 simrun (u32 addr, int steps, int reset);
u32 simtrace (u32 addr, int steps, int reset);
int disasm(int adr, char *dat, char *opc, char *opr, int *dsp);
void reg_dump();

char mem_rd(int adr);

char *search_symbol(u32 pc);

