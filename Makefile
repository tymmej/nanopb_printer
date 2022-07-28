 .PHONY: example tests

PROTOS = simple simpleinclude

example:
	make -C ./example

tests:
	make -C ./tests

clean:
	make -C ./example clean
	make -C ./tests clean
	$(foreach proto,$(PROTOS),rm -f $(proto)_pb2.py;)
	$(foreach proto,$(PROTOS),rm -f src/$(proto).pb.*;)
	$(foreach proto,$(PROTOS),rm -f src/$(proto).*;)

proto:
	$(foreach proto,$(PROTOS),nanopb_generator $(proto).proto;mv $(proto).pb.* ./src;protoc -I=. --python_out=. $(proto).proto;)

parser:
	$(foreach proto,$(PROTOS),python3 parser.py $(proto);)
