#include "nanopb_printer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static nanopb_printer_callback_t nanopb_printer_callback;
static void *nanopb_printer_callback_user_data;
static const char *nanopb_printer_indentation;

static int
nanopb_printer_print_internal(const char *format, ...)
{
    va_list args_1;
    va_list args_2;

    va_start(args_1, format);
    va_copy(args_2, args_1);

    size_t buf_size = vsnprintf(NULL, 0, format, args_1) + 1;
    va_end(args_1);

    char *buffer = malloc(buf_size);

    if (buffer == NULL) {
        perror("Unable to alloc memory");
        exit(1);
    }
    vsnprintf(buffer, buf_size, format, args_2);
    va_end(args_2);

    nanopb_printer_callback(buffer, nanopb_printer_callback_user_data);

    free(buffer);

    return buf_size - 1;
}

static const char *
nanopb_printer_get_enum_text(const nanopb_printer_enum_desc_t *desc, pb_size_t idx)
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
nanopb_printer_print_bytes(const char *name, const uint8_t *buf, size_t len)
{

    nanopb_printer_print_internal("%s: \"", name);
    for (size_t i = 0; i < len; i++) {
        nanopb_printer_print_internal("%.02x", buf[i]);
    }
    nanopb_printer_print_internal("\"\n");
}

void
nanopb_printer_register_printer(nanopb_printer_callback_t cb, const char *indentation, void *user_data)
{

    nanopb_printer_callback = cb;
    nanopb_printer_callback_user_data = user_data;
    nanopb_printer_indentation = indentation;
}

void
nanopb_printer_print_message(const void *message, const nanopb_printer_proto_desc_t *desc, int indent)
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
                nanopb_printer_print_internal(nanopb_printer_indentation);
            }
            void *field_ptr = (uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size);
            if (desc[idx].field == FIELD_NORMAL) {
                char format[8] = { 0 };
                format[0] = '%';
                strcpy(&format[1], desc[idx].format);
                nanopb_printer_print_internal("%s: ", desc[idx].name);
                if (strcmp(&format[1], "s") == 0) {
                    nanopb_printer_print_internal("\"");
                    nanopb_printer_print_internal(format, field_ptr);
                    nanopb_printer_print_internal("\"");
                } else if (strcmp(&format[1], "f") == 0) {
                    nanopb_printer_print_internal(format, *(float *)field_ptr);
                } else if (strcmp(&format[1], "lf") == 0) {
                    strcpy(&format[1], &desc[idx].format[1]);
                    nanopb_printer_print_internal(format, *(double *)field_ptr);
                } else if (strcmp(&format[1], PRId64) == 0) {
                    nanopb_printer_print_internal(format, *(int64_t *)field_ptr);
                } else if (strcmp(&format[1], PRIu64) == 0) {
                    nanopb_printer_print_internal(format, *(uint64_t *)field_ptr);
                } else if (strcmp(&format[1], PRId32) == 0) {
                    nanopb_printer_print_internal(format, *(int32_t *)field_ptr);
                } else if (strcmp(&format[1], PRIu32) == 0) {
                    nanopb_printer_print_internal(format, *(uint32_t *)field_ptr);
                } else if (strcmp(&format[1], "b") == 0) {
                    strcpy(&format[1], PRIu8);
                    nanopb_printer_print_internal(format, *(uint8_t *)field_ptr);
                } else if (strcmp(&format[1], PRIu32) == 0) {
                    nanopb_printer_print_internal(format, *(uint32_t *)field_ptr);
                }
                nanopb_printer_print_internal("\n");
            }
            if (desc[idx].field == FIELD_ENUM) {
                nanopb_printer_print_internal("%s: %s", desc[idx].name, nanopb_printer_get_enum_text(desc[idx].enum_type.desc, *(pb_size_t *)field_ptr));
                nanopb_printer_print_internal("\n");
            }
            if (desc[idx].field == FIELD_BYTES) {
                nanopb_printer_print_bytes(desc[idx].name, (uint8_t *)field_ptr + sizeof(pb_size_t), *(pb_size_t *)field_ptr);
                nanopb_printer_print_internal("\n");
            }
            if (desc[idx].field == FIELD_MESSAGE) {
                nanopb_printer_print_internal("%s {\n", desc[idx].name);
                nanopb_printer_print_message(field_ptr, desc[idx].message.desc, indent + 1);
                for (int j = 0; j < indent; j++) {
                    nanopb_printer_print_internal(nanopb_printer_indentation);
                }
                nanopb_printer_print_internal("}\n");
            }
        }             
    }
}