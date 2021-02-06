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

def print_code(text, indent = 0):
    text = textwrap.dedent(text[1:])
    text = text.splitlines()
    text = ["\t" * indent + l + "\n" for l in text]
    return ''.join(text)

def indent_print(indent = 0, offset = None):
    if offset is None:
        offset = ""
    elif offset > 0:
        offset = " + %d" % offset
    elif offset < 0:
        offset = " - %d" % -offset
    return print_code("""
                    for (int j = 0; j < level%s; j++) {
                        printf("\\t");
                    }
                    """ % offset, indent)

def print_field(name, field, indent, offset, repeated = False, one_of = None):
    if repeated:
        repeated = "[i]"
    else:
        repeated = ""
    if one_of:
        one_of = "%s." %one_of
    else:
        one_of = ""
    c_text = indent_print(indent, offset)
    if field[0] == "TYPE_ENUM":
        c_text += print_code("""
                            printf("%s=%%s\\n", get_enum_text(%s_desc, msg->%s%s%s));
                            """ % (name, field[2], one_of, name, repeated), indent)
    elif field[0] == "TYPE_BYTES":
        c_text += print_code("""
                            print_bytes("%s", msg->%s.bytes, msg->%s%s%s.size);
                            """ % (name, name, one_of, name, repeated), indent)
    else:
        c_text += print_code("""
                            printf("%s=%%"%s"\\n", msg->%s%s%s);
                            """ % (name, _proto_type_to_c[field[0]], one_of, name, repeated), indent)
    return c_text

def parse_msg(message):
    c_text = ""
    h_text = ""
    h_text += print_code("""
                        void %s_print(%s *msg, uint8_t level);
                        """ % (message[0], message[0]))
    c_text += print_code("""
                        void
                        %s_print(%s *msg, uint8_t level)
                        {
                        """ % (message[0], message[0]))
    c_text += indent_print(indent = 1)
    c_text += print_code("""
                        printf("%s: \\n");
                        """ % message[0], indent = 1)

    oneofs = []
    for name, field in message[1]._asdict().items():
        print("\t field: %s" % name)
        if name == "oneofs_":
            oneofs = field
        elif isinstance(field, module_parser.Repeated):
            if type(field[0]) is not tuple:
                message_name = type(field[0]).__name__
            c_text += print_code("""
                                for (pb_size_t i = 0; i < msg->%s_count; i++) {
                                """ % name, indent = 1)
            if type(field[0]) is tuple: #repetead field is normal
                c_text += print_field(name, field[0], 2, 1, repeated = True, one_of = None)
            else: #repeated field is message
                c_text += print_code("""
                                    %s_print(&msg->%s[i], level + 1);
                                    """ % (message_name, name), indent = 2)
            c_text += print_code("""
                                }
                                """, indent = 1)
        elif isinstance(field[1], tuple):
            one_of = field_in_of(name, oneofs)
            if one_of:
                c_text += print_code("""
                                    if (msg->which_%s == %s_%s_tag) {
                                    """ % (one_of, message[0], name), indent = 1)
                c_text += indent_print(indent = 2, offset = 1)
                c_text += print_code("""
                                    %s_print(&msg->%s.%s, level + 1);
                                    """ % (type(field).__name__, one_of, name), indent = 2)
                c_text += print_code("""
                                    }
                                    """, indent = 1)
            else:
                c_text += print_code("""
                                    %s_print(&msg->%s, level + 1);
                                    """ % (type(field).__name__, name), indent = 1)
        else:
            one_of = field_in_of(name, oneofs)
            if field[1] != 'LABEL_OPTIONAL' and field[1] != 'LABEL_REQUIRED':
                print(field)
                raise Exception("Unknown label")
            elif one_of:
                c_text += print_code("""
                                    if (msg->which_%s == %s_%s_tag) {
                                    """ % (one_of, message[0], name), indent = 1)
                c_text += print_field(name, field, 2, 1, repeated = False, one_of = one_of)
                c_text += "\t}\n"
                continue
            elif field[1] == 'LABEL_OPTIONAL':
                c_text += print_code("""
                                    if (msg->has_%s) {
                                    """ % name, indent = 1)
            c_text += print_field(name, field, 1, 1, repeated = False, one_of = None)
            
            if field[1] == 'LABEL_OPTIONAL':
                c_text += print_code("""
                                    }
                                    """, indent = 1)

    c_text += print_code("""
                        }

                        """)

    return (c_text, h_text)

def parse_enum(enum):
    c_text = ""
    h_text = ""
    c_text = print_code("""
                        static enum_desc_t %s_desc[] = {
                        """ % enum[0])
    for name, field in enum[1]._asdict().items():
        c_text += print_code("""
                            {.idx = %d, .text = \"%s\"},
                            """ % (field[1], field[0]), indent = 1)
    c_text += print_code("""
                        {.idx = -1}
                        """, indent = 1)
    c_text += print_code("""
                        };

                        """)
    return (c_text, h_text)


module = sys.argv[1]
proto = importlib.import_module(module + "_pb2")

c_text = print_code("""
                    #include "autogen.h"
                    #include <inttypes.h>
                    #include <stdio.h>

                    """)

h_text = print_code("""
                    #pragma once
                    #include "%s.pb.h"

                    """ % module)

c_text += print_code("""
                    static char *
                    get_enum_text(enum_desc_t *desc, int idx)
                    {
                        int i = 0;
                        while (desc[i].idx != -1) {
                            if (desc[i].idx == idx) {
                                return desc[i].text;
                            }
                            i++;
                        }
                        return "Unknown enum";
                    }
                    
                    """)

c_text += print_code("""
                    static void
                    print_bytes(const char *name, const uint8_t *buf, size_t len)
                    {
                        printf("%s=\\"", name);
                        for (size_t i = 0; i < len; i++) {
                            printf("%.02x", buf[i]);
                        }
                        printf("\\"\\n");
                    }
                    
                    """)

h_text += print_code("""
                    typedef struct {
                        int idx;
                        char *text;
                    } enum_desc_t;

                    """)

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