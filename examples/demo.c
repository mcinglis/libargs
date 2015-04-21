
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <libbase/bool.h>
#include <libbase/int.h>

#include <argparse.h>


#define MAX_WIDGETS 4

// This approach will break if the `--widgets` argument is given twice; to
// solve this, you will need a dedicated array / vector data type (which you
// would want to have in any realistic application anyway).
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
    int widgets[ MAX_WIDGETS ] = { 9999, 9999, 9999, 9999 };
    bool got_help_flag = false;
    argparse( argc, argv, &err, ( ArgsSpec ){
        .positionals = ARRAY_ARGPOSITIONAL(
            { .name = "something",
              .destination = &something }
        ),
        .flags = ARRAY_ARGFLAG(
            { .name        = "--foo",
              .destination = &foo
            },
            { .name        = "--bar",
              .destination = &bar,
              .parser      = arg_set_false
            },
            { .name        = "--help",
              .destination = &got_help_flag
            }
        ),
        .options = ARRAY_ARGOPTION(
            { .name        = "--bazqux",
              .destination = &bazqux
            },
            { .name        = "--widgets",
              .parser      = parse_widgets,
              .destination = widgets,
              .num_args    = { .min = 2, .max = 4 }
            }
        )
    } );
    if ( got_help_flag ) {
        printf( "%s <something>\n"
                "        [--help] [--foo] [--bar]\n"
                "        [--bazqux <str>]\n"
                "        [--widgets <n> <n> [<n> [<n>]]\n",
                argv[ 0 ] );
    } else if ( err.type == ArgsError_NONE ) {
        printf( "something = %s\n", something );
        printf( "foo       = %s\n", bool__to_str( foo ) );
        printf( "bar       = %s\n", bool__to_str( bar ) );
        printf( "bazqux    = %s\n", bazqux ? bazqux : "(null)" );
        printf( "widgets   = %d %d %d %d\n",
                widgets[ 0 ], widgets[ 1 ], widgets[ 2 ], widgets[ 3 ] );
    } else {
        printf( "ERROR: %s for `%s`", argserrortype__to_str( err.type ), err.str );
        if ( errno ) {
            printf( " (errno=%d: %s)", errno, strerror( errno ) );
        }
        printf( "\nPass `--help` to see usage.\n" );
    }
}

