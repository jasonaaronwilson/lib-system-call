all: main

SRC=system-call-numbers.c linux-x86-64.S main.c system-call.h

main: ${SRC}
	gcc system-call-numbers.c -o system-call-numbers
	gcc -g -nostdlib linux-x86-64.S main.c -o main

clean:
	rm -f *~ a.out ./system-call-numbers ./main *~

diff:	clean
	git difftool HEAD
