elf_targets += test
targets += lib.o.o $(elf_targets)

include $(MBUILD)/Makefile.build

lib.o.o: read_config.c.o

test: test.c.o lib.o.o
