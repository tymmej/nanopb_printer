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

def print_field(message, name, field, indent, offset, optional = False, repeated = False, one_of = None, submessage=None):
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
        element_size = "sizeof(((%s *)NULL)->%s) / (sizeof(((%s *)NULL)->%s) / sizeof(((%s *)NULL)->%s[0]))" % (message, name, message, name, message, name)
    else:
        repeated = ""
    if one_of:
        label = "LABEL_ONEOF"
        oneof_desc = ", .oneof.offset_tag = offsetof(%s, which_%s), .oneof.tag = %s_%s_tag" % (message, one_of, message, name)
        one_of = "%s." % one_of
    else:
        one_of = ""
    c_text = indent_print(indent, offset)
    if submessage is not None:
        desc_text += print_code("""
                                {.field = FIELD_MESSAGE, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s, .message.desc = %s_desc%s},
                                """ % (label, name, message, one_of, name, offset_optional, offset_repeated, element_size, submessage, oneof_desc), indent)
        return (c_text, desc_text)
    if field[0] == "TYPE_ENUM":
        c_text += print_code("""
                            printf("%s=%%s\\n", get_enum_text(%s_desc, msg->%s%s%s));
                            """ % (name, field[2], one_of, name, repeated), indent)
        desc_text += print_code("""
                            {.field = FIELD_ENUM, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s, .enum_type.desc = %s_desc%s},
                            """ % (label, name, message, one_of, name, offset_optional, offset_repeated, element_size, field[2], oneof_desc), indent)
    elif field[0] == "TYPE_BYTES":
        c_text += print_code("""
                            print_bytes("%s", msg->%s.bytes, msg->%s%s%s.size);
                            """ % (name, name, one_of, name, repeated), indent)
        desc_text += print_code("""
                            {.field = FIELD_BYTES, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s%s},
                            """ % (label, name, message, one_of, name, offset_optional, offset_repeated, element_size, oneof_desc), indent)
    else:
        c_text += print_code("""
                            printf("%s=%%"%s"\\n", msg->%s%s%s);
                            """ % (name, _proto_type_to_c[field[0]], one_of, name, repeated), indent)
        desc_text += print_code("""
                            {.field = FIELD_NORMAL, .label = %s, .name = "%s", .offset = offsetof(%s, %s%s), .offset_optional = %s, .offset_repeated = %s, .element_size = %s, .format = %s%s},
                            """ % (label, name, message, one_of, name, offset_optional, offset_repeated, element_size, _proto_type_to_c[field[0]], oneof_desc), indent)
    return (c_text, desc_text)

def parse_msg(message):
    c_text = ""
    h_text = ""
    desc_text = ""
    h_text += print_code("""
                        void %s_print(%s *msg, uint8_t level);
                        """ % (message[0], message[0]))
    desc_text += print_code("""
                            const proto_desc_t %s_desc[] = {
                            """ % message[0])
    h_text += print_code("""
                        extern const proto_desc_t %s_desc[];
                        """ % message[0])
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
                new_c, new_desc = print_field(message[0], name, field[0], 2, 1, repeated = True, one_of = None)
                c_text += new_c
                desc_text += new_desc
            else: #repeated field is message
                c_text += print_code("""
                                    %s_print(&msg->%s[i], level + 1);
                                    """ % (message_name, name), indent = 2)
            
                new_c, new_desc = print_field(message[0], name, type(field).__name__, 2, 1, repeated = True, submessage=message_name)
                #c_text += new_c
                desc_text += new_desc
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
            
            new_c, new_desc = print_field(message[0], name, type(field).__name__, 2, 1, repeated = False, one_of = one_of, submessage=type(field).__name__)
            #c_text += new_c
            desc_text += new_desc
        else:
            one_of = field_in_of(name, oneofs)
            if field[1] != 'LABEL_OPTIONAL' and field[1] != 'LABEL_REQUIRED':
                print(field)
                raise Exception("Unknown label")
            elif one_of:
                c_text += print_code("""
                                    if (msg->which_%s == %s_%s_tag) {
                                    """ % (one_of, message[0], name), indent = 1)
                new_c, new_desc = print_field(message[0], name, field, 2, 1, repeated = False, one_of = one_of)
                c_text += new_c
                desc_text += new_desc
                c_text += "\t}\n"
                continue
            elif field[1] == 'LABEL_OPTIONAL':
                c_text += print_code("""
                                    if (msg->has_%s) {
                                    """ % name, indent = 1)

            new_c, new_desc = print_field(message[0], name, field, 1, 1, optional = field[1] == 'LABEL_OPTIONAL', repeated = False, one_of = None)
            c_text += new_c
            desc_text += new_desc
            
            if field[1] == 'LABEL_OPTIONAL':
                c_text += print_code("""
                                    }
                                    """, indent = 1)

    c_text += print_code("""
                        }

                        """)

    desc_text += print_code("""
                                {.field = FIELD_LAST},
                            };
                            
                            """)

    return (desc_text + c_text, h_text)

def parse_enum(enum):
    c_text = ""
    h_text = ""
    c_text = print_code("""
                        static const enum_desc_t %s_desc[] = {
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
                    static const char *
                    get_enum_text(const enum_desc_t *desc, int idx)
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

                    """)

h_text += print_code("""
                    typedef struct {
                        int idx;
                        const char *text;
                    } enum_desc_t;

                    """)

h_text += print_code("""

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

                    """)

h_text += print_code("""
                    void print_message(const void *message, const proto_desc_t *desc, int indent);

                    """)

c_text += print_code("""
                    void
                    print_message(const void *message, const proto_desc_t *desc, int indent)
                    {
                        for (int idx = 0; desc[idx].field != FIELD_LAST; idx++) {
                            pb_size_t count = 1;
                            if (desc[idx].label == LABEL_OPTIONAL) {
                                if (!(*(bool *)((uint8_t *)message + desc[idx].offset_optional))) {
                                    count = 0;
                                }
                            }
                            if (desc[idx].label == LABEL_REPEATED) {
                                count = *(pb_size_t *)((uint8_t *)message + desc[idx].offset_repeated);
                            }
                            if (desc[idx].label == LABEL_ONEOF) {
                                count = 0;
                                if (*(pb_size_t *)((uint8_t *)message + desc[idx].oneof.offset_tag) == desc[idx].oneof.tag) {
                                    count = 1;
                                }
                            }
                            for (pb_size_t i = 0; i < count; i++) {
                                for (int j = 0; j < indent; j++) {
                                    printf("\\t");
                                }
                                if (desc[idx].field == FIELD_NORMAL) {
                                    char format[8] = { 0 };
                                    format[0] = '%';
                                    strcpy(&format[1], desc[idx].format);
                                    printf("%s=", desc[idx].name);
                                    if (strcmp(&format[1], "s") == 0) {
                                        printf(format, ((uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size)));
                                    } else {
                                        printf(format, *((uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size)));
                                    }
                                    printf("\\n");
                                }
                                if (desc[idx].field == FIELD_ENUM) {
                                    printf("%s=%s", desc[idx].name, get_enum_text(desc[idx].enum_type.desc, *((uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size))));
                                    printf("\\n");
                                }
                                if (desc[idx].field == FIELD_BYTES) {
                                    print_bytes(desc[idx].name, (uint8_t *)message + desc[idx].offset + sizeof(pb_size_t), *(pb_size_t *)((uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size)));
                                    printf("\\n");
                                }
                                if (desc[idx].field == FIELD_MESSAGE) {
                                    printf("%s:\\n", desc[idx].name);
                                    print_message((uint8_t *)message + desc[idx].offset + (i * desc[idx].element_size), desc[idx].message.desc, indent  + 1);
                                    printf("\\n");
                                }
                            }             
                        }
                    }
                    
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