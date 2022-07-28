#include "simple.h"
#include <inttypes.h>
#include <stdio.h>

static const nanopb_printer_enum_desc_t SimpleEnum1_desc[] = {
	{.idx = 1, .text = "ONE"},
	{.idx = 2, .text = "TWO"},
	{.idx = (pb_size_t)-1}
};

static const nanopb_printer_enum_desc_t SimpleEnum2_desc[] = {
	{.idx = 1, .text = "ONEONE"},
	{.idx = 2, .text = "TWOTWO"},
	{.idx = (pb_size_t)-1}
};

const nanopb_printer_proto_desc_t SingleNumber_desc[] = {
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "number", .offset = offsetof(SingleNumber, number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32},
	{.field = FIELD_LAST},
	};
	
const nanopb_printer_proto_desc_t SimpleMessage1_desc[] = {
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "lucky_number", .offset = offsetof(SimpleMessage1, lucky_number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32},
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "unlucky_number", .offset = offsetof(SimpleMessage1, unlucky_number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32},
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum1", .offset = offsetof(SimpleMessage1, enum1), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum1_desc},
	{.field = FIELD_LAST},
	};
	
const nanopb_printer_proto_desc_t SimpleMessage2_desc[] = {
	{.field = FIELD_NORMAL, .label = LABEL_OPTIONAL, .name = "unlucky_number", .offset = offsetof(SimpleMessage2, unlucky_number), .offset_optional = offsetof(SimpleMessage2, has_unlucky_number), .offset_repeated = -1, .element_size = -1, .format = PRId64},
	{.field = FIELD_NORMAL, .label = LABEL_OPTIONAL, .name = "lucky_number", .offset = offsetof(SimpleMessage2, lucky_number), .offset_optional = offsetof(SimpleMessage2, has_lucky_number), .offset_repeated = -1, .element_size = -1, .format = PRId64},
	{.field = FIELD_ENUM, .label = LABEL_OPTIONAL, .name = "enum1", .offset = offsetof(SimpleMessage2, enum1), .offset_optional = offsetof(SimpleMessage2, has_enum1), .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum2_desc},
	{.field = FIELD_BYTES, .label = LABEL_OPTIONAL, .name = "byte", .offset = offsetof(SimpleMessage2, byte), .offset_optional = offsetof(SimpleMessage2, has_byte), .offset_repeated = -1, .element_size = -1},
	{.field = FIELD_LAST},
	};
	
const nanopb_printer_proto_desc_t SimpleNested_desc[] = {
	{.field = FIELD_MESSAGE, .label = LABEL_OPTIONAL, .name = "nested1", .offset = offsetof(SimpleNested, nested1), .offset_optional = offsetof(SimpleNested, has_nested1), .offset_repeated = -1, .element_size = -1, .message.desc = SimpleMessage1_desc},
	{.field = FIELD_MESSAGE, .label = LABEL_REQUIRED, .name = "nested2", .offset = offsetof(SimpleNested, nested2), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .message.desc = SimpleMessage2_desc},
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum1", .offset = offsetof(SimpleNested, enum1), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum1_desc},
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum2", .offset = offsetof(SimpleNested, enum2), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleEnum2_desc},
	{.field = FIELD_LAST},
	};
	
const nanopb_printer_proto_desc_t SimpleRepeated_desc[] = {
	{.field = FIELD_NORMAL, .label = LABEL_REPEATED, .name = "text", .offset = offsetof(SimpleRepeated, text), .offset_optional = -1, .offset_repeated = offsetof(SimpleRepeated, text_count), .element_size = sizeof(((SimpleRepeated *)NULL)->text[0]), .format = "s"},
	{.field = FIELD_MESSAGE, .label = LABEL_REPEATED, .name = "message", .offset = offsetof(SimpleRepeated, message), .offset_optional = -1, .offset_repeated = offsetof(SimpleRepeated, message_count), .element_size = sizeof(((SimpleRepeated *)NULL)->message[0]), .message.desc = SimpleMessage1_desc},
	{.field = FIELD_LAST},
	};
	
const nanopb_printer_proto_desc_t SimpleOneof_desc[] = {
	{.field = FIELD_MESSAGE, .label = LABEL_ONEOF, .name = "msg1", .offset = offsetof(SimpleOneof, oneofmsg.msg1), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .message.desc = SimpleMessage1_desc, .oneof.offset_tag = offsetof(SimpleOneof, which_oneofmsg), .oneof.tag = SimpleOneof_msg1_tag},
	{.field = FIELD_NORMAL, .label = LABEL_ONEOF, .name = "unlucky_number", .offset = offsetof(SimpleOneof, oneofmsg.unlucky_number), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId32, .oneof.offset_tag = offsetof(SimpleOneof, which_oneofmsg), .oneof.tag = SimpleOneof_unlucky_number_tag},
	{.field = FIELD_NORMAL, .label = LABEL_REQUIRED, .name = "notnested", .offset = offsetof(SimpleOneof, notnested), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .format = PRId64},
	{.field = FIELD_LAST},
	};
	
const nanopb_printer_proto_desc_t SimpleIncluding_desc[] = {
	{.field = FIELD_MESSAGE, .label = LABEL_REQUIRED, .name = "included", .offset = offsetof(SimpleIncluding, included), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .message.desc = SimpleIncluded_desc},
	{.field = FIELD_LAST},
	};
	
