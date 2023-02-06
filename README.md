# (Unix) System Call Library

This exteremely low-level library is meant to be a tiny implementation
of Unix system calls (in particular for Linux) from C directly into
the kernel on Unix like systems avoiding the need for any direct usage
of assembly language in projects that rely on it. It is meant to
generate the smallest possible and least dependent executables in a
Unix environment, aka, no need for glibc or even alternative C
standard libraries such as uClibc, Diet libc, and newlib.

This library can also be used anywhere that glibc can be used and the
earliest checkins will simply use glibc.

## Rationale

This is primarily meant for language compilers that would like to
target C code as though it were assembly language but otherwise would
prefer to not use *any* C standard libraries.

## Platforms

We will initially target only gcc and clang on Linux in 64 bit mode.

## Usage

This can be used as a either a "single header file library" or as a
more standard header file plus linking in a ".a" file.

The single header file version would look like roughly like this:

    #include "lib-system-call-single-header-file-library.h"

    void main(int argc, char **argv) {
         long error = syscall(SYS_exit, 42);
         return 0;
    }

The more standard library version would look like:

    #include "lib-system-call.h"

    int main(int argc, char **argv) {
         long error = syscall(SYS_exit, 42);
         return 0;
    }

And then a flag will be required to the C compiler or linker
invocation to find the ".a" file that actually implements the extern
"syscall".

Note that "#include <sys/syscall.h>" and "#include <unistd.h>" do
*not* (and probably should not) be included as would typically be done
when using glibc.
