# Auburn
A small library written in C99 using the Raylib library to simulate falling leaves.

The program implements a basic fluid simulator to generate somewhat randomized leaf movement. I stayed conservative with the effect on the leaves so that the general effect is still of leaves falling down.

# Demo
[Video Coming Soon...]

# Running
For linux users, the `auburn` executable is provided. You can run it through the command line:
```
$ auburn
```
It will create a 1024x1024 non-resizable window with the simulation. Press R to reset/initialize the fluid simulation, and hold D to look at the fluid simulation rather than the leaves.

# Building
A makefile is provided with profiling, debugging, and release targets. If you only want to run the program, on the command line you can call
```
$ make release
```

Otherwise, choose the option corresponding to what you want to do with the code. The `make profile` option creates an executable that you can analyze with `gprof`, and `make debug` embeds debugging symbols and disables optimizations to make it easier to debug with `gdb`. It also enables address sanitizer.
