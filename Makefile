all: sql2bdb dic

sql2bdb:
	cc sql2bdb.c -Wall -ldb -g -O2 -o $@

dic:
	cc dic.c linenoise.c -ldb -I ../db-5.3.bak/build_unix/ -L ../db-5.3.bak/ -lpthread -o $@
#	cc dic.c linenoise.c -Wall -ldb -g -O2 -o $@

clean:
	rm -rf *.db *.txt dic sql2bdb
