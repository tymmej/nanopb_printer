#include "common.h"

#include "nanopb_printer.h"
#include "simple.pb.h"
#include "simple.h"

#include <pb_encode.h>

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>

static int
example_printer(const char *str, void *user_data)
{
    size_t len = strlen(str);

    if (*(void **)user_data == NULL) {
        *(void **)user_data = malloc(len + 1);
        if (*(void **)user_data == NULL) {
            perror("Unable to alloc memory");
            exit(1);
        }
    } else {
        char *new_ptr = realloc(*(void **)user_data, strlen(*(void **)user_data) + len + 1);
        if (new_ptr == NULL) {
            perror("Unable to alloc memory");
            exit(1);
        }
        *(void **)user_data = new_ptr;
    }

    strcat(*(void **)user_data, str);

    return len;
}

static void
test_single_number(void **state)
{
    (void)state;

    const char *expected = "number: 99\n";
    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_single_number();

    assert_string_equal(output, expected);
    
    free(output);
}

static void
test_simple_1(void **state)
{
    (void)state;

    const char *expected = "lucky_number: 42\n"
                           "unlucky_number: 0\n"
                           "enum1: TWO\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_1();

    assert_string_equal(output, expected);
    
    free(output);
}

static void
test_simple_2(void **state)
{
    (void)state;

    const char *expected = "lucky_number: 23\n"
                           "enum1: TWOTWO\n"
                           "byte: \"a55a\"\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_2();

    assert_string_equal(output, expected);
    
    free(output);
}

static void
test_simple_nested(void **state)
{
    (void)state;

    const char *expected = "nested1 {\n"
                           "\tlucky_number: 0\n"
                           "\tunlucky_number: 123\n"
                           "\tenum1: ONE\n"
                           "}\n"
                           "nested2 {\n"
                           "}\n"
                           "enum1: ONE\n"
                           "enum2: TWOTWO\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_nested();

    assert_string_equal(output, expected);
    
    free(output);
}

static void
test_simple_repeated(void **state)
{
    (void)state;

    const char *expected = "text: \"asdf\"\n"
                           "text: \"zxcv\"\n"
                           "message {\n"
                           "\tlucky_number: 0\n"
                           "\tunlucky_number: 169\n"
                           "\tenum1: ONE\n"
                           "}\n"
                           "message {\n"
                           "\tlucky_number: 0\n"
                           "\tunlucky_number: 144\n"
                           "\tenum1: ONE\n"
                           "}\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_repeated();

    assert_string_equal(output, expected);
    
    free(output);
}

static void
test_simple_oneof_1(void **state)
{
    (void)state;

    const char *expected = "unlucky_number: 123\n"
                           "notnested: 124\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_oneof_1();

    assert_string_equal(output, expected);
    
    free(output);
}

static void
test_simple_oneof_2(void **state)
{
    (void)state;

    const char *expected = "msg1 {\n"
                           "\tlucky_number: 42\n"
                           "\tunlucky_number: 24\n"
                           "\tenum1: TWO\n"
                           "}\n"
                           "notnested: 142\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_oneof_2();

    assert_string_equal(output, expected);
    
    free(output);    
}

static void
test_simple_including(void **state)
{
    (void)state;

    const char *expected = "included {\n"
                           "\tenum_inc: EXCLUDED\n"
                           "}\n";

    char *output = NULL;

    nanopb_printer_register_printer(example_printer, "\t", &output);

    example_simple_including();

    assert_string_equal(output, expected);
    
    free(output);    
}

int
main(int argc, char *argv[])
{

    (void)argc;
    (void)argv;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_single_number),
        cmocka_unit_test(test_simple_1),
        cmocka_unit_test(test_simple_2),
        cmocka_unit_test(test_simple_nested),
        cmocka_unit_test(test_simple_repeated),
        cmocka_unit_test(test_simple_oneof_1),
        cmocka_unit_test(test_simple_oneof_2),
        cmocka_unit_test(test_simple_including),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
