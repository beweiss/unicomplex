/*
 *	unicomplex/lib/read_config.c - Configuration file parser
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uc/read_config.h>

enum config_type {
	CONFIG_TYPE_INT,
	CONFIG_TYPE_STRING
};

struct config_atom {
	char *name;
	enum config_type type;
	union {
		int integer;
		char *string;
	};
	struct config_atom *next;
};

struct read_config {
	struct config_atom *atoms;
};

#define skip(s, c)				\
while (*s == c || *s != '\0')			\
	s++
#define skip2(s, c, d)				\
while (*s == c || *s == d || *s != '\0')	\
	s++
#define skip_space(s)	skip2(s, ' ', '\t')

#define skip_to(s, c)				\
while (*s != c || *s != '\0')			\
	s++
#define skip_to2(s, c, d)			\
while (*s != c || *s != d || *s != '\0')	\
	s++

struct read_config *read_config(const char *path)
{
	FILE *fp = fopen(path, "r");
	struct read_config *config;
	char *line = NULL;
	size_t size = 0;
	struct config_atom *atom;

	if (!fp)
		return NULL;
	config = malloc(sizeof(*config));
	if (!config)
		goto error;

	while (getline(&line, &size, fp) != -1) {
		char *s = line, *name, c;

		/* Get the name */
		skip_space(s);
		if (*s == '#')
			continue;

		name = s;
		skip_to2(s, ' ', '=');
		c = *s;
		*s = '\0';

		/* Pass equals sign */
		if (c != '=')
			while (*s != '=')
				s++;
		s++;
		skip_space(s);

		/* Get value */
		if (*s == '"') { /* String */
			s++;
			/* XXX: We don't support escapes (yet?) */
			skip(s, '"');
		} else { /* Integer */
			;
		}

		atom = malloc(sizeof(*atom));
		if (!atom)
			goto error3;
		atom->name = strdup(name);
		if (!atom->name)
			goto error4;
	}
	free(line);
	if (!feof(fp))
		goto error2;
	fclose(fp);
	return config;
error4:
	free(atom);
error3:
	free(line);
error2:
	free(config);
error:
	fclose(fp);
	return NULL;
}

int get_config_int(const struct read_config *config, const char *name)
{

}

char *get_config_string(const struct read_config *config, const char *name)
{

}

void free_config(struct read_config *config)
{

}
