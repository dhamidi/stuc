EXECNAME=stuc
PREFIX  ?= /usr/local
EPREFIX ?= $(PREFIX)/bin

$(EXECNAME):

install: $(EPREFIX)/$(EXECNAME)

$(EPREFIX)/$(EXECNAME): $(EXECNAME)
	cp $(EXECNAME) $(EPREFIX)/$(EXECNAME)

test: $(EXECNAME)
	export STUCPATH=$$PWD
	[ "$$(./$(EXECNAME) test-data/bar)" = "bar" ]
	[ "$$(./$(EXECNAME) test-data/now)" = "$$(date)" ]

.ONESHELL: test
