targets += p-lang.o.o $(elf_targets)
elf_targets += upp

include $(MBUILD)/Makefile.build

p-lang.o.o: preproc.c.o error.c.o

upp: p-lang.o.o upp.c.o

aesthetic_targets += LANGUAGE

include $(TREE)/Makefile.aesthetic
