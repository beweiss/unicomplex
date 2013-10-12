/*
 *	unicomplex/include/read_config.h
 *
 *	(C) 2013 Tobias Boege <tobias@gambas-buch.de>
 */

#ifndef READ_CONFIG_H
#define READ_CONFIG_H

struct read_config;

struct read_config *read_config(const char *path);
int get_config_int(const struct read_config *config, const char *name);
char *get_config_string(const struct read_config *config, const char *name);
void free_config(struct read_config *config);

#endif /* READ_CONFIG_H */
