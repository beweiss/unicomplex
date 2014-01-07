/*
 *	unicomplex/p-lang/error.c - Reporting stuff
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#include <stdio.h>
#include <stdarg.h>

#include <uc/p-lang/upc.h>
#include <uc/p-lang/error.h>

void warn(unsigned int line, const char *fmt, ...)
{
	va_list ap;

	fprintf(stderr, "%s:%u: warning: ", source_file_name ? : "?", line);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}
