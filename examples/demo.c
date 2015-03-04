
#include <stdio.h>

#include <libbase/bool.h>
#include <libbase/int.h>

#include "../arg-parse.h"


#define MAX_WIDGETS 5

size_t num_widgets = 0;


void
parse_widgets( char const * const name,
               char const * const arg,
               void * const vwidgets )
{
    int * const widgets = vwidgets;
    widgets[ num_widgets ] = int__from_str( arg );
    num_widgets++;
}


int
main( int const argc,
      char const * const * const argv )
{
    ArgsError err = { .type = ArgsError_NONE };
    char const * something = "test";
    bool * foo = false;
    bool * bar = true;
    char const * bazqux = NULL;
    int widgets[ MAX_WIDGETS ] = { 0 };
    arg_parse( argc, argv, &err, ( ArgSpec ){
        .positionals = ARRAY_ARGPOSITIONAL(
            { .name = "something",
              .destination = &something }
        ),
        .flags = ARRAY_ARGFLAG(
            { .short_name = "f",
              .long_name = "foo",
              .destination = &foo
            },
            { .long_name = "bar",
              .destination = &bar,
              .parser = arg_set_false
            }
        ),
        .options = ARRAY_ARGOPTION(
            { .short_name  = "b",
              .long_name   = "bazqux",
              .destination = &bazqux
            },
            { .short_name  = "w",
              .long_name   = "widgets",
              .parser      = parse_widgets,
              .destination = widgets,
              .num_args    = { .min = 2, .max = MAX_WIDGETS }
            }
        )
    } );
    if ( err.type == ArgsError_NONE ) {
        printf( "something = %s\n", something );
        printf( "foo = %s\n", str__from_bool( foo ) );
        printf( "bar = %s\n", str__from_bool( bar ) );
        printf( "bazqux = %s\n", bazqux );
        printf( "widgets = %d %d %d %d %d\n",
                widgets[ 0 ], widgets[ 1 ], widgets[ 2 ],
                widgets[ 3 ], widgets[ 4 ] );
    } else {
        printf( "ERROR! %s (str=%s)\n",
                str__from_argserrortype( err.type ), err.str );
    }
}

