#pragma once
#include "simple.pb.h"

typedef struct {
    int idx;
    char *text;
} enum_desc_t;

void SimpleMessage1_print(SimpleMessage1 *msg, uint8_t level);
void SimpleMessage2_print(SimpleMessage2 *msg, uint8_t level);
void SimpleNested_print(SimpleNested *msg, uint8_t level);
void SimpleRepeated_print(SimpleRepeated *msg, uint8_t level);
void SimpleOneof_print(SimpleOneof *msg, uint8_t level);
