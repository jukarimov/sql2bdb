/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 2011 Oracle and/or its affiliates.  All rights reserved.
 *
 * $Id$
 */

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise.h"

#ifdef _WIN32
extern int getopt(int, char * const *, const char *);
#else
#include <unistd.h>
#endif

#include <db.h>

#define	DATABASE	"/home/user/MyDocs/oxf.db"
int main __P((int, char *[]));
int usage __P((void));


#define save_time(t, tv) do {		\
	gettimeofday(&tv, NULL);	\
	t = tv.tv_sec * 1000;		\
	t += tv.tv_usec / 1000;		\
} while (0)


int
main(argc, argv)
	int argc;
	char *argv[];
{
	DB *dbp;
	DBC *dbcp;
	DBT key, data;
	int ret;
	char *database = DATABASE;
	const char *progname = "ex_access";		/* Program name. */

	int batch = 0;
	if (argc > 1)
		batch = 1;

	/* Create and initialize database object, open the database. */
	if ((ret = db_create(&dbp, NULL, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		return (EXIT_FAILURE);
	}
	dbp->set_errfile(dbp, stderr);
	dbp->set_errpfx(dbp, progname);
	if ((ret = dbp->set_pagesize(dbp, 1024)) != 0) {
		dbp->err(dbp, ret, "set_pagesize");
		goto err1;
	}
	if ((ret = dbp->set_cachesize(dbp, 0, 32 * 1024, 0)) != 0) {
		dbp->err(dbp, ret, "set_cachesize");
		goto err1;
	}
	if ((ret = dbp->open(dbp, NULL, database, NULL, DB_BTREE, DB_RDONLY, 0644)) != 0) {
		dbp->err(dbp, ret, "%s: open", database);
		goto err1;
	}

	/* Acquire a cursor for the database. */
	if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0) {
		dbp->err(dbp, ret, "DB->cursor");
		goto err1;
	}

	long long tm_start, tm_end;
	struct timeval tv;
	long long delta_ms; 
	double secs;

	int found = 0;
	char *line;
	linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

	if (batch)
		goto look;
	while((line = linenoise("lookup:> ")) != NULL) {
		if (line[0] != '\0') {

look:
			save_time(tm_start, tv);

			if (batch)
				line = argv[--argc];

			found = 0;
			memset(&key, 0, sizeof(key));
			memset(&data, 0, sizeof(data));

			key.data = line;
			key.size = strlen(line);

			/* search for data of the given key */
			if ((ret = dbcp->get(dbcp, &key, &data, DB_SET)) == 0)
				found = 1;

			save_time(tm_end, tv);

			/* time offset in milliseconds */
			delta_ms = tm_end - tm_start;
			secs = (double)delta_ms / 1000.0f;
			printf("[result in %f secs]\n", secs);

			puts("\n<br>-------8<----->8-------<br>\n");
			if (found) {
				printf("%.*s : %.*s\n",
				    (int)key.size, (char *)key.data,
				    (int)data.size, (char *)data.data);
			} else
				printf("[nothing for: %s]\n", (char *)key.data);
			puts("\n<br>-------8<----->8-------<br>\n");

			if (batch && argc == 1)
				break;

			linenoiseHistoryAdd(line);
			linenoiseHistorySave("history.txt"); /* Save every new entry */
		}
	}

	if (ret != DB_NOTFOUND && ret) {
		dbp->err(dbp, ret, "DBcursor->get");
		goto err2;
	}

	/* Close everything down. */
	if ((ret = dbcp->close(dbcp)) != 0) {
		dbp->err(dbp, ret, "DBcursor->close");
		goto err1;
	}

	if ((ret = dbp->close(dbp, 0)) != 0) {
		fprintf(stderr,
		    "%s: DB->close: %s\n", progname, db_strerror(ret));
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);

err2:	
	
	(void)dbcp->close(dbcp);
err1:	

	(void)dbp->close(dbp, 0);

	puts("something went wrong");

	return (EXIT_FAILURE);
}

