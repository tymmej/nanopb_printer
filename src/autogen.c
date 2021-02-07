#include "autogen.h"
#include <inttypes.h>
#include <stdio.h>

static const char *
get_enum_text(const enum_desc_t *desc, pb_size_t idx)
{
    int i = 0;
    while (desc[i].idx != (pb_size_t)-1) {
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
    printf("%s: \"", name);
    for (size_t i = 0; i < len; i++) {
        printf("%.02x", buf[i]);
    }
    printf("\"\n");
}

void
print_message(const void *message, const proto_desc_t *desc, int indent)
{
    for (int idx = 0; desc[idx].field != FIELD_LAST; idx++) {
        pb_size_t count = 1;
        if (desc[idx].label == LABEL_OPTIONAL) {
            if (!(*(bool *)((uint8_t *)message + desc[idx].offset_optional))) {
                count = 0;
            }
        }
        if (desc[idx].label == LABEL_REPEATED) {
            count = *(pb_size_t *)((uint8_t *)message + desc[idx].offset_repeated);
        }
        if (desc[idx].label == LABEL_ONEOF) {
            count = 0;
            if (*(pb_size_t *)((uint8_t *)message + desc[idx].oneof.offset_tag) == desc[idx].oneof.tag) {
                count = 1;
            }
        }
        for (pb_size_t i = 0; i < count; i++) {
            for (int j = 0; j < indent; j++) {
                printf("\t");
            }
            void *field_ptr = (uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size);
            if (desc[idx].field == FIELD_NORMAL) {
                char format[8] = { 0 };
                format[0] = '%';
                strcpy(&format[1], desc[idx].format);
                printf("%s: ", desc[idx].name);
                if (strcmp(&format[1], "s") == 0) {
                    printf("\"");
                    printf(format, field_ptr);
                    printf("\"");
                } else if (strcmp(&format[1], "f") == 0) {
                    printf(format, *(float *)field_ptr);
                } else if (strcmp(&format[1], "lf") == 0) {
                    strcpy(&format[1], &desc[idx].format[1]);
                    printf(format, *(double *)field_ptr);
                } else if (strcmp(&format[1], PRId64) == 0) {
                    printf(format, *(int64_t *)field_ptr);
                } else if (strcmp(&format[1], PRIu64) == 0) {
                    printf(format, *(uint64_t *)field_ptr);
                } else if (strcmp(&format[1], PRId32) == 0) {
                    printf(format, *(int32_t *)field_ptr);
                } else if (strcmp(&format[1], PRIu32) == 0) {
                    printf(format, *(uint32_t *)field_ptr);
                } else if (strcmp(&format[1], "b") == 0) {
                    strcpy(&format[1], PRIu8);
                    printf(format, *(uint8_t *)field_ptr);
                } else if (strcmp(&format[1], PRIu32) == 0) {
                    printf(format, *(uint32_t *)field_ptr);
                }
                printf("\n");
            }
            if (desc[idx].field == FIELD_ENUM) {
                printf("%s: %s", desc[idx].name, get_enum_text(desc[idx].enum_type.desc, *(pb_size_t *)field_ptr));
                printf("\n");
            }
            if (desc[idx].field == FIELD_BYTES) {
                print_bytes(desc[idx].name, (uint8_t *)field_ptr + sizeof(pb_size_t), *(pb_size_t *)field_ptr);
                printf("\n");
            }
            if (desc[idx].field == FIELD_MESSAGE) {
                printf("%s {\n", desc[idx].name);
                print_message(field_ptr, desc[idx].message.desc, indent + 1);
                for (int j = 0; j < indent; j++) {
                    printf("\t");
                }
                printf("}\n");
            }
        }             
    }
}

static const enum_desc_t SimpleEnum1_desc[] = {
	{.idx = 1, .text = "ONE"},
	{.idx = 2, .text = "TWO"},
	{.idx = (pb_size_t)-1}
};

static const enum_desc_t SimpleEnum2_desc[] = {
	{.idx = 1, .text = "ONEONE"},
	{.idx = 2, .text = "TWOTWO"},
	{.idx = (pb_size_t)-1}
};

const proto_desc_t SimpleMessage1_desc[] = {
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "lucky_number", .offset = offsetof(SimpleMessage1, lucky_number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32},
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "unlucky_number", .offset = offsetof(SimpleMessage1, unlucky_number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32},
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum1", .offset = offsetof(SimpleMessage1, enum1), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum1_desc},
    {.field = FIELD_LAST},
};

const proto_desc_t SimpleMessage2_desc[] = {
	{.field = FIELD_NORMAL, .label = LABEL_OPTIONAL, .name = "unlucky_number", .offset = offsetof(SimpleMessage2, unlucky_number), .offset_optional = offsetof(SimpleMessage2, has_unlucky_number), .offset_repeated = -1, .element_size = -1, .format = PRId64},
	{.field = FIELD_NORMAL, .label = LABEL_OPTIONAL, .name = "lucky_number", .offset = offsetof(SimpleMessage2, lucky_number), .offset_optional = offsetof(SimpleMessage2, has_lucky_number), .offset_repeated = -1, .element_size = -1, .format = PRId64},
	{.field = FIELD_ENUM, .label = LABEL_OPTIONAL, .name = "enum1", .offset = offsetof(SimpleMessage2, enum1), .offset_optional = offsetof(SimpleMessage2, has_enum1), .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum2_desc},
	{.field = FIELD_BYTES, .label = LABEL_OPTIONAL, .name = "byte", .offset = offsetof(SimpleMessage2, byte), .offset_optional = offsetof(SimpleMessage2, has_byte), .offset_repeated = -1, .element_size = -1},
    {.field = FIELD_LAST},
};

const proto_desc_t SimpleNested_desc[] = {
		{.field = FIELD_MESSAGE, .label = LABEL_OPTIONAL, .name = "nested1", .offset = offsetof(SimpleNested, nested1), .offset_optional = offsetof(SimpleNested, has_nested1), .offset_repeated = -1, .element_size = -1, .message.desc = SimpleMessage1_desc},
		{.field = FIELD_MESSAGE, .label = LABEL_REQUIRED, .name = "nested2", .offset = offsetof(SimpleNested, nested2), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .message.desc = SimpleMessage2_desc},
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum1", .offset = offsetof(SimpleNested, enum1), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum1_desc},
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum2", .offset = offsetof(SimpleNested, enum2), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum2_desc},
    {.field = FIELD_LAST},
};

const proto_desc_t SimpleRepeated_desc[] = {
		{.field = FIELD_NORMAL, .label = LABEL_REPEATED, .name = "text", .offset = offsetof(SimpleRepeated, text), .offset_optional = -1, .offset_repeated = offsetof(SimpleRepeated, text_count), .element_size = sizeof(((SimpleRepeated *)NULL)->text[0]), .format = "s"},
		{.field = FIELD_MESSAGE, .label = LABEL_REPEATED, .name = "message", .offset = offsetof(SimpleRepeated, message), .offset_optional = -1, .offset_repeated = offsetof(SimpleRepeated, message_count), .element_size = sizeof(((SimpleRepeated *)NULL)->message[0]), .message.desc = SimpleMessage1_desc},
    {.field = FIELD_LAST},
};

const proto_desc_t SimpleOneof_desc[] = {
		{.field = FIELD_MESSAGE, .label = LABEL_ONEOF, .name = "msg1", .offset = offsetof(SimpleOneof, oneofmsg.msg1), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .message.desc = SimpleMessage1_desc, .oneof.offset_tag = offsetof(SimpleOneof, which_oneofmsg), .oneof.tag = SimpleOneof_msg1_tag},
		{.field = FIELD_NORMAL, .label = LABEL_ONEOF, .name = "unlucky_number", .offset = offsetof(SimpleOneof, oneofmsg.unlucky_number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32, .oneof.offset_tag = offsetof(SimpleOneof, which_oneofmsg), .oneof.tag = SimpleOneof_unlucky_number_tag},
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "notnested", .offset = offsetof(SimpleOneof, notnested), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId64},
    {.field = FIELD_LAST},
};

