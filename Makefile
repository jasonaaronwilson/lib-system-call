all: main numbers run

SRC=syscall-linux-x86-64.S startup-linux-x86-64.S demo.c startup.c system-call.h hello.c

main: ${SRC}
	gcc -g -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S startup.c demo.c -o demo
	gcc -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S startup.c demo.c -o demo.skinny
	gcc hello.c -o hello

run: main
	./demo hello there

numbers: ${SRC} system-call-numbers.c
	gcc system-call-numbers.c -o system-call-numbers

clean:
	rm -f *~ a.out ./system-call-numbers demo demo.skinny hello *~ 

diff:	clean
	git difftool HEAD
