/*
 *	unicomplex/lib/test.c - Test read_config
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#include <uc/read_config.h>

int main(void)
{
	struct read_config *config = read_config("sample.conf");

	free(config);
	exit(0);
}
