#Oliver Goch
#123456789
#the.oliver.goch@gmail.com

CC = gcc
CFLAGS = -Wall -Wextra -pthread

default: build

build: lab2_add lab2_list

graphs: clean tests
	export PATH=/usr/local/cs/bin:$PATH
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

lab2_add: lab2_add.c
	$(CC) $(CFLAGS) lab2_add.c -o $@

lab2_list: lab2_list.c
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o $@	

dist: graphs lab2a-123456789.tar.gz 
dist_files = lab2_list.c lab2_add.c SortedList.h SortedList.c Makefile README lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png  lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png lab2_add.gp lab2_list.gp 
lab2a-123456789.tar.gz: $(dist_files)
	tar -czf $@ $(dist_files)

tests: build
	#normal
	./lab2_add --threads=2 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000 >> lab2_add.csv
	#with yield
	./lab2_add --threads=2 --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=20 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=40 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=80 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000 --yield >> lab2_add.csv
	#time tests
	./lab2_add --threads=2 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000000 --yield >> lab2_add.csv
	#single thread
	./lab2_add --threads=1 --iterations=100 >> lab2_add.csv
	./lab2_add --threads=1 --iterations=1000 >> lab2_add.csv
	./lab2_add --threads=1 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=1 --iterations=100000 >> lab2_add.csv
	#yield lock vs no lock
	./lab2_add --threads=2 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=4 --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=100000 --yield >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000000 --yield >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=m >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --yield --sync=c >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=8 --iterations=1000 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=1000 --yield --sync=s >> lab2_add.csv
	#no yield lock vs no lock
	./lab2_add --threads=1 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=1 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=1 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=c >> lab2_add.csv
	./lab2_add --threads=1 --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=2 --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=4 --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=8 --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=12 --iterations=10000 --sync=s >> lab2_add.csv
	#single thread list
	./lab2_list --threads=1 --iterations=10 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=100 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=10000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=20000 >> lab2_list.csv
	#multithreaded, basic
	-./lab2_list --threads=2 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1000 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=10 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=100 >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000 >> lab2_list.csv
	#multithreaded, yield
	-./lab2_list --threads=2 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=i >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=i >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=d >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=d >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=il >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2 --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4 --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8 --iterations=32 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16 --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=32 --yield=dl >> lab2_list.csv
	#guarantee problems eliminated with locks
	./lab2_list --threads=12 --iterations=32 --yield=i --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=i --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=d --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv
	#time tests
	./lab2_list --threads=1 --iterations=1000 >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=1 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=2 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=4 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=8 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=12 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=16 --iterations=1000 --sync=s >> lab2_list.csv
	./lab2_list --threads=24 --iterations=1000 --sync=s >> lab2_list.csv


clean:
	rm -f lab2_add lab2_list lab2a-123456789.tar.gz #lab2_add.csv lab2_list.csv *.png
