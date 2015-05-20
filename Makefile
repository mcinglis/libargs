
##############################
### VARIABLES
##############################

DEPS_DIR ?= deps

LIBBASE  := $(DEPS_DIR)/libbase
LIBSTR   := $(DEPS_DIR)/libstr
LIBMAYBE := $(DEPS_DIR)/libmaybe
LIBARRAY := $(DEPS_DIR)/libarray

CPPFLAGS += -I. -I$(DEPS_DIR)

CFLAGS ?= -std=c11 -g \
          -Wall -Wextra -pedantic \
          -Wcomments -Wformat=2 -Wlogical-op -Wmissing-include-dirs \
          -Wnested-externs -Wold-style-definition -Wredundant-decls \
          -Wshadow -Wstrict-prototypes -Wunused-macros -Wvla -Wwrite-strings \
          -Wno-type-limits -Wno-unused-parameter

CFLAGS ?= $(cflags_std) -g $(cflags_warnings)

TPLRENDER ?= $(DEPS_DIR)/tplrender/tplrender


name_from_path = $(subst -,,$1)

libarray_types := str arg-positional arg-flag arg-option
libmaybe_types := $(libarray_types) size
libbase_types  := $(libmaybe_types) bool int

size_type        := size_t
size_options     := --typeclasses NULL BOUNDED EQ ORD ENUM NUM FROM_STR TO_STRM \
                    --extra num_type=unsigned
size_func_header := libbase/size.h

bool_type        := bool
bool_options     := --typeclasses NULL BOUNDED EQ ORD ENUM FROM_STR TO_STR \
                    --extra min_bound=false max_bound=true
bool_func_header := libbase/bool.h

int_type        := int
int_options     := --typeclasses NULL BOUNDED EQ ORD ENUM NUM FROM_STR TO_STRM \
                   --extra num_type=signed
int_func_header := libbase/int.h

str_type        := char const *
str_options     := --typename str --typeclasses NULL EQ ORD
str_func_header := libstr/str.h

argpositional_type       := ArgPositional
argpositional_def_header := def/arg-positional.h

argflag_type       := ArgFlag
argflag_def_header := def/arg-flag.h

argoption_type       := ArgOption
argoption_def_header := def/arg-option.h


libbase_sources := $(foreach t,$(libbase_types),$(LIBBASE)/$t.c)
libbase_headers := $(libbase_sources:.c=.h)
libbase_objects := $(libbase_sources:.c=.o)

libmaybe_sources := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/maybe_$t.c)
libmaybe_headers := $(libmaybe_sources:.c=.h)
libmaybe_defs    := $(foreach t,$(libmaybe_types),$(LIBMAYBE)/def/maybe_$t.h)
libmaybe_objects := $(libmaybe_sources:.c=.o)

libarray_sources := $(foreach t,$(libarray_types),$(LIBARRAY)/array_$t.c)
libarray_headers := $(libarray_sources:.c=.h)
libarray_defs    := $(foreach t,$(libarray_types),$(LIBARRAY)/def/array_$t.h)
libarray_objects := $(libarray_sources:.c=.o)

gen_objects := \
    $(libbase_objects) \
    $(libmaybe_objects) \
    $(libarray_objects)

gen := \
    $(libbase_sources) \
    $(libbase_headers) \
    $(libmaybe_defs) \
    $(libmaybe_sources) \
    $(libmaybe_headers) \
    $(libarray_sources) \
    $(libarray_headers) \
    $(libarray_defs) \
    $(gen_objects)

sources  := $(wildcard *.c)
objects  := $(sources:.c=.o)
mkdeps   := $(objects:.o=.dep.mk) $(gen_objects:.o=.dep.mk)

examples := $(basename $(wildcard examples/*.c))


##############################
### BUILDING
##############################

.PHONY: all
all: $(examples)

.PHONY: clean
clean:
	rm -rf $(objects) $(mkdeps) $(examples) $(gen)


%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -MMD -MF "$(@:.o=.dep.mk)" -c $< -o $@


def/args-spec.h: \
    $(LIBARRAY)/def/array_arg-positional.h \
    $(LIBARRAY)/def/array_arg-flag.h \
    $(LIBARRAY)/def/array_arg-option.h

argparse.o: \
    def/args-spec.h \
    $(LIBARRAY)/def/array_str.h \
    $(LIBARRAY)/array_str.h

examples/demo: \
    $(LIBBASE)/bool.o \
    $(LIBBASE)/int.o \
    $(LIBBASE)/size.o \
    $(LIBSTR)/str.o \
    $(LIBMAYBE)/maybe_str.o \
    $(LIBARRAY)/array_str.o \
    argparse.o


$(libbase_headers): $(LIBBASE)/%.h: $(LIBBASE)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libbase_sources): $(LIBBASE)/%.c: $(LIBBASE)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libbase_objects): $(LIBBASE)/%.o: $(LIBBASE)/%.h

$(libmaybe_defs): $(LIBMAYBE)/def/maybe_%.h: $(LIBMAYBE)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libmaybe_headers): $(LIBMAYBE)/maybe_%.h: $(LIBMAYBE)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libmaybe_sources): $(LIBMAYBE)/maybe_%.c: $(LIBMAYBE)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers "libbase/$*.h" -o $@

$(libmaybe_objects): $(LIBMAYBE)/maybe_%.o: \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h \
    $(LIBBASE)/%.h

$(libarray_defs): $(LIBARRAY)/def/array_%.h: $(LIBARRAY)/def.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers $($(n)_def_header) -o $@

$(libarray_headers): $(LIBARRAY)/array_%.h: $(LIBARRAY)/header.h.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) -o $@

$(libarray_sources): $(LIBARRAY)/array_%.c: $(LIBARRAY)/source.c.jinja
	$(eval n := $(call name_from_path,$*))
	$(TPLRENDER) $< "$($(n)_type)" $($(n)_options) --sys-headers $($(n)_func_header) -o $@

$(libarray_objects): $(LIBARRAY)/array_%.o: \
    $($(*)_func_header) \
    $(LIBBASE)/size.h \
    $(LIBMAYBE)/def/maybe_size.h \
    $(LIBMAYBE)/def/maybe_%.h \
    $(LIBMAYBE)/maybe_%.h \
    $(LIBARRAY)/def/array_%.h \
    $(LIBARRAY)/array_%.h



-include $(mkdeps)

