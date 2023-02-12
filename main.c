#include "lib-system-call-single-header-file-library.h"
#include <stdio.h>

int main(int argc, char **argv) {

     fprintf(stdout, "SYS_write = %d\n", SYS_write);
     fprintf(stdout, "SYS_exit = %d\n", SYS_exit);

     long errror_1 = syscall(SYS_write, 1, "Hello World!\n", 13);
     long error = syscall(SYS_exit, 42);
     return 0;
}
