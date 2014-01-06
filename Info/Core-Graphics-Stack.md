Forward
=======

The lower level graphics stack of `Gfx` largely reflects the `Core Graphics` framework. Abstractions are made in the implementation to allow other backing libraries to eventually be used.

There will be a higher level interface developed on top of the core functions described in this document.

Classes
=======

The following is a mostly exhaustive list of the classes in the `Gfx` stack that pertain to the graphics stack:

- `gfx::Context`: Encapsulates the destination of all drawing operations.
- `gfx::Layer`: Encapsulates a prerendered `Gfx` image.
- `gfx::Color`: Encapsulates colors. Only `RGBa` colors are currently supported.
- `gfx::Path`: Encapsulates bezier paths.
- `gfx::Image`: Encapsulates creation and rendering of bitmap images.
- `gfx::Font`: Encapsulates font information.
- `gfx::Attributedstring`: Encapsulates a blob of text to be rendered.

Vectors
=======

The core graphics stack functions use vectors of numbers to represent core concepts such as Coordinate Points, Sizes, and Rectangles (Coordinate Points and a Size). The following list describes the vector variations referenced in this document:

- `size-vector`: A vector of two numbers, a `width` and a `height`.
- `point-vector`: A vector of two numbers describing a coordinate point, an `x` coordinate, and a `y` coordinate.
- `rect-vector`: A vector of two or four numbers. A full rect vector contains an `x` coordinate, a `y` coordinate, a `width`, and a `height`. A short-form `rect-vector` contains just a `width` and a `height`, and zero `xy` coordinates. A short-form `rect-vector`s may be used anywhere a long-form `rect-vector` is. No built-in functions will yield short-form `rect-vector`s.

ctx
===

ctx is the in-language name for `gfx::Context`s. The following functions are available:

- `ctx/begin (vec -- ctx)`: Takes a size-vector and creates a new canvas, pushing it onto both the context stack, and the language stack.
- `ctx/end (ctx -- )`: Consumes a ctx and pops it off of the context stack.
- `ctx/size (ctx -- vec)`: Takes a ctx and pushes its size-vector onto the language stack.
- `ctx/save (ctx str -- )`: Saves the contents of the ctx into the path contained in the str. The resulting file is a `png`.

Color
=====

There are three major forms colors take in the Gfx language. Colors are the one component of the graphics stack that has a literal syntax, taking the form of six-character HTML color codes. For example, the color white in a color literal would be `#ffffff`.

A number of simple colors are also provided by name. These colors are:

- `white`
- `black`
- `translucent`
- `red`
- `green`
- `blue`
- `orange`
- `purple`
- `pink`
- `brown`
- `yellow`

The exact values of these colors (excepting `white`, `black`, and `translucent`) are an implementation detail.

RGB and RGBa colors can be created using the functions of the same name:

- `rgb (num num num -- color)`: Takes red, green, and blue components, pushing a color onto the stack. All values are in the scale of {0, 255}.
- `rgba (num num num num -- color)`: Takes red, green, blue, and alpha components, pushing a color onto the stack. Red, green, and blue values are in the scale of {0, 255}, alpha is in the scale of {0, 1.0}.

###Using Colors

Following Core Graphics, Gfx has a concept of a fill color and a stroke color. A fill color is used when flood filling a rectangle or a path, and a stroke color is used when stroking the outline of a rectangle or a path. To specify fill and stroke colors, the following functions are available:

- `set-fill (color -- )`: Sets the current fill color.
- `set-stroke (color -- )`: Sets the current stroke color.

Simple Operations
=================

The following simple operations are included in the core graphics stack:

- `fill-rect (vec -- )`: Fills a rectangle's area with the current fill color.
- `stroke-rect (vec -- )`: Strokes a rectangle's border with the current stroke color.

Paths
=====

Bezier paths in Gfx go by the name of `path`. Paths consist of straight and curved lines that can be put together to make shapes such as rectangles and circles. 

###Creation

- `path/make ( -- path)`: Creates an empty path, pushing it onto the stack.
- `path/rect (vec -- path)`: Takes a rect-vector and creates a path from it, pushing it onto the stack.
- `path/round-rect (vec num -- path)`: Takes a rect-vector and corner radius number and creates a path from it, pushing it onto the stack.
- `path/oval -- (vec -- path)`: Takes a rect-vector and creates an oval path from it, pushing it onto the stack.

###Manipulating Paths

- `path/move (path vec -- path)`: Moves the pen (current location) of a path to a given point-vector.
- `path/line (path vec -- path)`: Draws a line from the current location of a path, to a given point-vector.
- `path/arc (path vec1 vec2 num -- path)`
- `path/curve (path vec vec1 vec2 -- path)`

###Introspection

- `path/bounding-box (path -- vec)`: Determines the area of the path, pushing a rect-vector onto the stack.
- `path/current-point (path -- vec)`: Determines the current location of a path's pen, pushing a point-vector onto the stack.
- `path/empty? (path -- bool)`: Indicates whether or not a path is empty.
- `path/contains-point (path vec -- bool)`: Indicates whether or not a path contains a point.

###Commands

- `path/fill (path -- )`: Performs a fill operation on the path's contents.
- `path/stroke (path -- )`: Strokes the outline of the path's lines.

Text
====

The text portion of the graphics stack is currently a bit anemic. Only basic text operations are currently available. This is mostly a matter of figuring out the best way to expose the complex APIs into the language.

- `font (str num -- font)`: Looks up a font with name `str` of size `num` and pushes it onto the stack. Throws an exception of the font cannot be found. The name of the font is the post-script name.
- `font/regular (num -- font)`: Looks up the platform specific regular weight system font with a given size `num`, yielding the result.
 `font/bold (num -- font)`: Looks up the platform specific bold weight system font with a given size `num`, yielding the result.
  `font/italic (num -- font)`: Looks up the platform specific italic weight system font with a given size `num`, yielding the result.
- `text (font color str -- text)`: Creates a new text with a font, color, and string.
- `text/size (text -- vec)`: Indicates the bounding box of a text.
- `text/draw-at (text vec -- )`: Renders the text at point-vector `vec`. The text will be rendered starting from the given point-vector. It will wrap if its width exceeds the area between the x-coordinate and the right edge of the containing context. __Note:__ this function may be removed due to its behavior being borderline too magical.
- `text/draw-in (text vec -- )`: Renders the text within a given rect-vector `vec`.

Layers
======

Generally speaking, most rendering in the graphics stack is into contexts maintained by layers. Layers in Gfx are similar in concept to Photoshop layers, and to CALayers. A layer contains some prerendered content, typically described by Gfx code. Layers have positioning and sizing information associated with them, and may be nested within each other. Gfx hosts generally manage the creation and positioning of layers, however, the core graphics stack provides all of the core operations needed to manipulate layers within the stack itself.

###Creating Layers

- `layer (vec func -- Layer)`: Creates a simple layer object using the rect-vector `vec`, and the render callback `func`. The `func` will be run whenever the layer needs to populate its contents.

###Positioning

- `layer/set-frame (layer vec -- )`: Updates the frame rectangle of `layer` with the rect-vector `vec`.
- `layer/frame (layer -- vec)`: Indicates the frame rectangle of `layer`.

###Rendering

- `layer/display (layer -- )`: Marks the contents of the layer as needing updating. Depending on the layer implementation in use, this will result in the layer immediately redrawing, or it being redrawn in the next runloop cycle.
- `layer/render (layer -- )`: Renders the contents of the layer and its children into the current ctx.

###Children

An arbitrary number of layers may be nested inside other layers.

- `layer/add-child (layer(parent) layer(child) -- )`: Adds a child layer into a parent layer.
- `layer/remove-as-child (layer -- )`: Removes the layer as a child of its parent.
- `layer/parent (layer -- layer)`: Indicates the parent of the layer, if any.
- `layer/children (layer -- vec)`: Indicates all the children of a layer.

Gradients
=========

The core graphics stack includes a very simple gradient type that can draw simple rectangles, or can be used to fill complex paths.

- `linear-gradient (vec num -- gradient)`: Takes a vector of the form `[color num ...]`, an angle number, and yields a gradient. E.g. `[red 0.0 yellow 1.0] 90 linear-gradient`.
- `radial-gradient (vec vec -- gradient)`: Takes a vector of the form `[color num ...]`, a point-vector number, and yields a gradient. E.g. `[red 0.0 yellow 1.0] [0.5 0.5] radial-gradient`.
- `gradient/draw (gradient angle -- )`: Takes a gradient, a rect-vector or path. Draws the gradient into the rect-vector or path with.

Shadows
=======

The core graphics stack includes a simple shadow type that can be applied to almost anything.

- `shadow (vec num color -- shadow)`: Takes a point-vector describing the offset of the shadow, a number describing its blur radius, and a color. Yields a shadow.
- `shadow/set (shadow -- )`: Applies the shadow to the current context. Any drawing after this will have the shadow applied to its edges.
- `shadow/unset ( -- )`: Removes any currently active shadow in the current context.

Images
======

The core graphics stack includes a barebones Image type that is capable of reading and writing PNGs.

- `image/from-file ( file -- image )`: Attempts to parse the contents of a `file` into an image. Yields the image if successfull, null otherwise.
- `image/size ( image -- vec )`: Yields the size of the image as a size-vector.
- `image/draw-in ( image vec -- )`: Draws the image into the rect-vector `vec`.
- `image/save-to ( image file -- ) `: Attempts ot save the `image` into the `file`. Throws an exception if there are any issues.
