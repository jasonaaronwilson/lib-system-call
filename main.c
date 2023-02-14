#include "system-call.h"

#define PATH_MAX 4096

void show_newline() {
  syscall(SYS_write, 1, "\n", 1);
}

//
// This is just a simple definition of strlen (though it accepts NULL
// as an argument because why not).
//
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

//
// Print a string to stdout followed by a newline.
// 
void show_c_string(char *str) {
  syscall(SYS_write, 1, str, c_string_length(str));
  show_newline();
}

//
// Reverse a C style (NUL terminated) string.
//
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

//
// This is like sprintf(buffer, "%d", number) except we probably don't
// handle negative numbers correctly.
//
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

//
// Print a non-negative number to stdout with a newline.
//
void show_number(long number) {
  long result = 0;
  char buffer[PATH_MAX];
  long_to_c_string(buffer, number);
  show_c_string(buffer);
}

//
// Print the current working directory to stdout with a newline.
//
void show_current_working_directory() {
  long result = 0;
  char buffer[PATH_MAX];
  result = syscall(SYS_getcwd, buffer, PATH_MAX);
  show_c_string(buffer);
}

int main(int argc, char **argv) {
  long result = 0;

  for (int i = 0; i < argc; i++) {
    show_number(i);
    show_c_string(argv[i]);
  }

  // Test our long to string
  // long_to_c_string(buffer, 9190919);

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

