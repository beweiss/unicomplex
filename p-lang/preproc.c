/*
 *	unicomplex/p-lang/preproc.c - P preprocessor
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 *
 * Like the P syntax itself, the P preprocessor is close to the C one but a
 * little less here and little more there. We support only the directive
 * #define (NO conditions and NO #include at the moment) and a more
 * versatile #subst which is a #define on the basis of POSIX extended
 * regular expressions:
 *
 * --8<--[ sample.P ]-------------------------------------------------------
 * #subst ___(.*)	__attribute(\1)
 * ...
 * --8<---------------------------------------------------------------------
 *
 * Other directives are left for future addition.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>

#include <uc/compiler.h>
#include <uc/p-lang/preproc.h>
#include <uc/p-lang/error.h>

#define CRLF	(((unsigned int) '\r' << 8) + '\n')

/*
 * Note that once the pp->lines member is completed, the number of elements
 * is invariant to maintain the line number relation to the original source.
 */

/**
 * __coalesce_lines() - Recursive helper for contract_lines()
 * @l: line to merge with its successor
 *
 * The purpose of this function is to deal with multiple lines to be
 * coalesced. We need to recurse down to the last of these adjacent lines to
 * have a simple algorithm for merging them.
 *
 * Returns -1 on system error and -2 if there is no successor line to merge
 * with.
 */
static int __coalesce_lines(struct line *l)
{
	struct line *next = l->next;
	int res;
	char *tmp;

	/* No need to coalesce? */
	if (!l->len || l->line[l->len - 1] != '\\')
		return 0;

	if (!next)
		return -2;

	/* First, check the next line to merge it with its successor if need
	 * be. If this wasn't recursive, we would end up from a source like:
	 * 	a\
	 * 	b\
	 * 	c
	 * with a result like that:
	 * 	ab\
	 *
	 * 	c
	 * which is wrong.
	 */
	if ((res = __coalesce_lines(next)) < 0)
		return res;

	tmp = l->line;
	l->line = realloc(l->line, l->len + next->len);
	if (!l->line) {
		l->line = tmp;
		return -1;
	}
	/* Remove backslash */
	l->line[l->len - 1] = '\0';
	strcat(l->line, next->line);
	l->len += next->len - 1;

	tmp = next->line;
	next->line = realloc(next->line, 1);
	if (!next->line) {
		next->line = tmp;
		return -1;
	}
	next->line[0] = '\0';
	next->len = 0;
	return 0;
}

/**
 * contract_lines() - Merge lines divided by backslash at end of line
 * @pp: struct preproc
 *
 * EXAMPLE:
 * 	"abc\
 * 	def"
 * becomes
 * 	"abcdef"
 */
int contract_lines(struct preproc *pp)
{
	struct line *l;
	unsigned int lineno;

	pp_for_each_lineno(pp, l, lineno) {
		switch (__coalesce_lines(l)) {
		case -1:
			return -1;
		case -2:
			warn(lineno, "backslash at EOF");
			goto out;
		}
	}
out:
	return 0;
}

/**
 * remove_comments() - Remove comments
 * @pp: struct preproc
 *
 * Comments are typical C-style block comments. These C++-style comments
 * hurt my eyes. I would be lucky if I never had to implement them. Knock
 * on wood!
 *
 * The comments are replaced by spaces as we assume that normalize_spaces()
 * is run after this function which tidies this up anyway - and we save a
 * potential realloc().
 */
int remove_comments(struct preproc *pp)
{
	struct line *l;
	unsigned int lineno;
	int in_comment = 0;

	pp_for_each_lineno(pp, l, lineno) {
		char *p;

		for (p = l->line; *p; p++) {
			if (in_comment && *p == '*' && *(p + 1) == '/') {
				*p++ = ' '; *p = ' ';
				in_comment = 0;
				continue;
			}
			if (*p == '/' && *(p + 1) == '*') {
				*p++ = ' '; *p = ' ';
				in_comment = 1;
				continue;
			}
			if (in_comment)
				*p = ' ';
		}
	}
	if (in_comment) {
		warn(lineno - 1, "unterminated comment");
		return -2;
	}
	return 0;
}

/**
 * normalize_spaces() - Remove unnecessary spaces
 * @pp: struct preproc
 *
 * We must be careful about what we do here. The goal is to make our lifes
 * easier when parsing and substituting but we also want the programmer to
 * still be able to read preprocessor output. Therefore:
 *  - replace '\t' by ' ',
 *  - remove trailing spaces, for reasons of morality and to clear lines
 *    completely made out of spaces,
 *  - contract multiple spaces to a single one,
 *     - except at the beginning of a line because then its indentation
 *       which we ought to keep for the programmer.
 */
int normalize_spaces(struct preproc *pp)
{
	struct line *l;
	char *tmp;

	pp_for_each_line(pp, l) {
		char *p = l->line, *end;
		size_t newlen;

		if (!l->len)
			continue;

		/*
		 * Replace tabs by spaces
		 */
		while ((p = strchr(p, '\t')))
			*p = ' ' ;

		/*
		 * Remove trailing spaces
		 */
		p = l->line + l->len - 1;
		while (*p == ' ' && p >= l->line)
			*p-- = '\0';
		/* Save this for later because we want _at most one_
		 * realloc() in each iteration */
		end = p + 1;
		/* Was all spaces? */
		if (end == l->line) {
			tmp = l->line;
			l->line = realloc(l->line, 1);
			if (!l->line)
				goto error;
			l->len = 0;
			continue;
		}

		/*
		 * Contract spaces
		 */
		p = l->line;
		while (*p == ' ')
			p++;
		/* Now, contract spaces from `p' on: if we find two adjacent
		 * spaces, make the first one a NUL byte, then continue the
		 * search. When we fill the NUL holes later, we can use
		 * l->len to see when we're at the real end of the line. */
		newlen = l->len;
		for (; *(p + 1); p++) { /* If we're here, *p != '\0' */
			if (*p == ' ' && *(p + 1) == ' ') {
				*p = '\0';
				newlen--;
			}
		}

		/* Finally, shrink the line for all the above changes at
		 * once */
		if (newlen != l->len) {
			char *q;

			tmp = l->line;
			l->line = realloc(l->line, newlen + 1);
			if (!l->line)
				goto error;
			for (q = p = l->line; p < end; p++)
				if (*p)
					*q++ = *p;
			*q = '\0';
			l->len = newlen;
		}
	}
	return 0;

error:
	l->line = tmp;
	return -1;
}

/**
 * __define_to_subst() - Create a struct pp_subst out of a #define directive
 * @ps: struct pp_subst to fill
 * @l:  line containing the #define
 *
 * EXAMPLE:
 * "#define a(b, c) b+c"
 * will be transformed into
 * "#subst a\(([^,]+),[ ]?([^)])\) \1+\2"
 * which is essentially the same.
 *
 * BUGS:
 * String literals containing comma characters break the regular expressions
 * ATM. Fortunately, P doesn't have strings anyway :-) (Of course, we need
 * to fix this, still).
 */
static int __define_to_subst(struct pp_subst *ps, const struct line *l)
{
	ps->next = NULL;
	ps->is_define = 1;

	/* TODO */

	return 0;
}

static inline int __is_directive(const char *s, const char *directive)
{
	return !strcmp(s, directive);
}

/**
 * parse_lines() - Read directives from pp->lines
 * @pp: struct preproc
 *
 * This function assumes that every directive can be read from the single
 * where it started and that there are only spaces, if any, before the
 * beginning of the directive, i.e. contract_lines() and normalize_spaces()
 * were run before.
 *
 * Currently supported directives are:
 *  - #define,
 *  - #subst.
 *
 * If PP_NO_SUBST is set in pp->flags, then no entry in pp->substs is made,
 * so that subsequently invoked functions don't know that they were present.
 *
 *
 */
int parse_lines(struct preproc *pp)
{
	struct line *l;
	int lineno;
	int disable_subst = !!(pp->flags & PP_NO_SUBST);

	pp_for_each_lineno(pp, l, lineno) {
		char *p = l->line;
		struct pp_subst *ps;

		while (*p == ' ')
			p++;
		if (*p != '#')
			continue;
		while (*p == ' ')
			p++;

		if (__is_directive(p, "define")) {
			ps = malloc(sizeof(*ps));
			if (!ps)
				return -1;

			__define_to_subst();
			/* TODO */
		} else if (__is_directive(p, "subst")) {
			/*
			 * FIXME: Should we add a flag to disable this kind
			 * of warning? They turned #subst off explicitly and
			 * likely know what they're doing...
			 */
			if (disable_subst) {
				warn(lineno, "#subst directive ignored");
				continue;
			}
			ps = malloc(sizeof(*ps))
			if (!ps)
				return -1;
			/* TODO */
		}
	}
	return 0;
}

/**
 * read_source() - Read a source file into a struct preproc
 * @fp:    input file
 * @flags: any legal combination of enum pp_flags
 *
 * This function only initialises pp->lines and pp->flags. The pp->lines
 * will not contain line delimiters.
 */
struct preproc *read_source(FILE *fp, enum pp_flags flags)
{
	struct preproc *pp = malloc(sizeof(*pp));
	int use_crlf = !!(flags & PP_CRLF), delim;
	char *line = NULL;
	size_t size, len;
	struct line *new, *last;

	if (!pp)
		goto error;
	memset(pp, 0, sizeof(*pp));
	pp->flags = flags;

	if (use_crlf)
		delim = CRLF;
	else if (pp->flags & PP_CR)
		delim = '\r';
	else
		delim = '\n';
	/* Isn't C just great? This saves us a branch prediction nightmare
	 * in the loop */
	last = container_of(&pp->lines, struct line, next);
	while ((len = getdelim(&line, &size, delim, fp)) != -1) {
		new = malloc(sizeof(*new));
		if (!new)
			goto error_free;
		new->line = line;
		/* FIXME: line may not contain the delimiters actually.
		 * Handle messed up source files here (without trailing
		 * newline at every line)... */
		new->len = len - (use_crlf ? 2 : 1);
		new->line[new->len] = '\0';
		/* Just to have them all zeroed out in each case. */
		new->line[new->len + 1] = '\0';

		new->next = NULL;
		last->next = new;
		last = new;

		line = NULL;
	}
	if (!feof(fp))
		goto error_free;

	return pp;

error_free:
	last = pp->lines;
	while (last) {
		free(last->line);
		new = last->next;
		free(last);
		last = new;
	}
	free(pp);
error:
	return NULL;
}

/**
 * write_source() - Write preprocessed source to a file
 * @pp: struct preproc
 * @fp: file to write to
 */
int write_source(const struct preproc *pp, FILE *fp)
{
	struct line *l = pp->lines;
	char delim[3];

	if (pp->flags & PP_CRLF) {
		delim[0] = '\r'; delim[1] = '\n'; delim[2] = '\0';
	} else if (pp->flags & PP_CR) {
		delim[0] = '\r'; delim[1] = '\0';
	} else {
		delim[0] = '\n'; delim[1] = '\0';
	}

	while (l) {
		if (fprintf(fp, "%s%s", l->line, delim) < 0)
			return -1;
		l = l->next;
	}
	return 0;
}

/**
 * free_preproc() - Deallocate a struct preproc
 * @pp: struct preproc
 */
void free_preproc(struct preproc *pp)
{
	struct pp_define *d = pp->defines;
	struct pp_subst *s = pp->substs;
	struct line *l = pp->lines;
	void *tmp;

	while (d) {
		tmp = d->next;
		free(d);
		d = tmp;
	}
	while (s) {
		tmp = s->next;
		free(s);
		s = tmp;
	}
	while (l) {
		tmp = l->next;
		free(l->line);
		free(l);
		l = tmp;
	}
	/* Help them debug :-) */
	memset(pp, 0, sizeof(*pp));
	pp->flags = PP_POISON;
	free(pp);
}

/**
 * __substitute() - One round of substitution
 * @pp: struct preproc
 *
 * This function returns the number of substitutions made or a negative
 * number on error.
 */
static long __substitute(struct preproc *pp)
{
	;
}

/**
 * substitute() - Do the #define and #subst directives
 * @pp: struct preproc
 */
int substitute(struct preproc *pp)
{
	long res;

	do
		res = __substitute(pp);
	while (res > 0);
	return res;
}

/**
 * apply_directives() - Apply all the recognised directives
 * @pp: struct preproc
 */
int apply_directives(struct preproc *pp)
{
	return substitute(pp);
}

/**
 * preprocess() - Normal preprocessor control flow all-in-one
 * @in:    input file
 * @out:   output file
 * @flags: any legal combination of enum pp_flags
 */
int preprocess(FILE *in, FILE *out, enum pp_flags flags)
{
	struct preproc *pp;

	if (flags & PP_UNIFDEF)
		return unifdef(in, out, flags);
	pp = read_source(in, flags);
	if (!pp)
		return -1;

	if (contract_lines(pp) || remove_comments(pp)
	 || normalize_spaces(pp) || parse_lines(pp)
	 || apply_directives(pp) || write_source(pp, out)) {
		free_preproc(pp);
		return -1;
	}

	free_preproc(pp);
	return 0;
}

/**
 * unifdef() - 
 * @in:    input file
 * @out:   output file
 * @flags: any legal combination of enum pp_flags
 */
int unifdef(FILE *in, FILE *out, enum pp_flags flags)
{
	return 0;
}
