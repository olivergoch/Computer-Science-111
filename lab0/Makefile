#Oliver Goch
#123456789
#the.oliver.goch@gmail.com

CC = gcc
CFLAGS = -Wall -Wextra

default: lab0

lab0: lab0.c
	$(CC) $(CFLAGS) lab0.c -o $@

check:
	./smoketest.sh

dist: lab0-123456789.tar.gz
dist_files = lab0.c Makefile backtrace.png breakpoint.png smoketest.sh README
lab0-123456789.tar.gz: $(dist_files)
	tar -czf $@ $(dist_files)

clean:
	rm -f lab0 lab0-123456789.tar.gz
