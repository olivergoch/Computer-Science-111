#NAME:Oliver Goch
#EMAIL:the.oliver.goch@gmail.com
#ID:123456789

CC = gcc
CFLAGS = -Wall -Wextra -lmraa -lm

default: lab4c_tcp lab4c_tls

lab4c_tcp: lab4c_tcp.c
	$(CC) $(CFLAGS) lab4c_tcp.c -o $@

lab4c_tls: lab4c_tls.c
	$(CC) $(CFLAGS) lab4c_tls.c -lssl -lcrypto -o $@

dist: lab4c-123456789.tar.gz
dist_files = Makefile README lab4c_tcp.c lab4c_tls.c
lab4c-123456789.tar.gz: $(dist_files)
	tar -czf $@ $(dist_files)

clean:
	rm -f lab4c_tcp lab4c_tls lab4c-123456789.tar.gz
