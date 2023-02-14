        .text
        .globl _start
_start:
        xor %rbp,%rbp
        mov %rsp,%rdi
        call c_underscore_start
        hlt

