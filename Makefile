all: main

main:
	gcc system-call-numbers.c -o system-call-numbers
	gcc -nostdlib linux-x86-64.S main.c -o main

clean:
	rm -f *~ a.out ./system-call-numbers ./main

diff:	clean
	git difftool HEAD
