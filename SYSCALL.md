# System Call Library

The goal of this library is to provide "indirect" system call
functionality via the "syscall" interface.

Since a major reason to use such a library when glibc and other
standard libraries provide this funtionality already is to avoid using
those libraries entirely, we also provide a small startup library (in
this same directory, STARTUP.md) which supplies argc, argv, and envp
from the initial conditions when the binary is first executed since
this information is not obtainable from system calls.

## Rationale

This is primarily meant for language compiler writers that would like
to target C code as though it were assembly language and have as few
dependencies as possible on "libc".

## Platforms

We are initially targeting gcc and clang on x86-64 bit Linux.

(I need help with other platforms! I'd love to have riscv64 and arm64
linux support followed by x86-64 and arm64 for Darwin.)

## Usage

Luckily gcc accepts .S files so for now give something like this a
shot:

    gcc -g -nostdlib linux-x86-64.S startup-linux-x86-64.S start.c <here>

Where <here> can be as simple as a path to a single file containing
something as simmple as:

    #include "system-call.h"

    void main(int argc, char **argv) {
        syscall(SYS_exit, 42);
    }
