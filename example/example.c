#include "common.h"

#include "nanopb_printer.h"
#include "proto/simple.pb.h"
#include "proto/nanopb_printer_simple.h"

#include <pb_encode.h>

#include <stdio.h>
#include <stdlib.h>

typedef void (*example_func_t)(void);

typedef struct {
    const char *name;
    example_func_t func;
} functions_t;

static int
example_printer(const char *str, void *user_data)
{

    fputs(str, (FILE *)user_data);

    return printf("%s", str);
}

int
main(int argc, char *argv[])
{

    (void)argc;
    (void)argv;

    const functions_t example_funcs[] = {
#define ENTRY_GENERATOR(name) {#name, name}
        ENTRY_GENERATOR(example_single_number),
        ENTRY_GENERATOR(example_simple_1),
        ENTRY_GENERATOR(example_simple_2),
        ENTRY_GENERATOR(example_simple_nested),
        ENTRY_GENERATOR(example_simple_repeated),
        ENTRY_GENERATOR(example_simple_oneof_1),
        ENTRY_GENERATOR(example_simple_oneof_2),
        ENTRY_GENERATOR(example_simple_including),
        ENTRY_GENERATOR(NULL),
#undef ENTRY_GENERATOR
    };

    FILE *f = fopen("simple.txt", "w");
    if (f == NULL)
    {
        perror("Error opening file!\n");
        exit(1);
    }

    nanopb_printer_register_printer(example_printer, "\t", f);

    for (size_t i = 0; example_funcs[i].func; i++) {
        printf("Running %s:\n", example_funcs[i].name);
        (*example_funcs[i].func)();
    }

    fclose(f);

    return 0;
}
