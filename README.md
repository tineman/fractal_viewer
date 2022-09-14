# fractal_viewer

A side project that allows the user to view the Mandelbrot set fractal and save a gif of a path along it.


ADD SAMPLES

### Acknowledgements

This project uses [SDL2](https://www.libsdl.org/) and a [GIF encoder](https://github.com/lecram/gifenc) by lecram, author of gifenc.h and gifenc.c.

### Usage

1) Please install SDL2 and add it to your PATH. If you are having trouble adding SDL to your PATH, include '-I/Library/Frameworks/SDL.framework/Headers -framework SDL2' to line 2 of the makefile.

2) cd to the directory this file is in and run 'make'. If this doesn't work, try specifying 'make -f makefile'

3) Customise the palette and framerate in helper.h

### Notes

Generating a gif requires a bit of time. Uncomment line 244 in 'main.c' to see the encoder progress frame-by-frame. In addition, this is a personal project, so it is somewhat unstable. A lot of input is not sanitised. All software is released to the public domain as is.