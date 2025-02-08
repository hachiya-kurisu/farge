OS != uname -s

-include Makefile.$(OS)

PREFIX ?= /usr
MANDIR ?= /share/man

all: farge

farge: farge.c
	${CC} -Os -Wall -pedantic -o farge farge.c

install:
	install farge ${DESTDIR}${PREFIX}/bin/farge

push:
	got send
	git push github

README.md: README.gmi
	sisyphus -a "." -f markdown <README.gmi >README.md

doc: README.md

again: clean farge

clean:
	rm -f farge

release: push
	git push github --tags
