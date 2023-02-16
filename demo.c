//
//
// This is a *demonstration* (test) that will not effect your final
// size or speed if you use these libraries to run without the C
// standard library.
// 
//

#include "system-call.h"

#define PATH_MAX 4096

void print_newline() {
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
void print_c_string(char *str) {
  syscall(SYS_write, 1, str, c_string_length(str));
}

//
// Reverse a C style (NUL terminated) string.
//
void reverse_c_string(char *str) {
  long len = c_string_length(str);
  if (len > 1) {
    long low = 0;
    long high = len - 1;
    while (low < high) {
      char low_ch = str[low];
      char high_ch = str[high];
      str[low] = high_ch;
      str[high] = low_ch;
      low += 1;
      high -= 1;
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
void print_number(long number) {
  long result = 0;
  char buffer[PATH_MAX];
  long_to_c_string(buffer, number);
  print_c_string(buffer);
}

void print_char(char ch, int times) {
  char buffer[times + 1];
  for (int i = 0; i < times; i++) {
    buffer[i] = ch;
  }
  buffer[times+1] = 0;
  print_c_string(buffer);
}

//
// Print the current working directory to stdout with a newline.
//
void print_current_working_directory() {
  long result = 0;
  char buffer[PATH_MAX];
  result = syscall(SYS_getcwd, buffer, PATH_MAX);
  print_c_string(buffer);
  print_newline();
}

int main(int argc, char **argv, char **envp) {
  long result = 0;

  print_c_string("** Command line arguments **\n");
  for (int i = 0; i < argc; i++) {
    print_number(i);
    print_char(' ', 4);
    print_c_string(argv[i]);
    print_newline();
  }

  print_c_string("\n**Environment**\n");
  for (int i = 0; 1; i++) {
    char* var = envp[i];
    if (var == 0) {
      break;
    }
    print_number(i);
    print_char(' ', 4);
    print_c_string(var);
    print_newline();
  }

  print_current_working_directory();
  result = syscall(SYS_chdir, "..");
  print_current_working_directory();
  result = syscall(SYS_chdir, "/");
  print_current_working_directory();

  long* addr = (long *) syscall(SYS_mmap, 0, 8192, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
  print_c_string("Memory was mapped at ");
  print_number((long) addr);
  print_newline();
  addr[0] = 4242;
  print_number(*addr);
  print_newline();

  result = syscall(SYS_exit, 0);

  return 0;
}
