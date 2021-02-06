#include <stdio.h>

#include "simple.pb.h"
#include "autogen.h"

#include <pb_encode.h>

static void
print_bytes(const char *name, const uint8_t *buf, size_t len)
{
    printf("%s=\"", name);
    for (size_t i = 0; i < len; i++) {
        printf("%.02x", buf[i]);
    }
    printf("\"\n");
}

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
    simple2.has_byte = true;
    simple2.byte.size = 2;
    simple2.byte.bytes[0] = 0xa5;
    simple2.byte.bytes[1] = 0x5a;
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


    printf("\n");
    uint8_t buffer[128];
    pb_ostream_t ostream;

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleMessage1_fields, &simple1);
    print_bytes("simple1", buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleMessage2_fields, &simple2);
    print_bytes("simple2", buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleNested_fields, &simplenested);
    print_bytes("simplenested", buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleRepeated_fields, &simplerepeated);
    print_bytes("simplerepeated", buffer, ostream.bytes_written);

    ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    pb_encode(&ostream, SimpleOneof_fields, &simpleoneof);
    print_bytes("simpleoneof", buffer, ostream.bytes_written);

    return 0;
}
