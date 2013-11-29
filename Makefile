EXECNAME=stuc
SUBCMDS = $(basename $(wildcard stuc-*.c))
PREFIX  ?= /usr/local
EPREFIX ?= $(PREFIX)/bin
BINARIES =$(EPREFIX)/$(EXECNAME) $(foreach subcmd,$(SUBCMDS),$(EPREFIX)/$(subcmd))

#@? create all binaries
all: $(EXECNAME) $(SUBCMDS)

#@? remove all binaries from the current directory
clean:
	@rm -v $(addprefix ./,$(EXECNAME)) $(addprefix ./,$(SUBCMDS))

#@? install binaries into ${PREFIX}/bin
install: $(BINARIES)

#@? uninstall binaries from ${PREFIX}/bin
uninstall: $(BINARIES)
	@rm -v $(BINARIES)

$(EPREFIX)/%: %
	cp $* $(EPREFIX)/$*

#@? list all targets
help:
	@awk -F':' '/^#@?/ {help=substr($$0,5)} /^([a-z]+):/{printf("%10s -- %s\n",$$1,help)}' Makefile
