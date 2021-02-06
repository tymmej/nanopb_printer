#pragma once
#include "simple.pb.h"

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
    int idx;
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
    union {
        proto_enum_desc_t enum_type;
        proto_message_desc_t message;
        proto_oneof_desc_t oneof;
    };
} proto_desc_t;

void print_message(const void *message, const proto_desc_t *desc, int indent);

void SimpleMessage1_print(SimpleMessage1 *msg, uint8_t level);
extern const proto_desc_t SimpleMessage1_desc[];
void SimpleMessage2_print(SimpleMessage2 *msg, uint8_t level);
extern const proto_desc_t SimpleMessage2_desc[];
void SimpleNested_print(SimpleNested *msg, uint8_t level);
extern const proto_desc_t SimpleNested_desc[];
void SimpleRepeated_print(SimpleRepeated *msg, uint8_t level);
extern const proto_desc_t SimpleRepeated_desc[];
void SimpleOneof_print(SimpleOneof *msg, uint8_t level);
extern const proto_desc_t SimpleOneof_desc[];
