#Oliver Goch
#the.oliver.goch@gmail.com
#123456789

CC = gcc
CFLAGS = -Wall -Wextra

default: lab1a

lab1a: lab1a.c
	$(CC) $(CFLAGS) lab1a.c -o $@

clean:
	rm -f lab1a lab1a-123456789.tar.gz

dist: lab1a-123456789.tar.gz
dist_files = lab1a.c Makefile README
lab1a-123456789.tar.gz: $(dist_files)
	tar -czf $@ $(dist_files)
