#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(int argc, char **argv) {
  fprintf(stdout, "SYS_write = %d\n", SYS_write);
  fprintf(stdout, "SYS_exit = %d\n", SYS_exit);
}
