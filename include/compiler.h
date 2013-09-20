/*
 *	unicomplex/include/compiler.h
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#ifndef COMPILER_H
#define COMPILER_H

/* >:-) */
#ifndef __GNUC__
# error "Yuck! This is not GCC. Use a decent compiler."
#endif

/*
 * The usual stuff
 */
#define __noreturn      __attribute__((noreturn))
#define __export        extern __attribute__((visibility("default")))
#define __hidden        extern __attribute__((visibility("hidden")))
#ifdef __always_inline
# undef __always_inline
#endif
#define __always_inline inline __attribute__((__always_inline__))
#define __used          __attribute__((used))
#define __unused        __attribute__((unused))
#define __align(size)   __attribute__((aligned(size)))
#define __max_align     __align(__BIGGEST_ALIGNMENT__)
#define __printflike(f) __attribute__((format(printf, f, f + 1)))

#endif /* COMPILER_H */
