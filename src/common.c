#include "common.h"

#include "nanopb_printer.h"
#include "simple.pb.h"
#include "simple.h"

#include <pb_encode.h>

#include <stdio.h>
#include <stdlib.h>

static void
print_bytes(const char *name, const uint8_t *buf, size_t len)
{
    printf("%s=\"", name);
    for (size_t i = 0; i < len; i++) {
        printf("%.02x", buf[i]);
    }
    printf("\"\n");
}

void
example_single_number(void)
{
    SingleNumber singlenumber = SingleNumber_init_default;

    singlenumber.number = 99;

    nanopb_printer_print_message(&singlenumber, SingleNumber_desc, 0);
    printf("\n");

    uint8_t buffer[SingleNumber_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SingleNumber_fields, &singlenumber);
    print_bytes("singlenumber", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_1(void)
{
    SimpleMessage1 simple1 = SimpleMessage1_init_default;

    simple1.enum1 = SimpleEnum1_TWO;
    simple1.lucky_number = 42;

    nanopb_printer_print_message(&simple1, SimpleMessage1_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleMessage1_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleMessage1_fields, &simple1);
    print_bytes("simple1", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_2(void)
{
    SimpleMessage2 simple2 = SimpleMessage2_init_default;

    simple2.has_lucky_number = true;
    simple2.lucky_number = 23;
    simple2.has_enum1 = true;
    simple2.enum1 = SimpleEnum2_TWOTWO;
    simple2.has_byte = true;
    simple2.byte.size = 2;
    simple2.byte.bytes[0] = 0xa5;
    simple2.byte.bytes[1] = 0x5a;

    nanopb_printer_print_message(&simple2, SimpleMessage2_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleMessage2_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleMessage2_fields, &simple2);
    print_bytes("simple2", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_nested(void)
{
    SimpleNested simplenested = SimpleNested_init_default;

    simplenested.enum1 = SimpleEnum1_ONE;
    simplenested.enum2 = SimpleEnum2_TWOTWO;
    simplenested.has_nested1 = true;
    simplenested.nested1.unlucky_number = 123;

    nanopb_printer_print_message(&simplenested, SimpleNested_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleNested_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleNested_fields, &simplenested);
    print_bytes("simplenested", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_repeated(void)
{
    SimpleRepeated simplerepeated = SimpleRepeated_init_default;

    simplerepeated.text_count = 2;
    simplerepeated.message_count = 2;
    strcpy(simplerepeated.text[0], "asdf");
    strcpy(simplerepeated.text[1], "zxcv");
    simplerepeated.message[0].unlucky_number = 169;
    simplerepeated.message[1].unlucky_number = 144;

    nanopb_printer_print_message(&simplerepeated, SimpleRepeated_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleRepeated_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleRepeated_fields, &simplerepeated);
    print_bytes("simplerepeated", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_oneof_1(void)
{
    SimpleOneof simpleoneof = SimpleOneof_init_default;

    simpleoneof.which_oneofmsg = SimpleOneof_unlucky_number_tag;
    simpleoneof.oneofmsg.unlucky_number = 123;
    simpleoneof.notnested = 124;

    nanopb_printer_print_message(&simpleoneof, SimpleOneof_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleOneof_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleOneof_fields, &simpleoneof);
    print_bytes("simpleoneof", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_oneof_2(void)
{
    SimpleOneof simpleoneof = SimpleOneof_init_default;

    simpleoneof.which_oneofmsg = SimpleOneof_msg1_tag;
    simpleoneof.oneofmsg.msg1.enum1 = SimpleEnum1_TWO;
    simpleoneof.oneofmsg.msg1.lucky_number = 42;
    simpleoneof.oneofmsg.msg1.unlucky_number = 24;
    simpleoneof.notnested = 142;

    nanopb_printer_print_message(&simpleoneof, SimpleOneof_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleOneof_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleOneof_fields, &simpleoneof);
    print_bytes("simpleoneof2", buffer, ostream.bytes_written);
    printf("\n");
}

void
example_simple_including(void)
{
    SimpleIncluding simpleincluding = SimpleIncluding_init_default;

    simpleincluding.included.enum_inc = SimpleIncludedEnum_EXCLUDED;

    nanopb_printer_print_message(&simpleincluding, SimpleIncluding_desc, 0);
    printf("\n");

    uint8_t buffer[SimpleIncluding_size];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleIncluding_fields, &simpleincluding);
    print_bytes("simpleincluded", buffer, ostream.bytes_written);
    printf("\n");
}
