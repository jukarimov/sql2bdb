/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 2011 Oracle and/or its affiliates.  All rights reserved.
 *
 * $Id$
 * -- stolen from db's examples 8)
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
extern int getopt(int, char * const *, const char *);
#else
#include <unistd.h>
#endif

#include <db.h>

#define	DATABASE	"access.db"
int main __P((int, char *[]));
int usage __P((void));

int getln(char line[])
{
	int c, i = 0;
	while ((c=getchar()) != EOF) {
		if (c == '\n') {
			line[i] = 0;
			return i;
		}
		line[i++] = c;
	}

	return -1;
}


int
main(argc, argv)
	int argc;
	char *argv[];
{
	extern int optind;
	DB *dbp;
	DBT key, data;
	int ch, ret, rflag;
	char *database;
	const char *progname = "ex_access";		/* Program name. */

	rflag = 0;
	while ((ch = getopt(argc, argv, "r")) != EOF)
		switch (ch) {
		case 'r':
			rflag = 1;
			break;
		case '?':
		default:
			return (usage());
		}
	argc -= optind;
	argv += optind;

	/* Accept optional database name. */
	database = *argv == NULL ? DATABASE : argv[0];

	/* Optionally discard the database. */
	if (rflag)
		(void)remove(database);

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
	if ((ret = dbp->open(dbp,
	    NULL, database, NULL, DB_BTREE, DB_CREATE, 0664)) != 0) {
		dbp->err(dbp, ret, "%s: open", database);
		goto err1;
	}

	/*
	 * Insert records into the database, where the key is extracted from
	 * first field and the data from second.
	 */

	memset(&key, 0, sizeof(DBT));
	memset(&data, 0, sizeof(DBT));
	/*=================================================================*/

	printf(">>> Waiting on SQL input <<<\n");

	char line[100 * 1000], k[10 * 1000], v[90 * 1000];
	int len, i, j, c = 0, klen, vlen;
	while ((len = getln(line)) != -1) {
		if (strstr(line, "INSERT")) {
			i = 0;
			while (line[i++] != '\'' && i < len)
				;
			j = 0;
			while (i < len && line[i] != '\'')
				k[j++] = line[i++];
			k[j] = 0;
			klen = j;

			i++;
			while (i < len && line[i] != '\'')
				i++;
			i++;

			j = 0;
			while (i < len && line[i] != '\'')
				v[j++] = line[i++];

			v[j] = 0;
			vlen = j;

		} else {
			printf("Expected INSERT, got %s\n", line);
			goto err1;
		}

		key.data = k;
		key.size = klen;

		data.data = v;
		data.size = vlen;

		switch (ret =
		    dbp->put(dbp, NULL, &key, &data, DB_NOOVERWRITE)) {
		case 0:
			break;
		default:
			dbp->err(dbp, ret, "DB->put");
			if (ret != DB_KEYEXIST)
				goto err1;
			break;
		}

		//printf("%s:%s\n", key.data, data.data);
		printf("inserted %d k/v pairs\n", ++c);
	}

	
	/*=================================================================*/


	dbp->close(dbp, 0);

	return 0;

err1:
	return EXIT_FAILURE;
}

int
usage()
{
	(void)fprintf(stderr, "usage: ex_access [-r] [database]\n");
	return (EXIT_FAILURE);
}
