import importlib
import sys
import textwrap

import module_parser

_proto_type_to_c = {
    "TYPE_DOUBLE": "f",
    "TYPE_FLOAT": "f",
    "TYPE_INT64": "PRId64",
    "TYPE_UINT64": "PRIu64",
    "TYPE_INT32": "PRId32",
    "TYPE_FIXED64": "PRIu64",
    "TYPE_FIXED32": "PRIu32",
    "TYPE_BOOL": "\"d\"",
    "TYPE_STRING": "\"s\"",
    # "TYPE_GROUP": ,
    # "TYPE_MESSAGE": ,
    # "TYPE_BYTES": , extra if
    "TYPE_UINT32": "PRIu32",
    # "TYPE_ENUM": , extra if
    "TYPE_SFIXED32": "PRId32",
    "TYPE_SFIXED64": "PRId64",
    "TYPE_SINT32": "PRId32",
    "TYPE_SINT64": "PRId64",
}


def field_in_of(field, oneofs):
    for k, v in oneofs.items():
        if field in v:
            return k
    return None


def print_code(text, indent=0):
    text = textwrap.dedent(text[1:])
    text = text.splitlines()
    text = ["\t" * indent + line + "\n" for line in text]
    return ''.join(text)


def print_field(message, name, field, indent, offset, optional=False,
                repeated=False, one_of=None, submessage=None):
    desc_text = ""
    label = "LABEL_REQUIRED"
    offset_optional = "-1"
    offset_repeated = "-1"
    oneof_desc = ""
    element_size = "-1"
    if optional:
        label = "LABEL_OPTIONAL"
        offset_optional = "offsetof(%s, has_%s)" % (message, name)
    if repeated:
        repeated = "[i]"
        label = "LABEL_REPEATED"
        offset_repeated = "offsetof(%s, %s_count)" % (message, name)
        element_size = "sizeof(((%s *)NULL)->%s[0])" % (message, name)
    else:
        repeated = ""
    if one_of:
        label = "LABEL_ONEOF"
        oneof_desc = ", .oneof.offset_tag = offsetof(%s, which_%s), .oneof.tag = %s_%s_tag" % (message, one_of, message, name)
        one_of = "%s." % one_of
    else:
        one_of = ""
    if submessage is not None:
        desc_text += print_code("""
                            {.field = FIELD_MESSAGE, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s, .message.desc = %s_desc%s},
                            """ % (label, name, message, one_of,
                                   name, offset_optional, offset_repeated,
                                   element_size, submessage, oneof_desc),
                                indent)
        return desc_text
    if field[0] == "TYPE_ENUM":
        desc_text += print_code("""
                            {.field = FIELD_ENUM, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s, .enum_type.desc = %s_desc%s},
                            """ % (label, name, message, one_of, name,
                                   offset_optional, offset_repeated,
                                   element_size, field[2], oneof_desc),
                                indent)
    elif field[0] == "TYPE_BYTES":
        desc_text += print_code("""
                            {.field = FIELD_BYTES, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s%s},
                            """ % (label, name, message, one_of, name,
                                   offset_optional, offset_repeated,
                                   element_size, oneof_desc),
                                indent)
    else:
        desc_text += print_code("""
                            {.field = FIELD_NORMAL, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s, .format = %s%s},
                            """ % (label, name, message, one_of, name,
                                   offset_optional, offset_repeated,
                                   element_size, _proto_type_to_c[field[0]],
                                   oneof_desc),
                                indent)
    return desc_text


def parse_msg(message):
    h_text = ""
    desc_text = ""
    desc_text += print_code("""
                            const nanopb_printer_proto_desc_t %s_desc[] = {
                            """ % message[0])
    h_text += print_code("""
                        extern const nanopb_printer_proto_desc_t %s_desc[];
                        """ % message[0])

    oneofs = []
    for name, field in message[1]._asdict().items():
        print("\t field: %s" % name)
        if name == "oneofs_":
            oneofs = field
        elif isinstance(field, module_parser.Repeated):
            if type(field[0]) is tuple:  # repetead field is normal
                new_desc = print_field(message[0], name, field[0], 1, 1,
                                       repeated=True, one_of=None)
                desc_text += new_desc
            else:  # repeated field is message
                message_name = type(field[0]["field"]).__name__
                new_desc = print_field(message[0], name, type(field).__name__,
                                       1, 1, repeated=True,
                                       submessage=message_name)
                desc_text += new_desc
        elif isinstance(field, dict):
            one_of = field_in_of(name, oneofs)
            optional = field["label"] == 'LABEL_OPTIONAL' and one_of is None
            new_desc = print_field(message[0], name,
                                   type(field["field"]).__name__,
                                   1, 1, optional=optional, repeated=False,
                                   one_of=one_of,
                                   submessage=type(field["field"]).__name__)
            desc_text += new_desc
        else:
            one_of = field_in_of(name, oneofs)
            if field[1] != 'LABEL_OPTIONAL' and field[1] != 'LABEL_REQUIRED':
                print(field)
                raise Exception("Unknown label")
            elif one_of:
                new_desc = print_field(message[0], name, field, 1, 1,
                                       repeated=False, one_of=one_of)
                desc_text += new_desc
                continue

            new_desc = print_field(message[0], name, field, 1, 1,
                                   optional=(field[1] == 'LABEL_OPTIONAL'),
                                   repeated=False, one_of=None)
            desc_text += new_desc

    desc_text += print_code("""
                            {.field = FIELD_LAST},
                            };

                            """, indent=1)

    return (desc_text, h_text)


def parse_enum(enum):
    c_text = ""
    h_text = ""
    c_text = print_code("""
                        static const nanopb_printer_enum_desc_t %s_desc[] = {
                        """ % enum[0])
    for _, field in enum[1]._asdict().items():
        c_text += print_code("""
                            {.idx = %d, .text = \"%s\"},
                            """ % (field[1], field[0]), indent=1)
    c_text += print_code("""
                        {.idx = (pb_size_t)-1}
                        """, indent=1)
    c_text += print_code("""
                        };

                        """)
    return (c_text, h_text)


module = sys.argv[1]
dest_path = sys.argv[2]
proto = importlib.import_module(module + "_pb2")

included = []
attributes = dir(proto)
for attr in attributes:
    if attr.endswith("__pb2"):
        included.append(attr.replace("__pb2", ""))

c_text = print_code("""
                    #include "nanopb_printer_%s.h"
                    #include <inttypes.h>
                    #include <stdio.h>

                    """ % module)

h_text = print_code("""
                    #pragma once

                    #include <nanopb_printer/nanopb_printer.h>

                    #include "%s.pb.h"
                    #include "nanopb_printer_%s.h"

                    """ % (module, module))

for inc in included:
    h_text += print_code("""
                        #include "%s.pb.h"
                        #include "nanopb_printer_%s.h"

                        """ % (inc, inc))

enums = module_parser.module_enums(proto)

for enum in enums.items():
    print("Parsing %s" % enum[0])
    new_c, new_h = parse_enum(enum)
    c_text += new_c
    h_text += new_h

for message in module_parser.module_msgs(proto).items():
    print("Parsing %s" % message[0])
    new_c, new_h = parse_msg(message)
    c_text += new_c
    h_text += new_h

f = open("{}/nanopb_printer_{}.h".format(dest_path, module), "w")
f.write(h_text)
f.close()

f = open("{}/nanopb_printer_{}.c".format(dest_path, module), "w")
f.write(c_text)
f.close()
