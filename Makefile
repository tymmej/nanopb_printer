TARGET ?= simple.out
SRC_DIRS ?= ./src

PROTOS = simple simpleinclude

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -or -name "*.c" -or -name "*.s")
SRCS += $(shell find nanopb -maxdepth 2 -name "*.cpp" -or -name "*.c" -or -name "*.s")
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) nanopb
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -Wall -Wextra -ggdb3 -O0

$(TARGET): $(OBJS)
	echo $(SRCS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

proto:
	$(foreach proto,$(PROTOS),nanopb_generator $(proto).proto;mv $(proto).pb.* ./src;protoc -I=. --python_out=. $(proto).proto;)

parser:
	$(foreach proto,$(PROTOS),python3 parser.py $(proto);)
	
-include $(DEPS)
