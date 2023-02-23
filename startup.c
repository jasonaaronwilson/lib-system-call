#include "system-call.h"

/**
 * Conceptually C programs start at main(), but execution really first
 * executes at "_start". With this library, we are doing the bare
 * minimum of extra work before invoking the user's main() routine.
 *
 * It's less common to see main taking envp but this prototype only
 * has to work here.
 */
extern int main(int argc, char** argv, char** envp);

/**
 * This is called immediately after _start with a pointer to the top
 * of the stack which is where Linux puts some important start up
 * data.
 *
 * Expecting a certain stack layout, we convert the stack into the
 * arguments that better suit how "main" functions typically are
 * written in C.
 *
 * Then we call the actual main routine.
 */
__attribute__((force_align_arg_pointer))
void c_underscore_start(long int *stk_ptr) {
  // Figure out what argc, argv, and envp are.
  long int argc = *stk_ptr;
  char **argv = (char **) &stk_ptr[1];
  char **envp = (char **) &stk_ptr[2+argc];

  // Invoke the real main routine that the user wrote. If control
  // returns here, call SYS_exit with the result of main.
  long result = main(argc, argv, envp);
  syscall(SYS_exit, result);
}
