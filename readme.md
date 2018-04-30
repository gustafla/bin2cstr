# bin2cstr by Lauri "luutifa" Gustafsson

This small tool was made for embedding data in self-compiling executables.

The program converts a file by filename from command line into a C string
literal to stdout.

Use shell redirection or concatenation to save the results.

The -p command line flag can be used to try to output printable characters
as such, but keeping to just hex escape sequences may compress better.

## Installation

    cc -Os main.c -o <install dir>/bin2cstr

I recommend installing to ~/.local/bin or similar
