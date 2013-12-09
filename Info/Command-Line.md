Summary
=======

The simple host for the Gfx stack is a tiny command line tool. The tool contains a rudimentary REPL, and is able to run an arbitrary number of files. All graphical output is in the form of `png` files.

	Usage: gfx [--to-file /path/to/output.png] [--of-size 100x100] [<path...>]
	
##Parameters

* `--to-file <path>`: Specifies an output location for any graphics described while the command line tool was running. This parameter is available both for files and the REPL.
* `--of-size <size>`: A string of the format N**x**N specifying the size of the canvas that will be created before any code is run. The default value is 500x500. This parameter is available both for files and the REPL.
* `<path...>`: Any number of paths may be specified. They are run in the order that they are specified in the tool's arguments. If no files are specified, the REPL is started.
