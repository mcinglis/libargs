
##############################
### VARIABLES
##############################

DEPS_DIR ?= ./deps

LIBBASE ?= $(DEPS_DIR)/libbase
LIBSTR ?= $(DEPS_DIR)/libstr
LIBMAYBE ?= $(DEPS_DIR)/libmaybe
LIBARRAY ?= $(DEPS_DIR)/libarray

CPPFLAGS += -I$(DEPS_DIR)

cflags_std := -std=c11
cflags_warnings := -Wall -Wextra -pedantic \
                   -Wcomments -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
                   -Wnested-externs -Wold-style-definition -Wredundant-decls \
                   -Wshadow -Wstrict-prototypes -Wunused-macros -Wvla \
                   -Wwrite-strings \
                   -Wno-unused-parameter

CFLAGS ?= $(cflags_std) -g $(cflags_warnings)

TPLRENDER ?= $(DEPS_DIR)/tplrender/tplrender


libbase_types  := size bool int
libmaybe_types := size
libarray_types := str

size_type    := size_t
size_options := --typeclasses BOUNDED EQ ORD ENUM NUM FROM_STR TO_STRM \
                --extra num_type=unsigned

bool_type    := bool
bool_options := --typeclasses BOUNDED EQ ORD ENUM FROM_STR TO_STR \
                --extra min_bound=false max_bound=true

int_type := int
int_options := --typeclasses BOUNDED EQ ORD ENUM NUM FROM_STR TO_STRM \
               --extra num_type=signed


str_type    := char const *
str_options := --typename str --typeclasses EQ ORD
str_func_header := libstr/str.h

libbase_sources := $(foreach t,$(libbase_types),$(LIBBASE)/$t.c)
libbase_headers := $(libbase_sources:.c=.h)
libbase_objects := $(libbase_sources:.c=.o)

libmaybe_defs := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/def/maybe-$t.h)

libarray_sources := $(foreach t,$(libarray_types),$(LIBARRAY)/array-$t.c)
libarray_headers := $(libarray_sources:.c=.h)
libarray_defs    := $(foreach t,$(libarray_types),$(LIBARRAY)/def/array-$t.h)
libarray_objects := $(libarray_sources:.c=.o)

gen_objects := $(libbase_objects) $(libarray_objects)

gen := $(libbase_sources) \
       $(libbase_headers) \
       $(libmaybe_defs) \
       $(libarray_sources) \
       $(libarray_headers) \
       $(libarray_defs) \
       $(gen_objects)

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

.PHONY: clean
clean:
	rm -rf $(objects) $(mkdeps) $(examples_binaries) $(gen)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


arg-parse.o: $(LIBMAYBE)/def/maybe-size.h \
             $(LIBARRAY)/def/array-str.h \
             $(LIBARRAY)/array-str.h

examples/demo: \
    $(LIBBASE)/bool.o \
    $(LIBBASE)/int.o \
    $(LIBBASE)/size.o \
    $(LIBSTR)/str.o \
    $(LIBARRAY)/array-str.o \
    arg-parse.o


name_from_path = $(subst -,_,$1)

$(libbase_headers): $(LIBBASE)/%.h: $(LIBBASE)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libbase_sources): $(LIBBASE)/%.c: $(LIBBASE)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libbase_objects): $(LIBBASE)/%.o: $(LIBBASE)/%.h

$(libmaybe_defs): $(LIBMAYBE)/def/maybe-%.h: $(LIBMAYBE)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" -o $@

$(libarray_defs): $(LIBARRAY)/def/array-%.h: $(LIBARRAY)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers $($(n)_func_header) -o $@

$(libarray_headers): $(LIBARRAY)/array-%.h: $(LIBARRAY)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libarray_sources): $(LIBARRAY)/array-%.c: $(LIBARRAY)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libarray_objects): $(LIBARRAY)/array-%.o: \
    $(LIBARRAY)/array-%.h \
    $(LIBARRAY)/def/array-%.h \
    $(LIBBASE)/size.h \
    $(LIBMAYBE)/def/maybe-size.h



-include $(mkdeps)

