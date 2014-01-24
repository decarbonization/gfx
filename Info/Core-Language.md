Forward
=======

This document describes the core langauge of the Gfx stack. The language is in the forth family. Gfx programs/documents are built around manipulating the stack, shared global state, simple functions.

The Stack
=========

The stack is a core part of programming with Gfx. Every time a literal is introduced in source code, it is pushed onto the stack. For example, `12` pushes the value of the number `12` onto the stack. Functions may then remove values from the stack and operate on them.

Gfx expands on the simple stack in a couple of ways. Gfx stacks are hierarchical. A stack may have a parent stack, which may have a parent stack, which... This allows Gfx functions to have their on stack, and still pull values off of the stack that was used to call the function. Gfx stacks also have a concept of variable bindings. Each stack has its own lookup hash that is used to store said bindings.

Literals
========

Gfx has a bare-minimum set of literals. The available literals are as follows:

##Numbers

Numbers may be of the format `12`, or `3.50`. Internally, they are always represented as double precision floating point numbers. Underscores may be introduced to improve readability, e.g. `10_000`. Numbers may not begin with underscores, but otherwise there are no positional restrictions on where they may be placed. If a number is immediately followed by a percentage sign (`%`), the value of the number will be divided by 100. E.g. `50%` = `0.5`.

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

Comment literals are enclosed in parentheses with asterisks on both sides, like `(* this is ignored *)`, and are otherwise ignored by the language under normal circumstances. It is customary to describe how a function will manipulate the stack using comments of the form `(* before -- after *)`. E.g. `(* num num -- num *)` could describe all of the basic math operations. Comments may contain nested parentheses as long as they are balanced.

##Annotations

Annotations are a comment-like syntactical construct that enables non-gfx-code machine readable information to be embedded. Annotations may be used to add metadata to files, such as documentation, or structural information for an editor. E.g.

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

Word literals are any sequence of characters that do not correspond to any of the other literals. Word literals are normally interpreted to mean apply a function by name, or lookup a variable by name. E.g. `__showstack` will print the stack, and `math/PI` will push `3.14...` onto the stack. If a literal word is needed, it may be prefixed with a colon, like `:hello`.

Words beginning with `&` bypass function application. This allows passing existing functions into other functions, e.g. `[1 2 3] &print vec/for-each` will print 1, 2, and 3.

Words beginning with `#` are currently reserved for the Gfx language for future usage.

The Gfx parser has a special case for words that end with parentheses. It will rewrite expressions of the form `str/concat("hello " "world")` into the form `"hello " "world" str/concat`. This allows prefix notation to be used whenever it is more natural. __Note:__ This syntax is currently considered experimental, and may be updated in the near future.

##Other Types

All other types besides the ones described above are provided through functions, and live outside of the Gfx parser.

Types and Functions
===================

##Number (num)

Numbers are currently represented as double precision floating point numbers. This may change at a future date. The currently supported words are:

* `+ ( num num -- num )`: adds two numbers and yields the resultant number
* `- ( num num -- num )`: subtracts one number from a second and yields the resultant number
* `* ( num num -- num )`: multiplies two numbers and yields the resultant number
* `/ ( num num -- num )`: divides one number from a second and yields the resultant number
* `^ ( num num -- num )`: raises one number by a second and yields the resultant number
* `num/min ( num num -- num )`: yields the lesser of two numbers
* `num/max ( num num -- num )`: yields the greater of two numbers

The type for numbers is `<num>`.

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

* `and ( bool bool -- bool )`: yields true if two bools are true; false otherwise.
* `or ( bool bool -- bool )`: yields true if one of two bools are true; false otherwise.
* `not ( bool -- bool )`: negates a boolean value.

The type for bools is `<num>`.

##null

Gfx does not have a separate type for `null`. `null` is represented as `0`.

The type for null is `<num>`.

##Identity Functions

Gfx includes the standard six identity operators, exposed as the following functions:

* `= ( val val -- bool )`: yields true if two values are equal; false otherwise.
* `!= ( val val -- bool )`: yields true if two values are not equal; false otherwise.

The following identity functions currently only operate on numbers:

* `< ( num num -- bool )`: yields true if num1 is less than num2; false otherwise.
* `<= ( num num -- bool )`: yields true if num1 is less than or equal to num2; false otherwise.
* `> ( num num -- bool )`: yields true if num1 is greater than num2; false otherwise.
* `>= ( num num -- bool )`: yields true if num1 is greater than or equal to num2; false otherwise.

##Strings (str)

Gfx strings are UTF16 character arrays. Strings literals are enclosed within quotes. E.g. `"hello world"`. Strings are readonly. The following functions are available for operating on strings:

* `str/eq ( str str -- bool )`: yields true if two strings are equal; false otherwise.
* `str/compare ( str str -- num )`: performs a simple case-sensitive comparison on two strings. yields the standard comparison result values: `-1`, `0`, and `1`.
* `str/contains ( str str -- bool )`: yields true if the second string is contained in the first; false otherwise.
* `str/starts-with ( str str -- bool )`: yields true if the first string starts with the second string; false otherwise.
* `str/ends-with ( str str -- bool )`: yields true if the first string ends with the second string; false otherwise.
* `str/char-at ( str num -- num )`: yields the character value at the number-index given.
* `str/index-of ( str str -- num )`: yields the offset of the second string in the first.
* `str/concat ( str str -- str )`: yields the combination of two strings.
* `str/replace ( str str2 str3 -- str )`: replaces occurances of str2 with str3 in str.
* `str/substr ( str num num -- str )`: yields a subset of str.
* `str/split ( str str -- vec )`: splits a str using the second str, yielding a vector.
* `str/lower-case ( str -- str )`: converts a string to upper case.
* `str/upper-case ( str -- str )`: converts a string to lower case.
* `str/capital-case ( str -- str )`: converts a string to capital case.

The type for strings is `<str>`.

##Vectors (vec)

Gfx vectors are immutable sequences of values. Vector literals are enclosed in brackets. E.g. `[1 2 3 4]`. __Important:__ functions are not evaluated within vector literals. So `[+]` would yield a vector that contains the `+` function, and not the sum of whatever is on the stack.

The following functions are available for vectors:

* `vec/at ( vec num -- val )`: yields the value at the `0`-start offset indicated by num in the vector.
* `vec/concat ( vec vec -- vec )`: merges the contents of two vectors into a new vector, yielding that.
* `vec/index-of ( vec val -- num )`: yields the offset of a value within the vector.
* `vec/last-index-of ( vec val -- num )`: yields the last known offset of a value within the vector.
* `vec/join ( vec str -- str )`: yields a new str by joining the str values of every item in the vector, separating them by a given str.
* `vec/subset ( vec num num -- vec )`: yields a subset of the vector.
* `vec/sort ( vec func -- vec )`: yields a sorted copy of the vector by applying a sort function to each element.
* `vec/for-each ( vec func -- )`: applies a function to each item in a vector
* `vec/filter ( vec func -- vec )`: yields a new vector by applying a function to each item in the vector, and building a new vector from the values which the function yielded `true` for.
* `vec/map ( vec func -- vec )`: yields a new vector by applying a function to each item in the vector, and collecting those values into the new vector.

The type for vectors is `<vec>`.

##Hashes (hash)

Gfx hashes are unordered maps of keys and values. Hash literals are almost identical to Vector literals. They are enclosed in square brackets, prefixed by a number sign. __Important:__ functions are not evaluated within hash literals. Example usage:

	#[
		"one"	1
		"two"	2
		"three"	3
	]

The following functions are available for hashes:

* `hash/get ( val -- val )`: yields the value contained in the hash by the given name, or `null` if no such value exists.
* `hash/concat ( hash hash -- hash )`: yields a new hash by merging two existing hashes.
* `hash/without ( hash vec|val -- hash )`: yields a new hash by removing a single key, or a vector of keys.
* `hash/for-each ( hash functor -- )`: applies a given functor to each key-value pair contained in the hash.

The type for hash is `<hash>`.

##Files (file)

Gfx provides a simple file type that can be used for both simple console IO, and for basic string-based file IO. It is currently only possible to open a file readonly, or writeonly. The following functions are available for files:

* `file/open ( str -- file )`: yields a new file for reading and writing at a given path.
* `file/close ( file -- )`: closes a file. __Important:__ Gfx will only automatically close files if they are popped off the stack.
* `file/size ( file -- num )`: yields the total number of bytes contained in the file.
* `file/seek ( file num -- )`: moves the pointer of a file to a given offset.
* `file/tell ( file -- num )`: yields the current pointer offset of a file.
* `file/read ( file num -- str)`: reads a specified number of bytes into a str, yielding it.
* `file/read-line ( file -- str )`: reads a line from a file, yielding it.
* `file/write ( file str -- num )`: writes a str to a file, yielding the number of bytes written.
* `file/write-line ( file str -- num )`: writes a str followed by a newline to a file, yielding the number of bytes written.

__Important:__ Files may be disabled in gfx when the interpreter is built.

The type for files is `<file>`.

##Core Functions

Gfx comes with a small set of core functions that emulate various syntax level features of other languages.

* `import ( str -- bool )`: import the contents of a file at a given path into the current scope, executing the instructions contained within. __Important:__ Import can be disabled at compile time, as such this function may not be available.
* `print ( val -- )`: prints a value to the papertape. The papertape is not guaranteed to take any particular visual form, and may be entirely absent.
* `read ( -- str )`: reads a line of data from the paper tape into a new str, yielding it. The papertape is not guaranteed to take any particular visual form, and may be entirely absent.
* `if ( bool functor -- )`: applies the functor if the bool is true.
* `ifelse ( bool functor1 functor2 -- )`: applies functor1 if bool is true, functor2 otherwise.
* `while ( functor1 functor2 -- )`: applies functor2 while functor1 yields a truthy value.
* `times ( functor num -- )`: applies the `functor` `num` times.
* `fn/apply ( functor -- val )`: applies the functor, yielding its return value. Calls to this function are normally implicit.
* `throw ( str -- )`: raises an error using the given string.
* `rescue ( functor1 functor2 -- val? )`: catches any issues raised in functor1, applying functor2 if any are caught. In `functor2`, the issue may be accessed through the name `__exception`.
* `=> ( val word|[word] -- )`: binds `val` to the `word`. E.g. `12 'twelve =>` or `1 2 3 ['one 'two 'three] =>`. Used to introduce variables or create new named functions. In addition to this function, it is possible to introduce variables like `12 =>twelve`. The form is `=>`_name_.
* `set! ( val word -- )`: updates the binding referred to by `word` to `val`. Creates a new binding if `word` does not already exist. E.g. `13 'twelve set!`. Always updates the top most variable with the name `word`.
* `destruct! ( vec -- ... )`: pushes each value contained in `vec` onto the stack. E.g. `[1 2 3] destruct ['one 'two 'three] let`.
* `__recurse ( -- )`: Resets the execution of the current function to the beginning. Only usable within an interpreted function. Provides a means of using constant-space tail-recursion. Using this function outside of the appropriate context will raise an exception.

##Stack Functions

Gfx exposes a small set of stack functions to allow manipulating the stack directly. These functions are:

* `__dup ( val -- val val )`: duplicates the top most value on the stack.
* `__swap ( a b -- b a )`: swaps two values on the stack.
* `__drop ( val -- )`: removes a value from the stack.
* `__clear ( ... -- )`: removes all values from the stack.
* `__showstack ( -- )`: prints the contents of the stack to the papertape.

##Type Conversion Functions

Gfx exposes a small set of functions to convert between function types. These functions are:

* `->str ( val -- str )`: converts the top most value on the stack to a str, yielding it.
* `->void ( val -- )`: converts the top most value on the stack to nothing. Synonym for `rt/drop`

##Type Introspection

Gfx has a very simple system for accessing and comparing types. For every conceptual type in the language, there is a literal type. For example, the type for a string is &lt;str&gt;, matching the function suite. This is applicable to all core and graphics types. The following functions are available for working with types:

* `is-a? ( val type -- bool )`: Takes a value, and a type, and yields a bool indicating whether or not the value is of the type. E.g. `"test" <str> is-a? (=> true)`.
