# The Startup Library

"main()" is not the first function in a binary that is executed on
Linux. Although it can be renamed with linker magic, an entry-point
called "_start" is first executed which determines argc, argv, and
envp, then initializes the C library itself, and then the main()
function is actually invoked.

One of the better startup descriptions of this process seems to be:

https://0xax.gitbooks.io/linux-insides/content/Misc/linux-misc-4.html

    At the very beginning, when _start is called, our stack looks
    like:

    +-----------------+
    |       NULL      |
    +-----------------+ 
    |       ...       |
    |       envp      |
    |       ...       |
    +-----------------+ 
    |       NULL      |
    +------------------
    |       ...       |
    |       argv      |
    |       ...       |
    +------------------
    |       argc      | <- rsp
    +-----------------+

The point of startup-linux-x86-64.S and startup.c is to call main with
argc, argv, and envp so that you don't need any other libraries for
your extremely low-level code that doesn't depend on anything except
except a very small routine to do system calls. Our sample program
with no debug information is 14,392 bytes 

