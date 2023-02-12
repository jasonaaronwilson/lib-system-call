all: main

main:
	gcc main.c

clean:
	rm -f *~ a.out

diff:	clean
	git difftool HEAD
