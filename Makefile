OS != uname -s

-include Makefile.$(OS)

PREFIX ?= /usr
MANDIR ?= /share/man

all: farge

farge: farge.c
	${CC} -Os -Wall -pedantic -static -o farge farge.c

install:
	install farge ${DESTDIR}${PREFIX}/bin/farge

again: clean farge

clean:
	rm -f farge
