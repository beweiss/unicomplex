/*
 *	unicomplex/lib/tuple.c - Managing tuples
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#include <stdlib.h>
#include <stdarg.h>

#include <openssl/sha.h>

#include <uc/tuple.h>

/*
 * XXX: Should tuples be really created like this? There should at least be
 *      an alternative to only allocate the shell of a tuple and then add
 *      elements with their type dynamically. Not difficult to implement on
 *      top of the structure as-is.
 *
 *      This is mainly an issue because we need a consistent and comfortable
 *      way for P to define tuples and later matrices... The thing with
 *      matrices is even more difficult as they're allowed to be n-dimensional.
 */

/*
 * new_tuple() - Create a new tuple for a number of elements
 * @n  : number of elements
 * @...: P_TYPE_* to specify the data type of each element
 *
 * The values of the elements are undefined.
 */
struct tuple *new_tuple(size_t n, ...)
{
	va_list args;
	struct tuple *tp;
	unsigned int i;

	tp = malloc(sizeof(*tp) + n * sizeof(*tp->e));
	if (!tp)
		return NULL;
	tp->n = n;
	va_start(args, n);
	for (i = 0; i < n; i++)
		tp->e[i].type = va_arg(args, enum p_type);
	va_end(args);
	return tp;
}

/*
 * free_tuple() - Deallocate a tuple
 * @tp: tuple
 */
void free_tuple(struct tuple *tp)
{
	unsigned int i;

	for (i = 0; i < tp->size; i++)
		mpz_clear(&tp->members[i]);
	free(tp);
}

/*
 * get_tuple_hash() - Return the hash of a tuple
 * @tp: tuple
 *
 * This functions returns a pointer to a malloc'd 20 bytes memory region
 * where the SHA1 hash of the tuple is stored.
 *
 * In case of error, the function returns NULL.
 */
char *get_tuple_hash(struct tuple *tp)
{
	SHA_CTX ctx;
	char *hash = malloc(SHA_DIGEST_LENGTH);
	unsigned int i;

	if (!hash)
		return NULL;

	SHA1_Init(&ctx);
	for (i = 0; i < tp->size; i++) {
		SHA1_Update(&ctx, data, len);
	}
	SHA1_Final(hash, &ctx);
	return hash;
}
