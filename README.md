# advent-of-code-d-template

A template for Advent of Code solutions written in D.

## Notes

Unicode output is janky on Windows; enable Unicode UTF-8 support in `intl.cpl`'s  Administrative | Change system locale...

## Dependencies

- A D compiler.
  Check [the wiki](https://wiki.dlang.org/Compilers) for available options.
- Dub, the language's package manager.
  Should be included with your compiler installation.

## Included libraries

I have added two libraries to the dependencies in [`dub.json`](dub.json):
- [`emsi_containers`](https://code.dlang.org/packages/emsi_containers)
- [`optional`](https://code.dlang.org/packages/optional)

