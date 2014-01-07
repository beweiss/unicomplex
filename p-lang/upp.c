/*
 *	unicomplex/p-lang/upp.c - P preprocessor
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <error.h>

#include <uc/p-lang/preproc.h>
#include <uc/p-lang/upc.h>

const char *source_file_name;

static int use_crlf = 0;
static int use_cr = 0;
static int no_subst = 0;

struct option opts[] = {
	{"crlf", 0, &use_crlf, 1},
	{"cr", 0, &use_cr, 1},
	{"unifdef", 0, NULL, 'u'},
	{"no-subst", 0, &no_subst, 1}
};

int main(int argc, char **argv)
{
	FILE *in, *out = stdout;
	enum pp_flags flags = 0;
	struct preproc *pp;

	source_file_name = "samples/preproc.P";
	in = fopen("samples/preproc.P", "r");
	if (!in)
		error(1, errno, "fopen");
	pp = read_source(in, flags);
	if (!pp)
		error(1, errno, "read_source");
//	preprocess(pp);
	write_source(pp, out);
	free_preproc(pp);
	exit(0);
}
