
##############################
### VARIABLES
##############################

DEPS_DIR ?= ./deps

CPPFLAGS += -I$(DEPS_DIR)

cflags_std := -std=c11
cflags_warnings := -Wall -Wextra -pedantic \
                   -Wcomments -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
                   -Wnested-externs -Wold-style-definition -Wredundant-decls \
                   -Wshadow -Wstrict-prototypes -Wunused-macros -Wvla \
                   -Wwrite-strings \
                   -Wno-unused-parameter

CFLAGS ?= $(cflags_std) -g $(cflags_warnings)

PYTHON ?= python

RENDER_JINJA_SCRIPT ?= $(DEPS_DIR)/render-jinja/render_jinja.py
RENDER_JINJA ?= $(PYTHON) $(RENDER_JINJA_SCRIPT)

sources  := $(wildcard *.c)
objects  := $(sources:.c=.o)
mkdeps   := $(sources:.c=.dep.mk)

examples_sources  := $(wildcard examples/*.c)
examples_binaries := $(examples_sources:.c=)


##############################
### BUILDING
##############################

.PHONY: all
all: objects examples


.PHONY: fast
fast: CPPFLAGS += -DNDEBUG
fast: CFLAGS = $(cflags_std) -O3 $(cflags_warnings)
fast: all


.PHONY: objects
objects: $(objects)


.PHONY: examples
examples: $(examples_binaries)


examples/demo: \
    $(DEPS_DIR)/libbase/bool.o \
    $(DEPS_DIR)/libbase/char.o \
    $(DEPS_DIR)/libbase/int.o \
    $(DEPS_DIR)/libstr/str.o \
    $(DEPS_DIR)/libarray/array-str.o \
    arg-parse.o


.PHONY: clean
clean:
	rm -rf $(objects) $(mkdeps) $(examples_binaries)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


-include $(mkdeps)

