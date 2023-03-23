# README

This directory contains three related projects:

1. A non "libc" minimal startup replacement
2. A non "libc" syscall library
3. A structure/union "decoder"

#1 actually depends on #2 and this is OK from a binrary size
perspective because #2 is a trivial amount of code on Linux and allows
#1 to call SYS_exit to behave more as expected.

The demo program "demo.c" excercises both #1 and #2 to produces a
small binary with zero external dependencies while hello.c represents
a minimal binary that uses the default dynamic linking:

    jasonaaronwilson@penguin:~/src/lib-system-call$ ldd demo
            statically linked
    jasonaaronwilson@penguin:~/src/lib-system-call$ ldd hello
            linux-vdso.so.1 (0x00007ffc9dc9a000)
            libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007d6a6f5e1000)
            /lib64/ld-linux-x86-64.so.2 (0x00007d6a6f7d6000)
    jasonaaronwilson@penguin:~/src/lib-system-call$ ls -al hello demo.skinny 
    -rwxr-xr-x 1 jasonaaronwilson jasonaaronwilson 16656 Feb 14 10:13 hello
    -rwxr-xr-x 1 jasonaaronwilson jasonaaronwilson 14392 Feb 14 10:13 demo.skinny

"hello" which dynamically links things is not that much larger than
"demo.skinny" so there isn't much to gain from a binary size
perspective but obviously not depending on any shared libraries is a
benefit in some circumstances.

## A Structure/Union Decoder

In order to properly interact with the Linux kernel or other C
libraries, being able to interact with C structures and unions is
essential.

Instead of directly parsing C structure or union definitions and
recreating the logic a C compiler uses to layout these objects in
memory, we can use a C program to determine the the actual memory
layout (and size) of fields and use that to generate code in a foriegn
language (of course this is probably architecture dependent). Since we
are likely to want to heavily edit the names of both these structures
and fields anyways, this isn't actually that terrible.

This code can be reused though it might be simpler to simply parse the
output which outputs "$define-blob".



