# README

This directory contains two related projects:

1. A non "libc" minimal startup replacement
2. A non "libc" syscall library

#1 actually depends on #2 and this is OK from a binrary size
perspective because #2 is a trivial amount of code on Linux and allows
#1 to call SYS_exit to behave more as expected.

The sample program "sample.c" excercises both #1 and #2 to produces a
small binary with zero external dependencies while hello.c represents
a minimal binary that uses the default dynamic linking:

    jasonaaronwilson@penguin:~/src/lib-system-call$ ldd sample
            statically linked
    jasonaaronwilson@penguin:~/src/lib-system-call$ ldd hello
            linux-vdso.so.1 (0x00007ffc9dc9a000)
            libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007d6a6f5e1000)
            /lib64/ld-linux-x86-64.so.2 (0x00007d6a6f7d6000)
    jasonaaronwilson@penguin:~/src/lib-system-call$ ls -al hello sample.skinny 
    -rwxr-xr-x 1 jasonaaronwilson jasonaaronwilson 16656 Feb 14 10:13 hello
    -rwxr-xr-x 1 jasonaaronwilson jasonaaronwilson 14392 Feb 14 10:13 sample.skinny

"hello" which dynamically links things is not that much larger than
"sample.skinny" so there isn't much to gain from a binary size
perspective but obviously not depending on any shared libraries is a
benefit in some circumstances.
