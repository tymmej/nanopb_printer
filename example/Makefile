TARGET := example
SRC_DIRS := ../src .

SRCS := $(shell find $(SRC_DIRS) -name "*.cpp" -or -name "*.c" -or -name "*.s")
SRCS += $(shell find ../nanopb -maxdepth 2 -name "*.c")
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d) ../nanopb
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

LDFLAGS += 
CFLAGS ?= $(INC_FLAGS) -Wall -Wextra -ggdb3 -O0

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(OBJS) -o $@

clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
