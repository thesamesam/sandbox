/*
 * mkdir*() pre-check.
 *
 * Copyright 1999-2009 Gentoo Foundation
 * Licensed under the GPL-2
 */

bool sb_mkdirat_pre_check(const char *func, const char *pathname, int dirfd)
{
	char canonic[SB_PATH_MAX];

	save_errno();

	/* XXX: need to check pathname with dirfd */
	if (-1 == canonicalize(pathname, canonic))
		/* see comments in check_syscall() */
		if (ENAMETOOLONG != errno) {
			if (is_env_on(ENV_SANDBOX_DEBUG))
				SB_EINFO("EARLY FAIL", "  %s(%s) @ canonicalize: %s\n",
					func, pathname, strerror(errno));
			return false;
		}

	/* XXX: Hack to prevent errors if the directory exist, and are
	 * not writable - we rather return EEXIST than fail.  This can
	 * occur if doing something like `mkdir -p /`.  We certainly do
	 * not want to pass this attempt up to the higher levels as those
	 * will trigger a sandbox violation.
	 */
	struct stat st;
	if (0 == lstat(canonic, &st)) {
		int new_errno;
		if (is_env_on(ENV_SANDBOX_DEBUG))
			SB_EINFO("EARLY FAIL", "  %s(%s[%s]) @ lstat: %s\n",
				func, pathname, canonic, strerror(errno));

		new_errno = EEXIST;

		/* Hmm, is this a broken symlink we're trying to extend ? */
		if (S_ISLNK(st.st_mode) && stat(pathname, &st) != 0) {
			/* XXX: This awful hack should probably be turned into a
			 * common func that does a better job.  For now, we have
			 * enough crap to catch gnulib tests #297026.
			 */
			char *parent = strrchr(pathname, '/');
			if (parent && (strcmp(parent, "/.") == 0 || strcmp(parent, "/..") == 0))
				new_errno = ENOENT;
		}

		errno = new_errno;
		return false;
	}

	restore_errno();

	return true;
}
