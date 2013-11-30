.POSIX:

PROGRAMS=stuc stuc-dispatch stuc-eval stuc-find stuc-get stuc-path
MANPAGES=stuc.1 stuc-dispatch.1 stuc-eval.1 stuc-find.1 stuc-get.1 stuc-path.1
include config.mk

all: $(PROGRAMS)

clean:
	-cd ${PWD}; rm -v ${PROGRAMS}

install: all install-bin install-man

install-bin:
	@mkdir -p ${BINPREFIX}
	@for program in ${PROGRAMS}; do    \
	cp -v ./$$program ${BINPREFIX}/;   \
	chmod 755 ${BINPREFIX}/$$program ; \
	done

install-man:
	@mkdir -p ${MANPREFIX}/man1
	@for manpage in ${MANPAGES}; do   \
	cp -v ./$$manpage ${MANPREFIX}/man1/;  \
	chmod 644 ${MANPREFIX}/man1/$$manpage; \
	done
