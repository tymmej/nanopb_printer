#pragma once

#include <nanopb_printer/nanopb_printer.h>

#include "simple.pb.h"
#include "simple.h"

#include "simpleinclude.pb.h"
#include "simpleinclude.h"

extern const nanopb_printer_proto_desc_t SingleNumber_desc[];
extern const nanopb_printer_proto_desc_t SimpleMessage1_desc[];
extern const nanopb_printer_proto_desc_t SimpleMessage2_desc[];
extern const nanopb_printer_proto_desc_t SimpleNested_desc[];
extern const nanopb_printer_proto_desc_t SimpleRepeated_desc[];
extern const nanopb_printer_proto_desc_t SimpleOneof_desc[];
extern const nanopb_printer_proto_desc_t SimpleIncluding_desc[];
