"""
https://gist.github.com/trianta2/fd04bdbfc9bdef5631c0d76582a04aca
"""

from collections import namedtuple

from google.protobuf.descriptor import FieldDescriptor
from google.protobuf.internal.enum_type_wrapper import EnumTypeWrapper

_type_dict = {v: k for k, v in vars(FieldDescriptor).items() if k.startswith('TYPE_')}
_label_dict = {v: k for k, v in vars(FieldDescriptor).items() if k.startswith('LABEL_')}

Repeated = namedtuple('Repeated', ['value'])
Map = namedtuple('Map', ['key', 'value'])

def _field_type(field, context):
    '''Helper that returns either a str or nametuple corresponding to the field type'''
    if field.message_type is not None:
        return message_as_namedtuple(field.message_type, context)
    else:
        if (_type_dict[field.type] == "TYPE_ENUM"):
            return _type_dict[field.type], _label_dict[field.label], field.enum_type.name
        if (_label_dict[field.label] == "LABEL_REPEATED"):
            return _type_dict[field.type], _label_dict[field.label], field.name
        return _type_dict[field.type], _label_dict[field.label]


def field_type(field, context):
    '''Returns the protobuf type for a given field descriptor
    A Repeated, Map, or str object may be returned. Strings correspond to protobuf types.
    '''
    if field.label == FieldDescriptor.LABEL_REPEATED:
        msg_type = field.message_type
        is_map = msg_type is not None and msg_type.GetOptions().map_entry
        if is_map:
            key = _field_type(field.message_type.fields[0], context)
            value = _field_type(field.message_type.fields[1], context)
            return Map(key, value)
        else:
            value = _field_type(field, context)
            return Repeated(value)
    else:
        return _field_type(field, context)


def message_as_namedtuple(descr, context):
    '''Returns a namedtuple corresponding to the given message descriptor'''
    name = descr.name
    if name not in context:
        normal = [f.name for f in descr.fields]
        oneofs = [{}]
        for k, v in descr.oneofs_by_name.items():
            oneofs.append({k: [l.name for l in v.fields]})
        if len(oneofs) > 1:
            oneofs = oneofs[1:]
        Msg = namedtuple(name, ["oneofs_"] + normal)
        normal = [field_type(f, context) for f in descr.fields]
        context[name] = Msg(*(oneofs + normal))
    return context[name]

def enum_as_namedtuple(descr, context):
    '''Returns a namedtuple corresponding to the given enum descriptor'''
    name = descr.name
    if name not in context:
        values = list(descr.values)
        Msg = namedtuple(name, [k.name for k in values])
        context[name] = Msg(*((k.name, k.number) for k in values))
    return context[name]


def module_msgs(module):
    '''Returns a dict of {message name: namedtuple} from a given protobuf module'''
    context = dict()
    return {k: message_as_namedtuple(v, context)
            for k, v in module.DESCRIPTOR.message_types_by_name.items()}


def module_enums(module):
    '''Returns a dict of {enum name: namedtuple} from a given protobuf module'''
    context = dict()
    return {k: enum_as_namedtuple(v, context)
            for k, v in module.DESCRIPTOR.enum_types_by_name.items()}


def is_message(field):
    '''Helper that returns True if a field is a custom message type'''
    return isinstance(field, tuple)
