#include "system-call.h"

int main(int argc, char **argv) {

  // fprintf(stdout, "SYS_write = %d\n", SYS_write);
  // fprintf(stdout, "SYS_exit = %d\n", SYS_exit);

     long errror_1 = syscall(SYS_write, 1, "Hello World!\n", 13);
     long error = syscall(SYS_exit, 42);
     return 0;
}

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
__attribute__((force_align_arg_pointer))
void _start() {
  main(0, 0);
    __builtin_unreachable();  // tell the compiler to make sure side effects are done before the asm statement
}

