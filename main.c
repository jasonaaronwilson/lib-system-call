#include "lib-system-call-single-header-file-library.h"

int main(int argc, char **argv) {
     long error = syscall(SYS_exit, 42);
     return 0;
}
