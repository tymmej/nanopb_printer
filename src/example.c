#include <stdio.h>

#include "simple.pb.h"
#include "autogen.h"

int
main(int argc, char *argv[])
{

    (void)argc;
    (void)argv;

    SimpleMessage1 simple1 = SimpleMessage1_init_default;
    SimpleMessage2 simple2 = SimpleMessage2_init_default;
    SimpleNested simplenested = SimpleNested_init_default;
    SimpleRepeated simplerepeated = SimpleRepeated_init_default;
    SimpleOneof simpleoneof = SimpleOneof_init_default;
    simple1.enum1 = SimpleEnum1_TWO;

    SimpleMessage1_print(&simple1, 0);
    SimpleMessage2_print(&simple2, 0);
    SimpleNested_print(&simplenested, 0);
    SimpleRepeated_print(&simplerepeated, 0);
    SimpleOneof_print(&simpleoneof, 0);

    simple1.lucky_number = 42;
    simple2.has_lucky_number = true;
    simple2.lucky_number = 23;
    simple2.has_enum1 = true;
    simple2.enum1 = SimpleEnum2_TWOTWO;
    simplenested.enum1 = SimpleEnum1_ONE;
    simplenested.enum2 = SimpleEnum2_TWOTWO;
    simplerepeated.text_count = 2;
    simplerepeated.message_count = 2;
    strcpy(simplerepeated.text[0], "asdf");
    strcpy(simplerepeated.text[1], "zxcv");
    simplerepeated.message[0].unlucky_number = 169;
    simplerepeated.message[1].unlucky_number = 144;
    simpleoneof.which_oneofmsg = SimpleOneof_unlucky_number_tag;
    simpleoneof.oneofmsg.unlucky_number = 123;
    simpleoneof.notnested = 124;

    SimpleMessage1_print(&simple1, 0);
    SimpleMessage2_print(&simple2, 0);
    SimpleNested_print(&simplenested, 0);
    SimpleRepeated_print(&simplerepeated, 0);
    SimpleOneof_print(&simpleoneof, 0);

    return 0;
}
