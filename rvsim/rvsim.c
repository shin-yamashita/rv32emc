//
// rvsim.c
//
// RV32IMC/RV32EMC ISS
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <bfd.h>
#include <elf.h>

#include "monlib.h"
#include "simcore.h"
#include "consio.h"

//--- rvsim grobals -------------------------

#define MAXARG	50

static int debug = 0;
extern int LPP;

bfd_byte    *memory = NULL;
bfd_byte    *stack = NULL;
bfd_size_type   memsize = 0;
bfd_size_type   stacksize = 0x20000;
bfd_vma     vaddr = (bfd_vma)-1;
bfd_vma     stack_top = 0x3ffffd0;
bfd         *abfd = NULL;

asymbol **symbol_table;
long number_of_symbols = 0;

u32     _end_adr;
u32     heap_ptr = 0x0;

static u32 simadr;
int     view_reg[100] = {1,2,3,10,11,12,13,20,21,0};
int     nview = 9;
int     reset = 1;
int     sys_exit = 0;
int     arch = 'i';
int     compress = 0;
int     Nregs = 32;

#define MAXBRK 100
int     n_break = 0;
u32     break_adr[MAXBRK];
char   *break_sym[MAXBRK];
int     break_en[MAXBRK];

FILE    *ofp;
//---------------------------------------------

char *program_name = "rvsim";

void
bfd_nonfatal (const char *string)
{
    const char *errmsg = bfd_errmsg (bfd_get_error ());

    if (string)
        fprintf (stderr, "%s: %s: %s\n", program_name, string, errmsg);
    else
        fprintf (stderr, "%s: %s\n", program_name, errmsg);
}

static void
dump_section_header (bfd *abfd, asection *section,
        void *ignored ATTRIBUTE_UNUSED)
{
    char *comma = "";
    unsigned int opb = bfd_octets_per_byte (abfd, section);

    /* Ignore linker created section. */
    if (section->flags & SEC_LINKER_CREATED)
        return;

    printf ("%3d %-16s %8lx ", section->index,
            bfd_section_name (section),
            (unsigned long) bfd_section_size (section) / opb);	//, section);
    bfd_printf_vma (abfd, bfd_section_vma (section));
    printf ("  ");
    bfd_printf_vma (abfd, section->lma);
    printf ("  %08lx  2**%u  ", (unsigned long) section->filepos,
            bfd_section_alignment (section));

#define PF(x, y) \
        if (section->flags & x) { printf ("%s%s", comma, y); comma = ", "; }

    PF (SEC_HAS_CONTENTS, "CONTENTS");
    PF (SEC_ALLOC, "ALLOC");
    PF (SEC_CONSTRUCTOR, "CONSTRUCTOR");
    PF (SEC_LOAD, "LOAD");
    PF (SEC_RELOC, "RELOC");
    PF (SEC_READONLY, "READONLY");
    PF (SEC_CODE, "CODE");
    PF (SEC_DATA, "DATA");
    PF (SEC_ROM, "ROM");
    PF (SEC_DEBUGGING, "DEBUGGING");
    PF (SEC_NEVER_LOAD, "NEVER_LOAD");
    PF (SEC_EXCLUDE, "EXCLUDE");
    PF (SEC_SORT_ENTRIES, "SORT_ENTRIES");
    PF (SEC_SMALL_DATA, "SMALL_DATA");
    PF (SEC_THREAD_LOCAL, "THREAD_LOCAL");
    PF (SEC_GROUP, "GROUP");
    if ((section->flags & SEC_LINK_ONCE) != 0)
    {
        const char *ls;

        switch (section->flags & SEC_LINK_DUPLICATES)
        {
        default:
            abort ();
        case SEC_LINK_DUPLICATES_DISCARD:
            ls = "LINK_ONCE_DISCARD";
            break;
        case SEC_LINK_DUPLICATES_ONE_ONLY:
            ls = "LINK_ONCE_ONE_ONLY";
            break;
        case SEC_LINK_DUPLICATES_SAME_SIZE:
            ls = "LINK_ONCE_SAME_SIZE";
            break;
        case SEC_LINK_DUPLICATES_SAME_CONTENTS:
            ls = "LINK_ONCE_SAME_CONTENTS";
            break;
        }
        printf ("%s%s", comma, ls);
        comma = ", ";
    }

    printf ("\n");
#undef PF
}

static void
dump_headers (bfd *abfd)
{
    printf ("Sections:\n");
    printf ("Idx Name                 Size  VMA       LMA       File off  Algn  Flags\n");

    bfd_map_over_sections (abfd, dump_section_header, NULL);
}

static void
dump_bfd_header (bfd *abfd)
{
    char *comma = "";


#define PF(x, y)    if (abfd->flags & x) {printf("%s%s", comma, y); comma=", ";}
    PF (HAS_RELOC, "HAS_RELOC");
    PF (EXEC_P, "EXEC_P");
    PF (HAS_LINENO, "HAS_LINENO");
    PF (HAS_DEBUG, "HAS_DEBUG");
    PF (HAS_SYMS, "HAS_SYMS");
    PF (HAS_LOCALS, "HAS_LOCALS");
    PF (DYNAMIC, "DYNAMIC");
    PF (WP_TEXT, "WP_TEXT");
    PF (D_PAGED, "D_PAGED");
    PF (BFD_IS_RELAXABLE, "BFD_IS_RELAXABLE");
    //  PF (HAS_LOAD_PAGE, "HAS_LOAD_PAGE");
    printf ("\nstart address 0x");
    bfd_printf_vma (abfd, abfd->start_address);
    printf ("\n");
}

void load_symtab(bfd *abfd)
{
    int i;
    int storage_needed = bfd_get_symtab_upper_bound (abfd);
    if (storage_needed <= 0) {
        fprintf (stderr, "Error storage_needed < 0\n");
        return ;
    }
    symbol_table = (asymbol **) malloc (storage_needed);
    number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);

    if (number_of_symbols < 0) {
        fprintf (stderr, "Error: number_of_symbols < 0\n");
        return ;
    }
    for (i = 0; i < number_of_symbols; i++) {
        if ((symbol_table[i]->flags & BSF_GLOBAL) ){
            if(!strcmp(bfd_asymbol_name (symbol_table[i]), "_end")){
                _end_adr = bfd_asymbol_value (symbol_table[i]);
                break;
            }
        }
    }
    if(debug){
        for (i = 0; i < number_of_symbols; i++) {
            ////    fprintf (stdout, "%08x %08x ", bfd_asymbol_base (symbol_table[i]), bfd_asymbol_value (symbol_table[i]));
            fprintf (stdout, "%08lx ", bfd_asymbol_value (symbol_table[i]));
            if ((symbol_table[i]->flags & BSF_FUNCTION) != 0x00) {
                fprintf (stdout, "Func   : %s\n", bfd_asymbol_name (symbol_table[i]));
                //} else if ((symbol_table[i]->flags & BSF_LOCAL) != 0x00) {
                //    fprintf (stdout, "Local ");
            } else if ((symbol_table[i]->flags & BSF_GLOBAL) != 0x00) {
                fprintf (stdout, "global : %s\n", bfd_asymbol_name (symbol_table[i]));
            }

            //        if(symbol_table[i]->flags & BSF_FUNCTION)       // || symbol_table[i]->flags & BSF_GLOBAL)
            //          show_debug_info(bfd_asymbol_value (symbol_table[i]));
        }
    }
}

char *search_symbol(u32 pc)
{
    int i;
    for (i = 0; i < number_of_symbols; i++) {
        if((symbol_table[i]->flags & (BSF_FUNCTION|BSF_GLOBAL)) && bfd_asymbol_value (symbol_table[i]) == pc)
            return (char*)bfd_asymbol_name (symbol_table[i]);
    }
    return NULL;
}

u32 get_symbol_addr(char *symbol)
{
    int i;
    for (i = 0; i < number_of_symbols; i++) {
        if((symbol_table[i]->flags & (BSF_FUNCTION|BSF_GLOBAL))
                && !strcmp(symbol, (char*)bfd_asymbol_name (symbol_table[i])))
            return bfd_asymbol_value(symbol_table[i]);
    }
    return strtoul(symbol, NULL, 16);
}

static void load_section (bfd *abfd, asection *section, void *dummy ATTRIBUTE_UNUSED)
{
    bfd_byte *data = 0;
    bfd_size_type datasize;

    if ((section->flags & SEC_ALLOC) == 0) return;
    if ((datasize = bfd_section_size (section)) == 0)
        return;
//    printf (" section %s:\n", section->name);
    if(vaddr == (bfd_vma)-1) vaddr = section->vma;
    memsize = section->vma - vaddr + datasize;
    memory = (bfd_byte*) realloc(memory, memsize);
    data = &memory[section->vma-vaddr];
    if ((section->flags & SEC_HAS_CONTENTS) == 0) return;
    bfd_get_section_contents (abfd, section, data, 0, datasize);
}

#define HEAP_SIZE	1024*16

//----- elf header  riscv
typedef struct elf_internal_ehdr {      // from  binutils/include/elf/internal.h
  unsigned char         e_ident[EI_NIDENT]; /* ELF "magic number" */
  bfd_vma               e_entry;        /* Entry point virtual address */
  bfd_size_type         e_phoff;        /* Program header table file offset */
  bfd_size_type         e_shoff;        /* Section header table file offset */
  unsigned long         e_version;      /* Identifies object file version */
  unsigned long         e_flags;        /* Processor-specific flags */
  unsigned short        e_type;         /* Identifies object file type */
  unsigned short        e_machine;      /* Specifies required architecture */
  unsigned int          e_ehsize;       /* ELF header size in bytes */
  unsigned int          e_phentsize;    /* Program header table entry size */
  unsigned int          e_phnum;        /* Program header table entry count */
  unsigned int          e_shentsize;    /* Section header table entry size */
  unsigned int          e_shnum;        /* Section header table entry count */
  unsigned int          e_shstrndx;     /* Section header string table index */
} Elf_Internal_Ehdr;
struct elf_obj_tdata
{
  Elf_Internal_Ehdr elf_header[1];      /* Actual data, but ref like ptr */
};
#define elf_tdata(bfd)          ((bfd) -> tdata.elf_obj_data)   // from binutils/bfd/elf-bfd.h
#define EF_RISCV_RVE 0x0008
#define EF_RISCV_FLOAT_ABI 0x0006

//----------------------------

static void load_abs(char *filename, int verb)
{
    char **matching;

    if(abfd) bfd_close (abfd);

    if((abfd = bfd_openr (filename, NULL)) == NULL){
        bfd_nonfatal (filename);
    } else if(! bfd_check_format_matches(abfd, bfd_object, &matching)){
        bfd_close (abfd);
        abfd = NULL;
    } else {
        if (abfd->xvec->flavour == bfd_target_elf_flavour){
          unsigned long e_flags = elf_tdata(abfd)->elf_header->e_flags;

          if(verb) printf(" load ELF file '%s' :", filename);
          int elfclass = elf_tdata(abfd)->elf_header->e_ident[4];
          if(elfclass != 1){
              printf("\n not a elf32 excutable\n");
              return;
          }
          if(elf_tdata(abfd)->elf_header->e_machine == EM_RISCV) {
              if(verb) printf(" RISCV");
          } else {
              printf("\n not a risc-v excutable\n");
              return;
          }
          //printf("e_flags:   %lx\n", e_flags);
          if(e_flags & EF_RISCV_RVC) {
              compress = 'c';
              if(verb) printf(" RVC");
          }
          if(e_flags & EF_RISCV_RVE) {
              arch = 'e';
              if(verb) printf(" RVE");
          }
          if(verb) printf("\n");
          if(e_flags & EF_RISCV_FLOAT_ABI){
              printf(" float insn not supported.\n");
              return;
          }

          bfd_map_over_sections (abfd, load_section, NULL);
          load_symtab(abfd);
          if(verb) printf (" number_of_symbols = %ld\n", number_of_symbols);
          if(verb) printf(" vaddr:%x memsize:%x _end:%x start:%x\n", (unsigned)vaddr, (unsigned)memsize, _end_adr, (u32)abfd->start_address);
          memsize += HEAP_SIZE;
          memory = (bfd_byte*) realloc(memory, memsize);
        } else {
            printf(" not a elf excutable.\n");
        }
    }
}

void Header(int argc, char *argv[])
{
    if(!abfd) return;
    dump_bfd_header (abfd);
    bfd_print_private_bfd_data (abfd, stdout);
    dump_headers(abfd);
}
void List_symbol(int argc, char *argv[])
{
    int i;

    for (i = 0; i < number_of_symbols; i++) {
        if ((symbol_table[i]->flags & BSF_GLOBAL) != 0x00) {
            fprintf (stdout, "%08lx global : %s\n", bfd_asymbol_value (symbol_table[i]), bfd_asymbol_name (symbol_table[i]));
        }
    }
    for (i = 0; i < number_of_symbols; i++) {
        if ((symbol_table[i]->flags & BSF_FUNCTION) != 0x00) {
            fprintf (stdout, "%08lx Func   : %s\n", bfd_asymbol_value (symbol_table[i]), bfd_asymbol_name (symbol_table[i]));
        }
    }
}

void Info(int argc, char *argv[])
{
    if(!abfd) return;
    printf(" architecture : RV32");
    if(arch=='e') putchar('E');
    if(compress=='c') putchar('C');
    printf("\n vaddr     (memsize)   : %8x (%ld)\n", (u32)vaddr, memsize);
    printf(" stack_top (stacksize) : %8x (%ld)\n", (u32)stack_top, stacksize);
    printf(" heap_ptr  (heapsize)  : %8x (%d)\n", heap_ptr, HEAP_SIZE);
    printf(" end_addr              : %8x\n", _end_adr);
    printf(" start_address         : %8x\n", (u32)abfd->start_address);
}

void RegDump(int argc, char *argv[])
{
    reg_dump();
}

void Dump(int argc, char *argv[])
{
    if(!abfd) return;
    int i, j, aofs = vaddr, padr, psize = memsize;
    bfd_byte *mem = memory;
    int adr = abfd->start_address;
    int c;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-stk")){
            mem = stack;
            aofs = stack_top-stacksize;
            psize = stacksize;
            adr = stack_top-512;
        }else if(isxdigit(*argv[i])){
            adr = get_symbol_addr(argv[i]);
//            adr = strtol(argv[i], NULL, 16);
        }
    }
    initerm();
    do{
        if(adr >= vaddr && adr < vaddr+memsize){
            mem = memory;
            aofs = vaddr;
            psize = memsize;
        }else if(adr >= stack_top-stacksize && adr <= stack_top+0x100){
            mem = stack;
            aofs = stack_top-stacksize;
            psize = stacksize;
        }
        printf(CLS "      : ");
        for(j = 0; j < 16; j++) printf(" %x ", j);
        printf("\n");

        for(i = 0; i < 256; i+=16){
            printf("%06x: ", adr);
            for(j = 0; j < 16; j++){
                padr = adr - aofs + j;
                if(padr >= 0 && padr < psize)
                    printf("%02x ", (unsigned)mem[padr]);
                else
                    printf("-- ");
            }
            for(j = 0; j < 16; j++){
                padr = adr - aofs + j;
                if(padr >= 0 && padr < psize){
                    c = mem[padr];
                    printf("%c", isprint(c) ? c : '.');
                }else
                    printf("-");
            }
            adr += j;
            printf("\n");
        }
        c = inchr();
        c = esc_seq(c);
        switch(c){
        case 'b': adr -= 256*2; break;
        case 'n': break;
        case K_Up: adr -= 256+16; break;
        case K_Down: adr -= 256-16; break;
        case K_PgUp: adr -= 256*2; break;
        case K_PgDn: break;
        default: adr -= 256; break; // stay
        }
//        if(c == 'b') adr -=1024;
    }while(c != 'q');
    deinitrm();
}

void disasm_all()
{
    if(!abfd) return;
    char str[80], opstr[80], oprstr[80], *sym, *dpsym;
    int inc, dsp;
    u32 adr = vaddr;
    while(adr <= _end_adr){
        inc = disasm(adr, str, opstr, oprstr, &dsp);
        sym = search_symbol(adr);
        dpsym = dsp > 0 ? search_symbol(dsp) : NULL;
        printf("%06x : %s%-20s %-8s %s %s\n", adr, str, sym?sym:"", opstr, oprstr, dpsym?dpsym:"");
        adr += inc;
    }
}

void Dis(int argc, char *argv[])
{
    if(!abfd) return;
    char str[80], opstr[80], oprstr[80], *sym, *dpsym;
    int i, inc, c;
    u32 adr = vaddr;
    int dsp;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-all")){
            disasm_all();
            return;
        }else{
            adr = get_symbol_addr(argv[i]);
        }
    }

    if(adr < vaddr || adr >= vaddr+memsize){
        printf(" ill address '%x'.\n", adr);
        return ;
    }
    initerm();
    for(i = 0; i < LPP-2; i++){
        inc = disasm(adr, str, opstr, oprstr, &dsp);
        sym = search_symbol(adr);
        dpsym = dsp > 0 ? search_symbol(dsp) : NULL;
        printf("%06x : %s%-20s %-8s %s %s\n", adr, str, sym?sym:"", opstr, oprstr, dpsym?dpsym:"");
        adr += inc;
    }
    do{
        c = inchr();
        switch(c){
        case 'b':
            adr -= LPP * 3;
        case ' ':
            for(i = 0; i < LPP-2; i++){
                inc = disasm(adr, str, opstr, oprstr, &dsp);
                sym = search_symbol(adr);
                dpsym = dsp > 0 ? search_symbol(dsp) : NULL;
                printf("%06x : %s%-20s %-8s %s %s\n", adr, str, sym?sym:"", opstr, oprstr, dpsym?dpsym:"");
                adr += inc;
            }
            break;
        case '\n':
            inc = disasm(adr, str, opstr, oprstr, &dsp);
            sym = search_symbol(adr);
            dpsym = dsp > 0 ? search_symbol(dsp) : NULL;
            printf("%06x : %s%-20s %-8s %s %s\n", adr, str, sym?sym:"", opstr, oprstr, dpsym?dpsym:"");
            adr += inc;
            break;
        default:
            break;
        }
    }while(c != 'q');
    deinitrm();
}

void Load(int argc, char *argv[])
{
    if(argc >= 2){
        load_abs(argv[1], 1);
        reset = 1;
        sys_exit = 0;
    }
}

void Break(int argc, char *argv[])
{
    int i, j;

    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-l")){
            break;
        }else if(!strcmp(argv[i], "-d")){
            if(++i >= argc) break;
            j = atoi(argv[i]);
            if(j < n_break){
                break_en[j] = 0;
                printf(" disable %d\n", j);
           }else{
                printf(" ill break point number '%s'\n", argv[i]);
            }
        }else if(!strcmp(argv[i], "-e")){
            if(++i >= argc) break;
            j = atoi(argv[i]);
            if(j < n_break){
                break_en[j] = 1;
                printf(" enable %d\n", j);
           }else{
                printf(" ill break point number '%s'\n", argv[i]);
           }
        }else{
            u32 adr = get_symbol_addr(argv[i]);
            if(adr){
                for(j = 0; j < n_break; j++){
                    if(adr == break_adr[j]) {
                        break_en[j] = 1;
                        printf("%3d : %8x %c %s\n", j, break_adr[j], break_en[j]?'@':'-', break_sym[j]);
                        return;
                    }
                }
                break_en[n_break] = 1;
                break_sym[n_break] = strdup(argv[i]);
                break_adr[n_break++] = adr;
                printf("%3d : %8x %c %s\n", j, break_adr[j], break_en[j]?'@':'-', break_sym[j]);
            }else{
                printf(" ill symbol or address '%s'\n", argv[i]);
            }
            return;
        }
    }
    for(j = 0; j < n_break; j++)
        printf("%3d : %8x %c %s\n", j, break_adr[j], break_en[j]?'@':'-', break_sym[j]);
}

void Run(int argc, char *argv[])
{
    if(!abfd) return;
    int i, n = 30;	//, adr;
    //  dump_data(abfd);
    for(i = 1; i < argc; i++){
        if(isdigit(*argv[i])){
            //			view_reg[nv++] = atoi(argv[i]);
            n = atoi(argv[i]);
        }else if(!strcmp(argv[i], "-all")){
            n = -1;
        }
    }
    //	if(nv) nview = nv;
    simadr = abfd->start_address;
    reset = 1;
    sys_exit = 0;
    heap_ptr = 0;
    simrun(simadr, n, reset);
    reset = 0;
}

void Cont(int argc, char *argv[])
{
    if(!abfd) return;
    int i;
    static int n = 30;
    for(i = 1; i < argc; i++){
        if(isdigit(*argv[i])){
            n = atoi(argv[i]);
        }else if(!strcmp(argv[i], "-all")){
            n = -1;
        }
    }
    simadr = abfd->start_address;
    simrun(simadr, n, reset);
    reset = 0;
}

void Trace(int argc, char *argv[])
{
    if(!abfd) return;
    int i, j, n = 0;
    char cmd[300];

    for(i = 1; i < argc; i++){
        if(isdigit(*argv[i])){
            n = atoi(argv[i]);
        }else if(!strcmp(argv[i], "-all")){
            n = -1;
        }else if(!strcmp(argv[i], "-r")){
            reset = 1;
            sys_exit = 0;
            heap_ptr = 0;
        }else if(!strcmp(argv[i], "|")){
            if(++i >= argc) break;
            j = 0;
            for(; i < argc; i++){
                sprintf(&cmd[j], "%s ", argv[i]);
                j = strlen(cmd);
            }
            ofp = popen(cmd, "w");
        }
    }
    simadr = abfd->start_address;
    printf("simadr:%x, n:%d, reset:%d, LPP:%d\n",simadr,n,reset,LPP);
    simtrace(simadr, n, reset);
    if(ofp != stdout){
        fflush(ofp);
        pclose(ofp);
    }
    ofp = stdout;
    reset = 0;
}

extern FILE *debfp_reg;
extern FILE *debfp_mem;

void DebugDump(int argc, char *argv[])
{
    int i;
    for(i = 1; i < argc; i++){
        if(!strcmp(argv[i], "-r")){
            if(++i >= argc) break;
            debfp_reg = fopen(argv[i], "w");
            if(debfp_reg){
                printf(" Register write log : '%s'\n", argv[i]);
                fprintf(debfp_reg, "insncount reg wdata label\n");
            }
        } else if(!strcmp(argv[i], "-m")){
            if(++i >= argc) break;
            debfp_mem = fopen(argv[i], "w");
            if(debfp_mem){
                printf(" Memory write log : '%s'\n", argv[i]);
                fprintf(debfp_mem, "insncount memadr wdata mode label-a label-d\n");
            }
        } else if(!strcmp(argv[i], "-c")){
            if(debfp_reg) {
                fclose(debfp_reg);
                debfp_reg = NULL;
            }
            if(debfp_mem) {
                 fclose(debfp_mem);
                 debfp_mem = NULL;
             }
        }
    }

}

void Exit(int argc, char *argv[])
{
    exit(0);
}

/*============== Command Table =================================================*/
void help(int argc, char *argv[]);

struct {
    void (*func) ();
    char *cmdstr, *menustr, *helpstr;
} CmdTab[] = {
        {Load,  "lo$ad",    "<file (rv32 executable)>", "\n             Load absolute binary (elf)."},
        {Dump,  "du$mp",    "<-stk> <addr|label>",      "\n             Dump memory."},
        {RegDump,"re$g",    "",                         "\n             Dump Register."},
        {Run,   "r$un",     "<N cyc|-all>",             "\n             Run simulation."},
        {Cont,  "c$ont",    "<N cyc|-all>",             "\n             Continue simulation."},
        {Trace, "t$race",   "<N cyc|-all> <-r> <| tee (fn)>", "\n             Trace simulation. -r : reset"},
        {Break, "b$reak",   "<addr|label> <-d|-e (n)>", "\n             Break point.  -d/-e : disable/enable"},
        {Info,  "in$fo",    "",                         "\n             Print simulator info."},
        {Header,"he$ader",  "",                         "\n             Print Header."},
        {List_symbol,"sy$mbol", "",                     "\n             Print symbol table."},
        {Dis,   "di$s",     "<addr|lable>",             "\n             Dis asm."},
        {DebugDump, "deb$ug","<-r (fn)> <-m (fn)> <-c>","\n             Register / Memory write log out."},
        {help,  "h$elp",    "<cmd>",                    "print help information"},
        {Exit,  "q$uit",    "",                         "terminate"},
        {Exit,  "exit",     "",                         "terminate"},
        {NULL,  "",         "",                      ""}
};

/*-------------- Command proccessor --------------------------------------------*/

static int h_level = 0, menu_en = ON;

char *cmddsp(char *cmd)
{
    static char tmp[20];
    char *t, *s;

    t = tmp;
    s = ATR_BOARD;
    while (*s)
        *t++ = *s++;
    s = cmd;
    while (*s && (*s != '$'))
        *t++ = *s++;
    if (*s)
        s++;
    *t = '\0';
    strcat(t, ATR_OFF);
    strcat(t, s);

    return tmp;
}

void help(int argc, char *argv[])
{
    int i;
    char *hcmd = "";

    for (i = 1; i < argc; i++) {
        if (isdigit(*argv[i])) {
            h_level = atoi(argv[i]);
            printf("  help level set to %d.\n", h_level);
            return;
        } else {
            hcmd = argv[i];
        }
    }

    i = 0;
    while ((AlmostSame(hcmd, CmdTab[i].cmdstr) != TRUE)
            && (CmdTab[i].func != NULL))
        i++;

    if (CmdTab[i].func == NULL) {       /* command not found            */
        i = 0;
        while (CmdTab[i].func != NULL) {
            printf("  %-18s %s %s\n", cmddsp(CmdTab[i].cmdstr)
                    , CmdTab[i].menustr, CmdTab[i].helpstr);
            i++;
        }
    } else {                    /* print command help           */
        printf("  %-18s %s %s\n", cmddsp(CmdTab[i].cmdstr)
                , CmdTab[i].menustr, CmdTab[i].helpstr);
    }
}

int PrintMenu(int h_level)
{
    int i;

    switch (h_level) {
    case 0:
        i = 0;
        while (CmdTab[i].func != NULL) {
            printf("  %-18s %s\n", cmddsp(CmdTab[i].cmdstr), CmdTab[i].menustr);
            i++;
        }
    case 1:
        //        PrintStatus();
        break;
    default:
        break;
    }
    return 0;
}

void IssueCmd(char *cmd)
{
    char *argv[MAXARG], tmp[130];
    int i, argc;
    int rv __attribute__ ((unused)) = 0;

    strcpy(tmp, cmd);

    argc = GetArgs(tmp, argv);

    if (argc == 0) {            /* print menu           */
        if (menu_en) {
            PrintMenu(h_level);
            menu_en = OFF;
        }
        printf("\n");
    } else {                    /* search command table         */
        menu_en = ON;
        if (!isalnum(*argv[0])) {
            PrintMenu(0);
            menu_en = OFF;
        } else {
            i = 0;
            while ((AlmostSame(argv[0], CmdTab[i].cmdstr) != TRUE)
                    && (CmdTab[i].func != NULL))
                i++;

            if (CmdTab[i].func == NULL) {       /* command not found            */
                rv = system(cmd);
            } else {            /* exec command function        */
                (CmdTab[i].func) (argc, argv);
            }
        }
    }
}

void help_message()
{
    printf("=== rv32emc simulator ===\n");
    printf(
    "Usage: rvsim {opts} {elf32 binary}\n"
    "Options:\n"
    "  -i SCR-FILE  Execute command script\n"
    "  -r           Run all and exit\n"
    "  -t           Trace all and exit\n"
    );

}

int main (int argc, char **argv)
{
    int i;
    char cmd[256], c, *s, *scr = NULL, *lfn = NULL, *prompt = "rvsim> ";
    char *argvall[2] = {"","-all"};
    FILE *ifp = NULL;

    bfd_init ();
    ofp = stdout;
    stack = (bfd_byte*)malloc(stacksize+0x100);
    s = NULL;

    for (i = 1; i < argc; i++) {
        c = *argv[i];
        if (!strcmp(argv[i], "-i")) {
            if (++i >= argc)
                break;
            scr = argv[i];
        } else if(!strcmp(argv[i], "-stk")){
            if(++i >= argc) break;
            stack_top = strtol(argv[i], NULL, 16);
        } else if (!strcmp(argv[i], "-h")||!strcmp(argv[i], "--help")) {
            help_message();
            return 0;
        } else if (!strcmp(argv[i], "-debug")) {
            debug = 1;
        } else if (!strcmp(argv[i], "-r")) {
            s = "r";
        } else if (!strcmp(argv[i], "-t")) {
            s = "t";
        } else if (isalnum(c) || c == '.' || c == '/') {
            lfn = argv[i];
        }
    }
    if(s){
        if(!lfn){
            load_abs(lfn, 0);
            if(*s == 'r') Run(2, argvall);
            if(*s == 't') Trace(2, argvall);
            return 0;
        }
    }
    if (scr != NULL) {
        if ((ifp = fopen(scr, "r")) == NULL) {
            printf("*** Can't open script file '%s'.\n", scr);
            exit(0);
        }
    }

 //   bfd_init ();
 //   ofp = stdout;

    printf("======= rvsim ==============================================\n"
            "   rv32 processor simulator.\n");

    if(lfn) load_abs(lfn, 1);
//    stack = (bfd_byte*)malloc(stacksize+0x100);

    Nregs = arch == 'e' ? 16 : 32;
    prompt = arch == 'e' ? "rvsim-E> " : "rvsim-I> ";

    if (ifp != NULL) {          /* exec script      */
        while (fgets(cmd, 130, ifp) != NULL) {
            RejectComment(cmd);
            if (!IsComment(cmd) && !IsBlank(cmd)) {
                //                add_history(cmd);
                IssueCmd(cmd);
            }
        }
    }

    for (;;) {
        if ((s = readline(prompt))) {
            if (!IsBlank(s)) {
                add_history(s);
            }
            strcpy(cmd, s);
            IssueCmd(cmd);
        } else
            break;
    }

    //   display_file (argv[1], NULL);
    return 0;
}

