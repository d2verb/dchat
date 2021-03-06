TARGET ?= dchat
SRC_DIRS ?= ./src
 
SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)
 
INC_DIRS := ./inc
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
 
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
 
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS) -lpthread
 
.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)
 
-include $(DEPS)
