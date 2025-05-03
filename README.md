# Dynamic Memory Allocator | emalloc

A dynamic virtual memory allocator realized as an Ensimag project
from the Concurrent programming and OS [course](https://ensimag.grenoble-inp.fr/fr/formation/syst-egrave-me-d-exploitation-et-programmation-concurrente-4mmseps1).

In this repository you have a memory allocator source code that could replace (**~~Not right now~~**) the classical `malloc` and `free` library functions from the Standard C library

## Setup

All the setup is fournished on the Dockerfile for anyone that wants to interact with the malloc library
on a container


## How to use

### Build
In order to compile the project and the tests with its dependencies, we execute:

```bash
cd build/
cmake ..
make
```



### Tests
There is a set of tests that can be run to verify the emalloc  works correctly. On the project directory we execute

```bash
cd build/
make test
make check
```

* `make test`: Verifies all tests passes executing all test 

* `make check`: Executes all tests and shows you a more detail view on each test result


### Memshell
There is an CLI called `memshell` that allows to make calls to the emalloc library to test allocation operations manually. For the moment it can only allocate and release memory blocks through an allocator. 

If you want to try it, go to the `build` directory and execute

```bash
./memshell
```

#### Debug mode
But, I advice you to run it with the gdb in order to see how the malloc main data structures are changed through allocation.

For that, on the `build` directory you can run

```bash
gdb -- ./memshell
```

## Warnings

It doesn't work yet on x32 machines, only on x64