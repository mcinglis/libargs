
**Libargs** provides an `arg_parse()` function for parsing arguments according to a given specification:

``` c
ArgsError err = { .type = ArgsError_NONE };
arg_parse( argc, argv, &err, ( ArgSpec ){
    .positionals = ARRAY_ARGPOSITIONAL(
        { .name        = "system-id",
          .destination = &system_id
        },
        { .name        = "api-key",
          .destination = &api_key
        }
    ),
    .flags = ARRAY_ARGFLAG(
        { .long_name   = "version",
          .destination = &got_version
        }
    ),
    .options = ARRAY_ARGOPTION(
        { .long_name   = "widget-file",
          .destination = &widget_file
        },
        { .short_name  = "t",
          .long_name   = "time-range",
          .destination = &time_range,
          .parser      = arg_parse_intmax
        }
    )
} );
if ( err.type != ArgsError_NONE ) {
    // handle error
}
// use values set from `arg_parse`...
```

See [`examples/demo.c`](examples/demo.c) for a full example.


## Releases

I'll tag the releases according to [semantic versioning](http://semver.org/spec/v2.0.0.html). All the macros preceded by `// @public` are considered public: they'll only change between major versions. The other macros could change any time. Non-preprocessor identifiers defined in header files are always considered public. New identifiers prefixed with `maybe` (any case) will not warrant a major version bump.

Every version tag will be signed with [my GPG key](http://pool.sks-keyservers.net/pks/lookup?op=vindex&search=0xD020F814) (fingerprint: `0xD020F814`).


## Dependencies

`Package.json` specifies the dependencies of Libargs: where to get them, and what version to use. I've developed a tool called [Puck](https://gitorious.org/mcinglis/puck) that will parse such a `Package.json`, download the specified repositories, check out the specified version, and, if the dependency has its own `Package.json`, repeat that process for *its* dependencies. With `puck` on your PATH, in the directory of Libargs:

``` sh
$ puck update
$ puck execute build
```

There's nothing magic to what Puck does, so if you would prefer, you can set up the dependencies manually. You just need to have the dependencies in the `deps` directory within the Libargs directory, and have them built (if necessary) before building Libargs.


## Collaboration

Libargs is available at [Gitorious](https://gitorious.org/mcinglis/libmaybe), [Bitbucket](https://bitbucket.org/mcinglis/libmaybe), and [GitHub](https://github.com/mcinglis/libmaybe).

Questions, discussion, bug reports and feature requests are welcome at [the GitHub issue tracker](https://github.com/mcinglis/libmaybe/issues), or via [emails](mailto:me@minglis.id.au).

To contribute changes, you're welcome to [email me](mailto:me@minglis.id.au) patches as per `git format-patch`, or to send me a pull request on any of the aforementioned sites. You're also welcome to just send me a link to your remote repository, and I'll merge stuff from that as I want to.

To accept notable contributions, I'll require you to assign your copyright to me. In your email/pull request and commit messages, please insert: "*I hereby irrevocably transfer to Malcolm Inglis (http://minglis.id.au) all copyrights, title, and interest, throughout the world, in these contributions to Libargs*". If you can, please sign the email or pull request, ensuring your GPG key is publicly available.


## License

**Copyright 2015 Malcolm Inglis <http://minglis.id.au>**

Libargs is free software: you can redistribute it and/or modify it under the terms of the [GNU Affero General Public License](https://gnu.org/licenses/agpl.html) as published by the [Free Software Foundation](https://fsf.org), either version 3 of the License, or (at your option) any later version.

Libargs is distributed in the hope that it will be useful, but **without any warranty**; without even the implied warranty of **merchantability** or **fitness for a particular purpose**. See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with Libargs If not, see <https://gnu.org/licenses/>.

[Contact me](mailto:me@minglis.id.au) for proprietary licensing options.

### Why AGPL?

[I believe that nonfree software is harmful](http://minglis.id.au/blog/2014/04/09/free-software-free-society.html), and I don't want to contribute to its development at all. I believe that a free society must necessarily operate on free software. I want to encourage the development of free software, and discourage the development of nonfree software.

The [GPL](https://gnu.org/licenses/gpl.html) was designed to ensure that the software stays free software; "to ensure that every user has freedom". The GPL's protections may have sufficed in 1990, but they don't in 2014. The GPL doesn't consider users of a web service to be users of the software implementing that server. Thankfully, the AGPL does.

The AGPL ensures that if Libargs is used to implement a web service, then the entire source code of that web service must be free software. This way, I'm not contributing to nonfree software, whether it's executed locally or provided over a network.

