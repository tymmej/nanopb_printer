import binascii
import simple_pb2


proto = simple_pb2.single_int(int = 127)
print(len(proto.SerializeToString()))
proto = simple_pb2.single_int(int = 128)
print(len(proto.SerializeToString()))
proto = simple_pb2.single_int(int = 16383)
print(len(proto.SerializeToString()))
proto = simple_pb2.single_int(int = 16384)
print(len(proto.SerializeToString()))

asdf

simple1="082a10001802"
simple2="101718022202a55a"
simplenested="120018012002"
simplerepeated="0a04617364660a047a7863761207080010a9011801120708001090011801"
simpleoneof="107b187c"


protos = simple_pb2.SimpleMessage1()
protos.ParseFromString(binascii.unhexlify(simple1))
print(protos)

protos = simple_pb2.SimpleMessage2()
protos.ParseFromString(binascii.unhexlify(simple2))
print(protos)

protos = simple_pb2.SimpleNested()
protos.ParseFromString(binascii.unhexlify(simplenested))
print(protos)

protos = simple_pb2.SimpleRepeated()
protos.ParseFromString(binascii.unhexlify(simplerepeated))
print(protos)

protos = simple_pb2.SimpleOneof()
protos.ParseFromString(binascii.unhexlify(simpleoneof))
print(protos)