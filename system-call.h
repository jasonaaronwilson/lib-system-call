#ifndef SYSTEM_CALL_H
#define SYSTEM_CALL_H

#define SYS_write 1
#define SYS_exit 60

extern long int syscall(long int __sysno, ...);

#endif /* SYSTEM_CALL_H */
