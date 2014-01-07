/*
 *	unicomplex/include/p-lang/preproc.h
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#ifndef PREPROC_H
#define PREPROC_H

enum pp_flags {
	PP_CRLF_BIT = 0,
	PP_CR_BIT,
	PP_UNIFDEF_BIT,
	PP_NO_SUBST_BIT,

	PP_CRLF     = 1 << PP_CRLF_BIT,
	PP_CR       = 1 << PP_CR_BIT,
	PP_UNIFDEF  = 1 << PP_UNIFDEF_BIT,
	PP_NO_SUBST = 1 << PP_NO_SUBST_BIT,

	/* Let's hope, we don't get so many flags that this becomes a
	 * possible combination */
	PP_POISON   = 0xdeadc0de
};

/* We support \1 to \9 in #subst. Due to internals, this is also the limit
 * for parameters to a #define macro */
#define PP_MAX_SUBMATCHES	9

struct pp_subst {
	struct pp_subst *next;
	int is_define : 1;
	char *pattern;
	char *replace;
};

#if 0
struct pp_cond {
	struct pp_cond *next;
	;
};
#endif

struct line {
	struct line *next;
	char *line;
	size_t len;
};

struct preproc {
	enum pp_flags flags;
	struct pp_define *defines;
	struct pp_subst *substs;
	/*struct pp_cond *conds;*/
	struct line *lines;
};

#define pp_for_each_line(pp, l)		\
for (l = (pp)->lines; l; l = l->next)

#define pp_for_each_lineno(pp, l, n)	\
for (l = (pp)->lines, n = 1; l; l = l->next, n++)

/* Internals */
struct preproc *read_source(FILE *fp, enum pp_flags flags);
int write_source(const struct preproc *pp, FILE *fp);
void free_preproc(struct preproc *pp);

int contract_lines(struct preproc *pp);
int remove_comments(struct preproc *pp);
int normalize_spaces(struct preproc *pp);
int parse_lines(struct preproc *pp);

int substitute(struct preproc *pp);
int apply_directives(struct preproc *pp);

/* Normal execution flow in compilation */
int preprocess(FILE *in, FILE *out, enum pp_flags flags);

/* Alternative execution flow for an unifdef program */
int unifdef(FILE *in, FILE *out, enum pp_flags flags);

#endif /* PREPROC_H */
