#pragma once

#include <pb.h>
#include <inttypes.h>

typedef enum {
    FIELD_NORMAL,
    FIELD_ENUM,
    FIELD_BYTES,
    FIELD_MESSAGE,
    FIELD_LAST,
} nanopb_printer_field_type_t;

typedef enum {
    LABEL_REQUIRED,
    LABEL_REPEATED,
    LABEL_OPTIONAL,
    LABEL_ONEOF,
} nanopb_printer_field_label_t;

typedef struct {
    pb_size_t idx;
    const char *text;
} nanopb_printer_enum_desc_t;

typedef struct {
    const nanopb_printer_enum_desc_t *desc;
} nanopb_printer_proto_enum_desc_t;

typedef struct {
    const void *desc;
} nanopb_printer_proto_message_desc_t;

typedef struct {
    size_t offset_tag;
    int tag;
} nanopb_printer_proto_oneof_desc_t;

typedef struct {
    nanopb_printer_field_type_t field;
    nanopb_printer_field_label_t label;
    const char *name;
    size_t offset;
    size_t offset_optional;
    size_t offset_repeated;
    size_t offset_oneof;
    size_t element_size;
    const char *format;
    nanopb_printer_proto_message_desc_t message;
    union {
        nanopb_printer_proto_enum_desc_t enum_type;
        nanopb_printer_proto_oneof_desc_t oneof;
    };
} nanopb_printer_proto_desc_t;

typedef int (*nanopb_printer_callback_t)(const char *str, void *user_data);

void nanopb_printer_print_message(const void *message, const nanopb_printer_proto_desc_t *desc, int indent);

void nanopb_printer_register_printer(nanopb_printer_callback_t cb, const char *indentation, void *user_data);

