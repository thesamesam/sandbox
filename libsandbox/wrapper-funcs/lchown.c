/*
 * lchown.c
 *
 * lchown() wrapper.
 *
 * Copyright 1999-2008 Gentoo Foundation
 * Licensed under the GPL-2
 *
 *  Partly Copyright (C) 1998-9 Pancrazio `Ezio' de Mauro <p@demauro.net>,
 *  as some of the InstallWatch code was used.
 */

extern int EXTERN_NAME(const char *, uid_t, gid_t);
static int (*WRAPPER_TRUE_NAME) (const char *, uid_t, gid_t) = NULL;

int WRAPPER_NAME(const char *path, uid_t owner, gid_t group)
{
	int result = -1;

	if FUNCTION_SANDBOX_SAFE("lchown", path) {
		check_dlsym(WRAPPER_TRUE_NAME, WRAPPER_SYMNAME,
			    WRAPPER_SYMVER);
		result = WRAPPER_TRUE_NAME(path, owner, group);
	}

	return result;
}