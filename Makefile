all: main

SRC=linux-x86-64.S startup-linux-x86-64.S main.c system-call.h

main: ${SRC}
	gcc -g -nostdlib linux-x86-64.S startup-linux-x86-64.S main.c -o main

numbers: ${SRC} system-call-numbers.c
	gcc system-call-numbers.c -o system-call-numbers

clean:
	rm -f *~ a.out ./system-call-numbers ./main *~

diff:	clean
	git difftool HEAD
