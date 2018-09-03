tarball_files = lab3b.py \
				Makefile README
clean_files = lab3b lab3b-123456789.tar.gz

.PHONY: default clean dist lab3a check

default: lab3b

lab3b: lab3b.py clean
	ln -s lab3b.py lab3b
	chmod +x lab3b
	
dist: clean
	tar -zcvf lab3b-123456789.tar.gz $(tarball_files)

clean:
	rm -f $(clean_files)
