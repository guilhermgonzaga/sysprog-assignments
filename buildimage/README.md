# buildimage: ELF32 file reader

This project was built to learn how an operating system boots up by setting up a bootloader and a primitive kernel in a bootable image. For the most part, the implementation in `buildimage.c` gets the two referred executables (ELF32 files) and extracts their information in order to create the image.


### Specification

This project was based on [this one](https://www.cs.princeton.edu/courses/archive/fall19/cos318/projects/project1/p1.html), from the Department of Computer Science in Princeton University.

This project is not necessarily an acceptable solution to the original specification, as ours was modified to fit time constraints and attendees' diverse curriculum progression.


## Workings

Development has been aimed at and only tested on Linux (Ubuntu 19 and Debian 10 distros).

Everything can be done via the `make` utility.

Make sure to overwrite the `kernel.s` file with one of the three available kernels in order to change tests.
```sh
cp kernel-small.s  kernel.s
cp kernel-medium.s kernel.s
cp kernel-large.s  kernel.s
```

After the build steps, the image is created and placed on the `bin` folder. It can then be tested with [Bochs](http://bochs.sourceforge.net/) or maybe even booted from a drive (not tested and we take no responsibility for bad outcomes).
