all: main run

SRC=syscall-linux-x86-64.S startup-linux-x86-64.S demo.c startup.c system-call.h hello.c

main: ${SRC} system-call-numbers.c
	gcc -g -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S startup.c demo.c -o demo
	gcc -nostdlib startup-linux-x86-64.S syscall-linux-x86-64.S startup.c demo.c -o demo.skinny
	gcc hello.c -o hello
	gcc -g -w -o linux-structure-metadata structure-metadata.c structure-code-generator.c linux-structure-metadata.c
	gcc -g -w -o test-structure-metadata structure-metadata.c structure-code-generator.c test-structure-metadata.c
	gcc system-call-numbers.c -o system-call-numbers

run: main
	./demo hello there
	./system-call-numbers
	./linux-structure-metadata
	./test-structure-metadata

clean:
	rm -f *~ a.out ./system-call-numbers demo demo.skinny hello *~ structure-metadata linux-structure-metadata test-structure-metadata

diff:	clean
	git difftool HEAD
