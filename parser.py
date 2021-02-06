import importlib
import sys

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
    # "TYPE_BYTES": ,
    "TYPE_UINT32": "PRIu32",
    #"TYPE_ENUM": , extra if
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

def parse_msg(message):
    print(message)
    c_text = ""
    h_text = ""
    h_text += "void %s_print(%s *msg, uint8_t level);\n" % (message[0], message[0])
    c_text += "void\n%s_print(%s *msg, uint8_t level) {\n" % (message[0], message[0])
    c_text += "\tfor (int i = 0; i < level; i++) {\n\t\tprintf(\"\\t\");\n\t}\n"
    c_text += "\tprintf(\"%s: \\n\");\n" % message[0]

    oneofs = []
    for name, field in message[1]._asdict().items():
        print(name, field)
        if name == "oneofs_":
            oneofs = field
        elif isinstance(field, module_parser.Repeated):
            if type(field[0]) is not tuple:
                message_name = type(field[0]).__name__
            if type(field[0]) is tuple:
                c_text += "\tfor (pb_size_t i = 0; i < msg->%s_count; i++) {\n\t\tfor (int j = 0; j < level + 1; j++) {\n\t\t\tprintf(\"\\t\");\n\t\t}\n\t\t" % name
                c_text += "printf(\"%s=%%\"%s\"\\n\", msg->%s[i]);" % (name, _proto_type_to_c[field[0][0]], name)
            else:
                c_text += "\tfor (pb_size_t i = 0; i < msg->%s_count; i++) {\n\t\tfor (int j = 0; j < level; j++) {\n\t\t\tprintf(\"\\t\");\n\t\t}\n\t\t" % name
                c_text += "%s_print(&msg->%s[i], level + 1);" % (message_name, name)
            c_text += "\n\t}\n"
        elif isinstance(field[1], tuple):
            one_of = field_in_of(name, oneofs)
            if one_of:
                c_text += "\tif (msg->which_%s == %s_%s_tag) {\n\t" % (one_of, message[0], name)
                c_text += "\t%s_print(&msg->%s.%s, level + 1);\n" % (type(field).__name__, one_of, name)
                c_text += "\t}\n"
            else:
                c_text += "\t%s_print(&msg->%s, level + 1);\n" % (type(field).__name__, name)
        else:
            one_of = field_in_of(name, oneofs)
            if field[1] != 'LABEL_OPTIONAL' and field[1] != 'LABEL_REQUIRED':
                print(field)
                raise Exception("Unknown label")
            elif one_of:
                c_text += "\tif (msg->which_%s == %s_%s_tag) {\n\t" % (one_of, message[0], name)
                c_text += "\tfor (int i = 0; i < level + 1; i++) {\n\t\t\tprintf(\"\\t\");\n\t\t}\n"
                if field[0] == "TYPE_ENUM":
                    c_text += "\t\tprintf(\"%s=%%s\\n\", get_enum_text(%s_desc, msg->%s.%s));\n" % (name, field[2], one_of, name)
                else:
                    c_text += "\t\tprintf(\"%s=%%\"%s\"\\n\", msg->%s.%s);\n" % (name, _proto_type_to_c[field[0]], one_of, name)
                c_text += "\t}\n"
                continue
            elif field[1] == 'LABEL_OPTIONAL':
                c_text += "\tif (msg->has_%s) {\n" % name
            c_text += "\tfor (int i = 0; i < level + 1; i++) {\n\t\tprintf(\"\\t\");\n\t}\n"
            if field[0] == "TYPE_ENUM":
                c_text += "\tprintf(\"%s=%%s\\n\", get_enum_text(%s_desc, msg->%s));\n" % (name, field[2], name)
            else:
                c_text += "\tprintf(\"%s=%%\"%s\"\\n\", msg->%s);\n" % (name, _proto_type_to_c[field[0]], name)
            
            if field[1] == 'LABEL_OPTIONAL':
                c_text += "\t}\n"

    c_text += "}\n\n"

    return (c_text, h_text)

def parse_enum(enum):
    print(enum)
    c_text = ""
    h_text = ""
    c_text = "static enum_desc_t %s_desc[] = {\n" % enum[0]
    for name, field in enum[1]._asdict().items():
        c_text += "\t{.idx = %d, .text = \"%s\"},\n" % (field[1], field[0])
    c_text += "\t{.idx = -1}\n"
    c_text += "};\n\n"
    return (c_text, h_text)


module = sys.argv[1]
proto = importlib.import_module(module + "_pb2")

c_text = "#include \"autogen.h\"\n#include <inttypes.h>\n#include <stdio.h>\n\n"
h_text ="#pragma once\n#include \"%s.pb.h\"\n\n" % module

c_text += "static char *\nget_enum_text(enum_desc_t *desc, int idx) {\n\tint i = 0;\n\twhile (desc[i].idx != -1) {\n\t\tif (desc[i].idx == idx) {\n\t\t\treturn desc[i].text;\n\t\t}\n\t\ti++;\n\t}\n\treturn NULL;\n}\n\n"

h_text += "typedef struct {\n\tint idx;\n\tchar *text;\n} enum_desc_t;\n\n"

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

f = open("src/autogen.h", "w")
f.write(h_text)
f.close()

f = open("src/autogen.c", "w")
f.write(c_text)
f.close()