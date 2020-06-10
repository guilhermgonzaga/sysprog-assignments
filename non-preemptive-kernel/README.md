# Non-preemptive kernel

The objective with this project was to enable multiprogramming in a very primitive kernel,
with support for kernel threads as well as processes. There are also two implementations for mutex locks.


### Specification

This project was based on [this one](https://www.cs.princeton.edu/courses/archive/fall19/cos318/projects/project2/p2.html), from the Department of Computer Science in Princeton University.

This project is not necessarily an acceptable solution to the original specification, as ours was modified to fit time constraints and attendees' diverse curriculum progression.


## Getting started

Development has been aimed at and only simulated on Linux (Ubuntu 18.04 and Mint 19.1 distros).

Everything can be built with the `make` utility.

In `tasks.c`, there is an array of tasks to be scheduled. It's initializer can be edited to exclude specific tasks or include new ones.
The following example tasks were provided.

- `task1` (thread) displays a timer (in seconds) as well as a counter for how many times it's been scheduled.
- `task2` and `task3` (threads) test the mutual exclusion implementation updating a shared variable.
- `task4` (process) displays an airplane flying across the terminal screen.
- `task5` (process) sums all numbers in the range \([0, n]\) for \(n\) in \([0, 100]\) and displays each new result overwriting the previous one.
- `time_switch_1` and `time_switch_3` (threads) measure the context switch overhead between two threads.
- `time_switch_3` (process) measures the context switch overhead between two processes (two instances needed).

After a successful build, a file `floppy.img` is created. It can then be tested with [Bochs](http://bochs.sourceforge.net/) or maybe even booted from a drive (not tested and we take no responsibility for bad outcomes).
