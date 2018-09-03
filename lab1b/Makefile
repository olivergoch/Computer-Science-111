#Oliver Goch
#123456789
#the.oliver.goch@gmail.com

CC = gcc
CFLAGS = -Wall -Wextra -lz

default: lab1b-server lab1b-client

lab1b-client: lab1b-client.c
	$(CC) $(CFLAGS) lab1b-client.c -o $@

lab1b-server: lab1b-server.c
	$(CC) $(CFLAGS) lab1b-server.c -o $@

clean:
	rm -f lab1b-client lab1b-server lab1b-123456789.tar.gz

dist: lab1b-123456789.tar.gz
dist_files = lab1b-client.c lab1b-server.c Makefile README
lab1b-123456789.tar.gz: $(dist_files)
	tar -czf $@ $(dist_files)
