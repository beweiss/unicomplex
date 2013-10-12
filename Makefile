# Main project Makefile.
# (C) 2013 Tobias Boege

CC			 = gcc -c
CCLD			 = gcc
AS			 = as
MERGE			 = ld -r
LD			 = ld

RM			 = rm -f
STRIP			 = strip

CFLAGS			+= -Wall -ggdb
CPPFLAGS		+= -I$(TREE)/include
LDFLAGS			+= -ggdb

TREE			:= $(CURDIR)
MBUILD			 = $(TREE)/Makefile.build

export CC CCLD AS MERGE LD RM STRIP CFLAGS CPPFLAGS LDFLAGS TREE MBUILD

desc_targets		 = client/ depot/ lib/ master/ p-lang/ proto/ slave/
targets			+= $(desc_targets)
clean_targets		+= $(desc_targets)

include $(MBUILD)/Makefile.build

aesthetic_targets += README Documentation/ p-lang/ proto/

include $(TREE)/Makefile.aesthetic
