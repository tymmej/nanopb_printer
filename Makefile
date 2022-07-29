 .PHONY: example tests clean proto parser

PROTOS_FILES = simple simpleinclude
PROTOS_PATH = ./src/proto

PROTOS = $(shell find parser -maxdepth 2 -name "*_pb2.py")

example:
	make -C ./example

tests:
	make -C ./tests

clean:
	make -C ./example clean
	make -C ./tests clean
	$(foreach proto,$(PROTOS_FILES),rm -f parser/$(proto)_pb2.py;)
	$(foreach proto,$(PROTOS_FILES),rm -f $(PROTOS_PATH)/$(proto).pb.*;)
	$(foreach proto,$(PROTOS_FILES),rm -f $(PROTOS_PATH)/nanopb_printer_$(proto).*;)

proto:
	$(foreach proto,$(PROTOS_FILES),nanopb_generator -I $(PROTOS_PATH) $(PROTOS_PATH)/$(proto).proto;mv $(proto).pb.* $(PROTOS_PATH);protoc -I=$(PROTOS_PATH) --python_out=parser $(proto).proto;)

parser:
	$(foreach proto,$(PROTOS_FILES),python3 parser/parser.py $(proto) $(PROTOS_PATH);)
