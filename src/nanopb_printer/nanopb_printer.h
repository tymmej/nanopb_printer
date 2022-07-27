#pragma once

#include <pb.h>
#include <inttypes.h>

typedef enum {
    FIELD_NORMAL,
    FIELD_ENUM,
    FIELD_BYTES,
    FIELD_MESSAGE,
    FIELD_LAST,
} field_type_t;

typedef enum {
    LABEL_REQUIRED,
    LABEL_REPEATED,
    LABEL_OPTIONAL,
    LABEL_ONEOF,
} field_label_t;

typedef struct {
    pb_size_t idx;
    const char *text;
} enum_desc_t;

typedef struct {
    const enum_desc_t *desc;
} proto_enum_desc_t;

typedef struct {
    const void *desc;
} proto_message_desc_t;

typedef struct {
    size_t offset_tag;
    int tag;
} proto_oneof_desc_t;

typedef struct {
    field_type_t field;
    field_label_t label;
    const char *name;
    size_t offset;
    size_t offset_optional;
    size_t offset_repeated;
    size_t offset_oneof;
    size_t element_size;
    const char *format;
    proto_message_desc_t message;
    union {
        proto_enum_desc_t enum_type;
        proto_oneof_desc_t oneof;
    };
} proto_desc_t;

void print_message(const void *message, const proto_desc_t *desc, int indent);
