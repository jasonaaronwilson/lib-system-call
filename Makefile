all: main numbers run

SRC=syscall-linux-x86-64.S startup-linux-x86-64.S main.c start.c system-call.h

main: ${SRC}
	gcc -g -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S start.c main.c -o main

run: main
	./main hello there

numbers: ${SRC} system-call-numbers.c
	gcc system-call-numbers.c -o system-call-numbers

clean:
	rm -f *~ a.out ./system-call-numbers ./main *~

diff:	clean
	git difftool HEAD
