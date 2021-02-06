#include "autogen.h"
#include <inttypes.h>
#include <stdio.h>

static char *
get_enum_text(enum_desc_t *desc, int idx)
{
    int i = 0;
    while (desc[i].idx != -1) {
        if (desc[i].idx == idx) {
            return desc[i].text;
        }
        i++;
    }
    return "Unknown enum";
}

static void
print_bytes(const char *name, const uint8_t *buf, size_t len)
{
    printf("%s=\"", name);
    for (size_t i = 0; i < len; i++) {
        printf("%.02x", buf[i]);
    }
    printf("\"\n");
}

static enum_desc_t SimpleEnum1_desc[] = {
	{.idx = 1, .text = "ONE"},
	{.idx = 2, .text = "TWO"},
	{.idx = -1}
};

static enum_desc_t SimpleEnum2_desc[] = {
	{.idx = 1, .text = "ONEONE"},
	{.idx = 2, .text = "TWOTWO"},
	{.idx = -1}
};

void
SimpleMessage1_print(SimpleMessage1 *msg, uint8_t level)
{
	for (int j = 0; j < level; j++) {
	    printf("\t");
	}
	printf("SimpleMessage1: \n");
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("lucky_number=%"PRId32"\n", msg->lucky_number);
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("unlucky_number=%"PRId32"\n", msg->unlucky_number);
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("enum1=%s\n", get_enum_text(SimpleEnum1_desc, msg->enum1));
}

void
SimpleMessage2_print(SimpleMessage2 *msg, uint8_t level)
{
	for (int j = 0; j < level; j++) {
	    printf("\t");
	}
	printf("SimpleMessage2: \n");
	if (msg->has_unlucky_number) {
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("unlucky_number=%"PRId64"\n", msg->unlucky_number);
	}
	if (msg->has_lucky_number) {
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("lucky_number=%"PRId64"\n", msg->lucky_number);
	}
	if (msg->has_enum1) {
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("enum1=%s\n", get_enum_text(SimpleEnum2_desc, msg->enum1));
	}
	if (msg->has_byte) {
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	print_bytes("byte", msg->byte.bytes, msg->byte.size);
	}
}

void
SimpleNested_print(SimpleNested *msg, uint8_t level)
{
	for (int j = 0; j < level; j++) {
	    printf("\t");
	}
	printf("SimpleNested: \n");
	SimpleMessage1_print(&msg->nested1, level + 1);
	SimpleMessage2_print(&msg->nested2, level + 1);
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("enum1=%s\n", get_enum_text(SimpleEnum1_desc, msg->enum1));
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("enum2=%s\n", get_enum_text(SimpleEnum2_desc, msg->enum2));
}

void
SimpleRepeated_print(SimpleRepeated *msg, uint8_t level)
{
	for (int j = 0; j < level; j++) {
	    printf("\t");
	}
	printf("SimpleRepeated: \n");
	for (pb_size_t i = 0; i < msg->text_count; i++) {
		for (int j = 0; j < level + 1; j++) {
		    printf("\t");
		}
		printf("text=%""s""\n", msg->text[i]);
	}
	for (pb_size_t i = 0; i < msg->message_count; i++) {
		SimpleMessage1_print(&msg->message[i], level + 1);
	}
}

void
SimpleOneof_print(SimpleOneof *msg, uint8_t level)
{
	for (int j = 0; j < level; j++) {
	    printf("\t");
	}
	printf("SimpleOneof: \n");
	if (msg->which_oneofmsg == SimpleOneof_msg1_tag) {
		for (int j = 0; j < level + 1; j++) {
		    printf("\t");
		}
		SimpleMessage1_print(&msg->oneofmsg.msg1, level + 1);
	}
	if (msg->which_oneofmsg == SimpleOneof_unlucky_number_tag) {
		for (int j = 0; j < level + 1; j++) {
		    printf("\t");
		}
		printf("unlucky_number=%"PRId32"\n", msg->oneofmsg.unlucky_number);
	}
	for (int j = 0; j < level + 1; j++) {
	    printf("\t");
	}
	printf("notnested=%"PRId64"\n", msg->notnested);
}

