import binascii
import simple_pb2

singlenumber="0863"
simple1="082a10001802"
simple2="101718022202a55a"
simplenested="120018012002"
simplerepeated="0a04617364660a047a7863761207080010a9011801120708001090011801"
simpleoneof="107b187c"
simpleoneof2="0a06082a10181802188e01"
simpleincluded="0a021802"

protos = simple_pb2.SingleNumber()
protos.ParseFromString(binascii.unhexlify(singlenumber))
print(protos)

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

protos = simple_pb2.SimpleOneof()
protos.ParseFromString(binascii.unhexlify(simpleoneof2))
print(protos)

protos = simple_pb2.SimpleIncluding()
protos.ParseFromString(binascii.unhexlify(simpleincluded))
print(protos)