all: main numbers run

SRC=syscall-linux-x86-64.S startup-linux-x86-64.S sample.c startup.c system-call.h hello.c

main: ${SRC}
	gcc -g -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S startup.c sample.c -o sample
	gcc -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S startup.c sample.c -o sample.skinny
	gcc hello.c -o hello
	# gcc -g sample.c -o sample.fat

run: main
	./sample hello there

numbers: ${SRC} system-call-numbers.c
	gcc system-call-numbers.c -o system-call-numbers

clean:
	rm -f *~ a.out ./system-call-numbers sample sample.skinny hello *~ 

diff:	clean
	git difftool HEAD
