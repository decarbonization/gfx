SimpleHost
==========

The `SimpleHost` example is a barebones document-oriented, single layer, graphical interpreter for the gfx language. It is written entirely using the Objective-C interface to the Gfx stack. It contains a naïve syntax highlighting implementation, and uses a `GFXView` to perform all of the heavy lifting.

The SimpleHost application generates documents with the extension `.shgfx`. The documents contain core gfx code that is run in the context of a `gfx::Layer`/`GFXLayer` object. The core language is extended slightly with the addition of an annotation to describe the size of the document canvas. That annotation takes the format of `(% size: 500x500 %)`. All other annotations are ignored, and recorded as non-fatal errors.

Points of Interest
==================

The majority of the code powering `SimpleHost` lives in the Gfx stack library. The following classes are recommended for study:

- `GFXView`
- `GFXLayer` and `gfx::Layer`
- `GFXInterpreter` and `gfx::Interpreter`
- `gfx::Parser`

Within the example itself, the majority of the work is performed by the `GraphicsDocument` class. A simple syntax highlighting system can be found in the `SHTextView` class and its dependencies.

Documentation is available in the headers of all files in the Gfx project.

Requirements
============

The `SimpleHost` project is setup with the `Gfx` OS X framework as a dependency. The code has only been tested on OS X 10.9 "Mavericks".
