#include "nanopb_printer.h"

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