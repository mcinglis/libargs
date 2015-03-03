
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

uc = $(shell echo $(1) | tr [:lower:] [:upper:])

types := bool ord char schar uchar short ushort int uint long ulong \
         llong ullong int8 uint8 int16 uint16 int32 uint32 \
         intmax uintmax ptrdiff wchar size

int8_type    := int8_t
uint8_type   := uint8_t
int16_type   := int16_t
uint16_type  := uint16_t
int32_type   := int32_t
uint32_type  := uint32_t
intmax_type  := intmax_t
uintmax_type := uintmax_t
ptrdiff_type := ptrdiff_t
wchar_type   := wchar_t
size_type    := size_t
ptr_type     := void const *
ptrm_type    := void *

arg_parse_headers := $(foreach t,$(types),arg-parse/$t.h)
arg_parse_sources := $(foreach t,$(types),arg-parse/$t.c)

sources  := $(wildcard *.c) $(arg_parse_sources)
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


libargs.o: $(objects)
	$(LD) -r $^ -o $@


.PHONY: examples
examples: $(examples_binaries)


examples/demo: \
    $(DEPS_DIR)/libbase/bool.o \
    $(DEPS_DIR)/libbase/char.o \
    $(DEPS_DIR)/libbase/int.o \
    $(DEPS_DIR)/libstr/str.o \
    $(DEPS_DIR)/libarray/array-str.o \
    arg-parse.o


$(arg_parse_headers) $(arg_parse_sources): $(RENDER_JINJA_SCRIPT)

$(arg_parse_sources): %.c: %.h

$(arg_parse_headers): arg-parse/%.h: arg-parse/header.h.jinja
	$(eval n := $*)
	$(eval N := $(call uc,$n))
	$(RENDER_JINJA) $< "include_guard=LIBARGS_ARG_PARSE_$N_H" "sys_headers=" "rel_headers=" "funcname=$n" -o $@

$(arg_parse_sources): %.c: %.h
$(arg_parse_sources): arg-parse/%.c: arg-parse/source.c.jinja
	$(eval n := $*)
	$(eval N := $(call uc,$n))
	$(RENDER_JINJA) $< "header=$n.h" "sys_headers=libbase/$n.h" "rel_headers=" "type=$(or $($(n)_type),$n)" "funcname=$n" -o $@


.PHONY: clean
clean:
	rm -rf $(arg_parse_headers) $(arg_parse_sources) $(objects) libargs.o $(mkdeps) $(examples_binaries)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


-include $(mkdeps)

