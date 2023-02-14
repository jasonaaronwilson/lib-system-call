#include "system-call.h"

// This is just a simple definition of strlen (though it accepts NULL
// as an argument because why not).
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

void reverse_c_string(char *str) {
  long len = c_string_length(str);
  if (len > 1) {
    long low = 0;
    long high = len - 1;
  loop:
    if (low < high) {
      char low_ch = str[low];
      char high_ch = str[high];
      str[low] = high_ch;
      str[high] = low_ch;
      low += 1;
      high -= 1;
      goto loop;
    }
  }
}

void long_to_c_string(char *buffer, long number) {
  int offset = 0;
 loop:
  {
    long remainder = number % 10;
    long next = number / 10;
    buffer[offset] = remainder + 48;
    if (next > 0) {
      offset += 1;
      number = next;
      goto loop;
    } else {
      buffer[offset + 1] = 0;
      reverse_c_string(buffer);
    }
  }
}

#define PATH_MAX 4096

void show_current_working_directory() {
  long result = 0;
  char buffer[PATH_MAX];
  result = syscall(SYS_getcwd, buffer, PATH_MAX);
  result = syscall(SYS_write, 1, buffer, c_string_length(buffer));
  result = syscall(SYS_write, 1, "\n", 1);
}

int main(int argc, char **argv) {
  long result = 0;
  char buffer[PATH_MAX];

  // Test our long to string
  long_to_c_string(buffer, 9190919);
  result = syscall(SYS_write, 1, buffer, c_string_length(buffer));
  result = syscall(SYS_write, 1, "\n", 1);

  show_current_working_directory();
  result = syscall(SYS_chdir, "..");
  show_current_working_directory();
  result = syscall(SYS_chdir, "/");
  show_current_working_directory();

  // Obviously it is useful to be able to print strings.
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

