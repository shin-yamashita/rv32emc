#include <assert.h>
#include <bfd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <stdint.h>

static bfd *abfd;
static asymbol **symbols;
static int nsymbols;

void show_debug_info (void *address)
{
    asection *section = bfd_get_section_by_name(abfd, ".debug_info");
    assert(section != NULL);

    const char *file_name;
    const char *function_name;
    unsigned int lineno;
    int found = bfd_find_nearest_line(abfd, section, symbols,
                                      (long)address,
                                      &file_name,
                                      &function_name,
                                      &lineno);
    if (found && file_name != NULL && function_name != NULL) {
        char tmp[strlen(file_name)];
        strcpy(tmp, file_name);
//        printf("                           %s:%s:%d\n", basename(tmp), function_name, lineno);
        printf("                           %s:%d\n", file_name, lineno);
    }
}

int main (int argc, char *argv[])
{
    abfd = bfd_openr (argv[1], NULL);

    fprintf (stdout, "Filename = %s\n", abfd->filename);

    bfd_check_format (abfd, bfd_object);

    long storage_needed;
    asymbol **symbol_table;
    long number_of_symbols;
    long i;

    storage_needed = bfd_get_symtab_upper_bound (abfd);
    if (storage_needed < 0) {
        fprintf (stderr, "Error storage_needed < 0\n");
        return 0;
    }
    if (storage_needed == 0) {
        fprintf (stderr, "Error storage_needed == 0\n");
        return 0;
    }

    symbol_table = (asymbol **) malloc (storage_needed);

    number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);

    fprintf (stdout, "number_of_symbols = %ld\n", number_of_symbols);

    if (number_of_symbols < 0) {
        fprintf (stderr, "Error: number_of_symbols < 0\n");
        return 0;
    }
    for (i = 0; i < number_of_symbols; i++) {
//        asection *section = symbol_table[i]->section;

//        fprintf (stdout, "%8s %08lx ", bfd_asymbol_name (symbol_table[i]), bfd_asymbol_value (symbol_table[i]));
        fprintf (stdout, " %08lx ", bfd_asymbol_value (symbol_table[i]));

        if ((symbol_table[i]->flags & BSF_FUNCTION) != 0x00) {
            fprintf (stdout, "Func  ");
        } else if ((symbol_table[i]->flags & BSF_LOCAL) != 0x00) {
            fprintf (stdout, "Local ");
        } else if ((symbol_table[i]->flags & BSF_GLOBAL) != 0x00) {
            fprintf (stdout, "global");
        } else {
            fprintf (stdout, "others");
        }

        fprintf (stdout, " : %s\n", bfd_asymbol_name (symbol_table[i]));
        if(symbol_table[i]->flags & BSF_FUNCTION)	// || symbol_table[i]->flags & BSF_GLOBAL)
          show_debug_info(bfd_asymbol_value (symbol_table[i]));
    }

    return 0;
}


