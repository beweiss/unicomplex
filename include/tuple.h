/*
 *	unicomplex/include/tuple.h
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#ifndef TUPLE_H
#define TUPLE_H

#include <gmp.h>

/*
 * All the native data types in P
 *
 *
 * TODO:
 * Maybe we should divide: p_type_numeric and p_type_object. It will get
 * difficult very soon if we allow tupels to be in tupels (which would be
 * possible with just one p_type (s. b.)), e.g. calculating a tuple hash
 * could lead to infinite recursion (since the contents of a tuple is to be
 * given to the hash function, a tuple inside a tuple would recurse down to
 * _its_ elements):
 *
 * tuple a; a = (a);
 *
 * BUT a prime factorisation would nicely fit into a two-dimensional tuple:
 * x = ((p1, n1), (p2, n2), ...) -> x = p1^n1*p2^n2*...
 *
 * MAYBE we could also add a matrix type for this use case? It would also
 * only be allowed to contain p_type_numeric.
 */
enum p_type {
	P_TYPE_INT,
	P_TYPE_FRAC,
	P_TYPE_FLOAT,
	P_TYPE_BOOL,
	P_TYPE_TUPLE
};

union p_val {
	enum p_type type;	/* Which datatype? */
	mpz_t z;		/* int   */
	mpq_t q;		/* frac  */
	mpf_t f;		/* float */
	uint8_t b;		/* bool  */
	struct tuple *t;	/* tuple */
};

struct tuple {
	size_t n;
	union p_val e[];
};

struct tuple *new_tuple(size_t n, ...);
void free_tuple(struct tuple *tp);
char *get_tuple_hash(struct tuple *tp);

#endif /* TUPLE_H */
