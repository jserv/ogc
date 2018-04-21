# OGC: Ordinary Garbage Collector

## About

OGC implements the typical "mark and sweep" algorithm. It works as follows:
* there is an internal struct representing a block of memory
* there is a list of blocks representing the allocated memory, and another
  list representing the local stack of memory
* once a new object is created,
    - its memory is allocated and its meta data is added to both lists
    - there is a threshold for the number of objects to allocate before
      cleaning up the memory
    - when the threshold is met:
        + call "mark" with the local stack of memory ("reachable" memory)
        + mark recurses through the list and set a "mark" flag for every
          block reachable from the inital list of reachable memory
        + we then call "sweep" with the global allocated memory list
        + every block that is not marked is not used, and can be freed and
          popped from given list


## How it works

For a basic mark and sweep garbage collector, two things are required:
* a list of all of the allocations made by the program;
* a list of all the allocations in use by the program at any given time;

With these two things the algorithm is simple - compare the two lists and
free any allocations which are in the first list, but not in the second -
exactly those allocations which are no longer in use.

To get a list of all the allocations made by the progam is relatively
simple. We make the programmer use a special function we have prepared
(in this case `gc_alloc`) which allocates memory, and then adds a pointer
to that memory to an internal list. If at any point this allocation is
freed (such as by `gc_free`), it is removed from the list.


## Function list

* `gc_alloc()`
    - Allocates a space in memory of the size which the user requests
    - Ensures the user entered a number > 0
    - Attempts to find a freed block of memory to reuse.
    - On success, returns the address of the newly allocated memory.
      Otherwise, returns NULL.

* `gc_free()`
    - Marks the object as free in the list of objects
    - Ensures the pointer isn't NULL or already free before marking it

* `gc_run()`
    - Recursively marks all reachable objects in the list of objects
    - Recursively sweeps (frees) all unmarked (unreachable) objects
