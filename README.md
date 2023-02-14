# README

This directory has two related projects:

1. A non "libc" minimal process startup replacement
2. A non "libc" syscall library

This library provides the "indirect" system call interface (namely the
same syscall function from from unistd.h) separate from libc.

Since a major reason to use such a library when glibc and other
standard libraries provide this funtionality already is to avoid using
those libraries entirely, we also provide a small startup library
which supplies argc, argv, and envp from the initial conditions when
the binary is first executed since this information is not recoverable
from simple system calls.

## Rationale

This is primarily meant for language compiler writers that would like
to target C code as though it were assembly language but otherwise
would prefer to not use *any* particular C libraries.

## Platforms

We will initially target gcc and clang on x86-64 bit Linux.

## Usage

    gcc -nostdlib linux-x86-64.S main.c -o main

Where main.c can be as simple as:

    #include "system-call.h"

    void main(int argc, char **argv) {
         long error = syscall(SYS_exit, 42);
         return 0;
    }

    // This is likely to change when using our startup routine.

    // Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
    __attribute__((force_align_arg_pointer))
    void _start() {
        main(0, 0);
        __builtin_unreachable();  // tell the compiler to make sure side effects are done before the asm statement
    }
