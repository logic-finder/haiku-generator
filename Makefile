## Variables ##
TARGET := haiku
SHELL := /bin/sh

CC := gcc
CFLAGS := -W -Wall -Wextra -pedantic
CFLAGS += -MMD -MP
CFLAGS += -g
CFLAGS += -O0
CFLAGS += --std=c2x
# CFLAGS += -fsanitize=leak,address,undefined

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
DEPENDS = $(OBJECTS:.o=.d)

## Targets ##
.PHONY: all clean test

# Default Goal #
all: $(TARGET)

# Build #
$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@

$(OBJECTS): %.o: %.c
	$(CC) $< $(CPPFLAGS) $(CFLAGS) -c -o $@

# Dependency #
-include $(DEPENDS)

# Tasks #
clean:
	rm -f $(TARGET) *.o *.d
	$(MAKE) --directory=test clean

test:
	$(MAKE) --directory=test run
