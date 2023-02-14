# System Call Library

This library provides the "indirect" system call function separate
from libc (aka syscall from unistd.h).

## Rationale

This is primarily meant for language compiler writers that would like
to target C code as though it were assembly language but otherwise
would prefer to not use *any* C standard libraries.

## Platforms

We will initially target gcc and clang on x86-64 bit Linux.

## Usage

    gcc -nostdlib linux-x86-64.S main.c -o main

Where main can be as simple as 

    #include "system-call.h"

    void main(int argc, char **argv) {
         long error = syscall(SYS_exit, 42);
         return 0;
    }

    // Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
    __attribute__((force_align_arg_pointer))
    void _start() {
        main(0, 0);
        __builtin_unreachable();  // tell the compiler to make sure side effects are done before the asm statement
    }
