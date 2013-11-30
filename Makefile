.POSIX:

PROGRAMS=stuc stuc-dispatch stuc-eval stuc-find stuc-get stuc-path

include config.mk

all: $(PROGRAMS)

clean:
	-cd ${PWD}; rm -v ${PROGRAMS}

install: install-bin install-man

install-bin:
	@for program in ${PROGRAMS}; do cp -v ./$$program ${BINPREFIX}/; done
