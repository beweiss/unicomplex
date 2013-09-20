targets += uc-client-gambas.gambas

GB_PROJECT = uc-client-gambas/

include $(MBUILD)/Makefile.build

uc-client-gambas.gambas:
	@which gbc3 >/dev/null 2>&1
	@if [ "$$?" -ne "0" ]; then echo -n "Gambas3 not installed. " >&2 \
	 echo "Nothing to do." >&2; exit 0; fi
	$(B_PRE)echo -e "  GBC3  \t" $(RELDIR)$(GB_PROJECT)
	$(Q)gbc3 -ga $(GB_PROJECT)
	$(B_PRE)echo -e "  GBA3  \t" $(RELDIR)$@
	$(@)gba3 -o $@ $(GB_PROJECT)
