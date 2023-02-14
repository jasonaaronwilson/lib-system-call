#include "system-call.h"

long c_string_length(char *str) {
  if (str == 0) {
    return 0;
  }
  for (long i = 0; 1; i++) {
    if (str[i] == 0) {
      return i;
    }
  }
  syscall(SYS_exit, 1);
}

#define PATH_MAX 4096

int main(int argc, char **argv) {

  char buffer[PATH_MAX];

  long result = 0;

  result = syscall(SYS_getcwd, buffer, PATH_MAX);

  result = syscall(SYS_write, 1, buffer, c_string_length(buffer));
  result = syscall(SYS_write, 1, "\n", 1);

  // This doesn't actually appear to work...
  result = syscall(SYS_chdir, 1, "..");

  result = syscall(SYS_write, 1, "Hello World!\n", 13);

  result = syscall(SYS_exit, 42);

  return 0;
}

// Tell the compiler incoming stack alignment is not RSP%16==8 or ESP%16==12
__attribute__((force_align_arg_pointer))
void _start() {
  main(0, 0);
    __builtin_unreachable();  // tell the compiler to make sure side effects are done before the asm statement
}

