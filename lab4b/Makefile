#NAME:Oliver Goch
#EMAIL:the.oliver.goch@gmail.com
#ID:123456789

CC = gcc
CFLAGS = -Wall -Wextra -lmraa -lm

default: lab4b

lab4b: lab4b.c
	$(CC) $(CFLAGS) lab4b.c -o $@

check: lab4b
	./smoketest.sh

dist: lab4b-123456789.tar.gz
dist_files = lab4b.c Makefile README smoketest.sh
lab4b-123456789.tar.gz: $(dist_files)
	tar -czf $@ $(dist_files)

clean:
	rm -f lab4b lab4b-123456789.tar.gz
