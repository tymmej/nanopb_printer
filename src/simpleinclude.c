#include "simpleinclude.h"
#include <inttypes.h>
#include <stdio.h>

static const enum_desc_t SimpleIncludedEnum_desc[] = {
	{.idx = 1, .text = "INCLUDED"},
	{.idx = 2, .text = "EXCLUDED"},
	{.idx = (pb_size_t)-1}
};

const proto_desc_t SimpleIncluded_desc[] = {
	{.field = FIELD_ENUM, .label = LABEL_REQUIRED, .name = "enum_inc", .offset = offsetof(SimpleIncluded, enum_inc), .offset_optional = -1, .offset_repeated = -1, .element_size = -1, .enum_type.desc = SimpleIncludedEnum_desc},
    {.field = FIELD_LAST},
};

