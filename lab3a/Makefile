CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -g
# CLIBS = -lmraa -lm # -lmraa is for the analog IO library, lm is for the log() function
tarball_files = lab3a.c \
				lab3a_SafeLib.h lab3a_SafeLib.c \
				ext2_fs.h \
				Makefile README
clean_files = lab3a lab3a-123456789.tar.gz

.PHONY: default clean dist lab3a check

default: lab3a

lab3a: lab3a.c
	$(CC) $(CFLAGS) lab3a.c lab3a_SafeLib.c -o lab3a

dist: clean
	tar -zcvf lab3a-123456789.tar.gz $(tarball_files)

clean:
	rm -f $(clean_files)
