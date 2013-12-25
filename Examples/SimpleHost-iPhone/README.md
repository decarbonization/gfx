SimpleHost-iPhone
=================

The `SimpleHost-iPhone` example is an extremely barebones application that demonstrates how to host the Gfx language within an iPhone application. Like the Mac verison of this example, it is written entirely using the Objective-C interface to the Gfx stack. The same views are used in both.

Currently this example has no persistence mechanism, and does not have any sort of interactive mode. Additionally, it's currently impossible to resize the canvas that the Gfx code will be rendered into.

In the future, this example will be fleshed out to have all of the same functionality as the `SimpleHost` Mac example.

Requirements
============

The `SimpleHost-iPhone` project is setup with the `Gfx-iOS` static library target as a dependency. All of the appropriate frameworks and libraries in order to link have been added. The code exclusively targets iOS 7, but could be back-ported to iOS 6 with minimal effort.