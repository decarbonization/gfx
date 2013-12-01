gfx
===

Graphics is a small forth-style language centered around manipulation of a graphics state machine created with the intention of creating a simple teaching environment for programming on the iPad.

Target
======

Gfx current targets OS X, and is intended to be cross platform with iOS. Supports for other platforms may be added at a future time, currently gfx is tied to CoreFoundation and Core Graphics.

The Stack
=========

Being a forth-style language, the stack is a central component to programming with Gfx. Every time a literal is introduced in source code, it is pushed onto the stack. E.g. `12` pushes the value of the number `12` onto the stack. Functions may then remove values from the stack and operate on them.

Gfx expands on the simple stack in a couple of ways. Gfx stacks are hierarchical. A stack may have a parent stack, which may have a parent stack, which... This allows Gfx functions to have their on stack, and still pull values off of the stack that was used to call the function. Gfx stacks also have a concept of variable bindings. Each stack has its own lookup hash that is used to store said bindings.

Literals
========

Gfx has a bare-minimum set of literals. The available literals are as follows:

##Numbers

Numbers may be of the format `12`, or `3.50`. Internally, they are always represented as double precision floating point numbers. Underscores may be introduced to improve readability, e.g. `10_000`. Numbers may not begin with underscores, but otherwise there are no positional restrictions on where they may be placed.

##Strings

String literals are enclosed in double quotes, like `"hello world"`. The following escape sequences are available:

* `\a`
* `\b`
* `\f`
* `\n`
* `\r`
* `\t`
* `\v`
* `\'`
* `\"`
* `\\`
* `\?`
* `\%`

String literals currently do not implement interpolation. In the future, interpolation will be implemented using the form "hello %(greeting)". As such, the % as a literal within a string is reserved, and should be prefixed with a backslash.

##Vectors

Vector literals are enclosed within square brackets, like `[12 15 18]`. Unlike in bare code, literals within a literal are placed within the vector, and are not pushed to the stack. This means that code such as `[12 12 +]` will not yield `[24]`, but will likely crash due to a stack underflow caused by the `+` function.

##Functions

Function literals are enclosed in curly braces, like `{"hello world" print}`. Function literals are given their own stack frame when evaluated, and honor a simple form of lexical scoping for variables. To evaluate a function literal immediately after you have created it, use the `apply` word.

##Comments

Comment literals are enclosed in parentheses, like `(this is ignored)`, and are otherwise ignored by the language under normal circumstances. It is customary to describe how a function will manipulate the stack using comments of the form `(before -- after)`. E.g. `(num num -- num)` could describe all of the basic math operations. Comments of the form `(%  %)` are reserved for machine readable information. These comments may be used to add metadata to files, such as documentation, or structural information for an editor. E.g.

	(%
		\function	add-three (num num num -- num)
		
		\param		num	The first number.
		\param		num	The second number.
		\param		num	The third num.
		
		\result		The sum of all three numbers given.
	 %)

##Booleans

Boolean literals come in the form of two reserved words, `true` and `false`. They always evaluate to themselves.

##Words

Word literals are any sequence of characters that do not correspond to any of the other literals. Word literals are normally interpreted to mean apply a function by name, or lookup a variable by name. E.g. `rt/showstack` will print the stack, and `math/PI` will push `3.14...` onto the stack. If a literal word is needed, it may be prefixed with a tick, like `'hello`. Words beginning with `#` and `&` are currently reserved for the Gfx language for future usage.

##Other Types

All other types besides the ones described above are provided through functions, and live outside of the Gfx parser.

Types and Functions
===================

##Number (num)

Numbers are currently represented as double precision floating point numbers. This may change at a future date. The currently supported words are:

* `+ ( num num -- num )` – adds two numbers and yields the resultant number
* `- ( num num -- num )` – subtracts one number from a second and yields the resultant number
* `* ( num num -- num )` – multiplies two numbers and yields the resultant number
* `/ ( num num -- num )` – divides one number from a second and yields the resultant number
* `^ ( num num -- num )` – raises one number by a second and yields the resultant number
* `num/min ( num num -- num )` – yields the lesser of two numbers
* `num/max ( num num -- num )` – yields the greater of two numbers

###The following mathematical functions are exposed for numbers:

* `math/cos ( num -- num )`
* `math/sin ( num -- num )`
* `math/tan ( num -- num )`
* `math/acos ( num -- num )`
* `math/asin ( num -- num )`
* `math/atan ( num -- num )`
* `math/atan2 ( num num -- num )`
* `math/cosh ( num -- num )`
* `math/sinh ( num -- num )`
* `math/tanh ( num -- num )`
* `math/acosh ( num -- num )`
* `math/asinh ( num -- num )`
* `math/atanh ( num -- num )`
* `math/exp ( num -- num )`
* `math/log ( num -- num )`
* `math/log10 ( num -- num )`
* `math/sqrt ( num -- num )`
* `math/cbrt ( num -- num )`
* `math/hypot ( num num -- num )`
* `math/ceil ( num -- num )`
* `math/floor ( num -- num )`
* `math/round ( num -- num )`
* `math/abs ( num -- num )`

###The following mathematical constants are exposed:

* `math/E`
* `math/LOG2E`
* `math/LOG10E`
* `math/LN2`
* `math/LN10`
* `math/PI`
* `math/PI2`
* `math/1PI`
* `math/2PI`
* `math/2SQRTPI`
* `math/SQRT2`
* `math/SQRT12`

##Booleans (bool)

Gfx does not have a separate type for bools, but represents them as `1` for `true`, and `0` for `false`. Only the number `0` is considered falsey. The following functions are exposed to operate on boolean values.

* `& ( bool bool -- bool )` – yields true if two bools are true; false otherwise.
* `| ( bool bool -- bool )` – yields true if one of two bools are true; false otherwise.
* `! ( bool -- bool )` – negates a boolean value.

##null

Gfx does not have a separate type for `null`. `null` is represented as `0`.

##Identity Functions

Gfx includes the standard six identity operators, exposed as the following functions:

* `= ( val val -- bool )` – yields true if two values are equal; false otherwise.
* `!= ( val val -- bool )` – yields true if two values are not equal; false otherwise.

The following identity functions currently only operate on numbers:

* `< ( num num -- bool )` – yields true if num1 is less than num2; false otherwise.
* `<= ( num num -- bool )` – yields true if num1 is less than or equal to num2; false otherwise.
* `> ( num num -- bool )` – yields true if num1 is greater than num2; false otherwise.
* `>= ( num num -- bool )` – yields true if num1 is greater than or equal to num2; false otherwise.

##Strings (str)

Gfx strings are UTF16 character arrays. Strings literals are enclosed within quotes. E.g. `"hello world"`. Strings are readonly. The following functions are available for operating on strings:

* `str/eq ( str str -- bool )` – yields true if two strings are equal; false otherwise.
* `str/compare ( str str -- num )` – performs a simple case-sensitive comparison on two strings. yields the standard comparison result values: `-1`, `0`, and `1`.
* `str/contains ( str str -- bool )` – yields true if the second string is contained in the first; false otherwise.
* `str/starts-with ( str str -- bool )` – yields true if the first string starts with the second string; false otherwise.
* `str/ends-with ( str str -- bool )` – yields true if the first string ends with the second string; false otherwise.
* `str/char-at ( str num -- num )` – yields the character value at the number-index given.
* `str/index-of ( str str -- num )` – yields the offset of the second string in the first.
* `str/concat ( str str -- str )` – yields the combination of two strings.
* `str/replace ( str str2 str3 -- str )` – replaces occurances of str2 with str3 in str.
* `str/substr ( str num num -- str )` – yields a subset of str.
* `str/split ( str str -- vec )` – splits a str using the second str, yielding a vector.
* `str/lower-case ( str -- str )` – converts a string to upper case.
* `str/upper-case ( str -- str )` – converts a string to lower case.
* `str/capital-case ( str -- str )` – converts a string to capital case.

##Vectors (vec)

Gfx vectors are immutable sequences of values. Vector literals are enclosed in brackets. E.g. `[1 2 3 4]`. __Important:__ functions are not evaluated within vector literals. So `[+]` would yield a vector that contains the `+` function, and not the sum of whatever is on the stack.

The following functions are available for vectors:

* `vec/at ( vec num -- val )` – yields the value at the `0`-start offset indicated by num in the vector.
* `vec/concat ( vec vec -- vec )` – merges the contents of two vectors into a new vector, yielding that.
* `vec/index-of ( vec val -- num )` – yields the offset of a value within the vector.
* `vec/last-index-of ( vec val -- num )` – yields the last known offset of a value within the vector.
* `vec/join ( vec str -- str )` – yields a new str by joining the str values of every item in the vector, separating them by a given str.
* `vec/subset ( vec num num -- vec )` – yields a subset of the vector.
* `vec/sort ( vec func -- vec )` – yields a sorted copy of the vector by applying a sort function to each element.
* `vec/for-each ( vec func -- )` – applies a function to each item in a vector
* `vec/filter ( vec func -- vec )` – yields a new vector by applying a function to each item in the vector, and building a new vector from the values which the function yielded `true` for.
* `vec/map ( vec func -- vec )` – yields a new vector by applying a function to each item in the vector, and collecting those values into the new vector.

##Hashes (hash)

Gfx hashes are unordered maps of keys and values. There are currently no literal representations of hashes. Hashes are created using the `hash/begin` and `hash/end` functions. Example usage:

	hash/begin
		"one"	1
		"two"	2
		"three"	3
	hash/end

The following functions are available for hashes:

* `hash/begin ( -- HashThunk )` – yields a thunk onto the stack. See `hash/end`.
* `hash/end ( HashThunk ... - Hash)` – yields a new hash from all of the values on the stack going back to the thunk pushed on by `hash/begin`. You must have an even number of values on the stack, or an error is raised.
* `hash/get ( val -- val )` – yields the value contained in the hash by the given name, or `null` if no such value exists.
* `hash/concat ( hash hash -- hash )` – yields a new hash by merging two existing hashes.
* `hash/for-each ( hash functor -- )` – applies a given functor to each key-value pair contained in the hash.

##Files (file)

Gfx provides a simple file type that can be used for both simple console IO, and for basic string-based file IO. It is currently only possible to open a file readonly, or writeonly. The following functions are available for files:

* `file/open ( str -- file )` – yields a new file for reading and writing at a given path.
* `file/close ( file -- )` – closes a file. __Important:__ Gfx will only automatically close files if they are popped off the stack.
* `file/size ( file -- num )` – yields the total number of bytes contained in the file.
* `file/seek ( file num -- )` – moves the pointer of a file to a given offset.
* `file/tell ( file -- num )` – yields the current pointer offset of a file.
* `file/read ( file num -- str)` – reads a specified number of bytes into a str, yielding it.
* `file/read-line ( file -- str )` – reads a line from a file, yielding it.
* `file/write ( file str -- num )` – writes a str to a file, yielding the number of bytes written.
* `file/write-line ( file str -- num )` – writes a str followed by a newline to a file, yielding the number of bytes written.

__Important:__ Files may be disabled in gfx when the interpreter is built.

##Core Functions

Gfx comes with a small set of core functions that emulate various syntax level features of other languages.

* `import ( str -- bool )` – import the contents of a file at a given path into the current scope, executing the instructions contained within. __Important:__ Import can be disabled at compile time, as such this function may not be available.
* `print ( val -- )` – prints a value to the papertape. The papertape is not guaranteed to take any particular visual form, and may be entirely absent.
* `read ( -- str )` – reads a line of data from the paper tape into a new str, yielding it. The papertape is not guaranteed to take any particular visual form, and may be entirely absent.
* `if ( bool functor -- )` – applies the functor if the bool is true.
* `ifelse ( bool functor1 functor2 -- )` – applies functor1 if bool is true, functor2 otherwise.
* `while ( functor1 functor2 -- )` – applies functor2 while functor1 yields a truthy value.
* `times ( functor num -- )` – applies the `functor` `num` times.
* `fn/apply ( functor -- val )` – applies the functor, yielding its return value. Calls to this function are normally implicit.
* `throw ( str -- )` – raises an error using the given string.
* `rescue ( functor1 functor2 -- val? )` – catches any issues raised in functor1, applying functor2 if any are caught. In `functor2`, the issue may be accessed through the name `__exception`.
* `let ( val word|[word] -- )` – binds `val` to the `word`. E.g. `12 'twelve let` or `1 2 3 ['one 'two 'three] let`. Used to introduce variables or create new named functions.
* `set! ( val word -- )` – updates the binding referred to by `word` to `val`. Creates a new binding if `word` does not already exist. E.g. `13 'twelve set!`. Always updates the top most variable with the name `word`.
* `destruct! ( vec -- ... )` – pushes each value contained in `vec` onto the stack. E.g. `[1 2 3] destruct ['one 'two 'three] let`.

##Stack Functions

Gfx exposes a small set of stack functions to allow manipulating the stack directly. These functions are:

* `rt/dup ( val -- val val )` – duplicates the top most value on the stack.
* `rt/swap ( a b -- b a )` – swaps two values on the stack.
* `rt/drop ( val -- )` – removes a value from the stack.
* `rt/clear ( ... -- )` – removes all values from the stack.
* `rt/showstack ( -- )` – prints the contents of the stack to the papertape.
* `rt/backtrace ( -- )` – prints the current function backtrace to the papertape.

##Type Conversion Functions

Gfx exposes a small set of functions to convert between function types. These functions are:

* `->str ( val -- str )` – converts the top most value on the stack to a str, yielding it.
* `->void ( val -- )` – converts the top most value on the stack to nothing. Synonym for `rt/drop`

Command Line Tool
=================

Gfx is currently distributed as a tiny command line tool for OS X. The tool contains a rudimentary REPL, and is able to run files.

	Usage: gfx [--to-file /path/to/output.png] [--of-size 100x100] [<path...>]
	
##Parameters

* `--to-file <path>`: Specifies an output location for any graphics described while the command line tool was running. This parameter is available both for files and the REPL.
* `--of-size <size>`: A string of the format N**x**N specifying the size of the canvas that will be created before any code is run. The default value is 500x500. This parameter is available both for files and the REPL.
* `<path...>`: Any number of paths may be specified. They are run in the order that they are specified in the tool's arguments. If no files are specified, the REPL is started.

Graphics
========

The graphics portion of Gfx is currently underdeveloped. This section will be filled out when the API becomes stable.

License
=======

The MIT License (MIT)

Copyright (c) 2013 Kevin MacWhinnie

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
