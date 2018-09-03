#Oliver Goch
#123456789
#the.oliver.goch@gmail.com

CC = gcc
CFLAGS = -g -Wall -Wextra -pthread

default: build

build: lab2_list

graphs: tests
	rm -rf *.png
	gnuplot lab2_list.gp

lab2_list: lab2_list.c
	$(CC) $(CFLAGS) lab2_list.c SortedList.c -o $@	

dist: graphs lab2b-123456789.tar.gz 
lab2b-123456789.tar.gz:
	tar -czf $@ profile.out lab2_list.c SortedList.h SortedList.c Makefile README lab2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2_list.gp

tests: build
	rm -rf *.csv
	#normal tests
	./lab2_list --sync=m --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --iterations=1000 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --iterations=1000 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=m --iterations=1000 --threads=24 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=s --iterations=1000 --threads=24 >> lab2b_list.csv
	#tests that fail
	-./lab2_list --yield=id --lists=4 --threads=1 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=1 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=4 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=8 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=12 --iterations=16 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=1 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=2 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=4 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=8 >> lab2b_list.csv
	-./lab2_list --yield=id --lists=4 --threads=16 --iterations=16 >> lab2b_list.csv
	#tests with lists that succeed
	./lab2_list --sync=m --yield=id --lists=4 --iterations=10 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=20 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=40 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=80 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=10 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=20 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=40 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=80 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=10 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=20 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=40 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=80 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=10 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=20 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=40 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=80 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=10 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=20 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=40 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=80 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=10 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=20 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=40 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=m --yield=id --lists=4 --iterations=80 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=10 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=20 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=40 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=80 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=10 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=20 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=40 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=80 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=10 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=20 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=40 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=80 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=10 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=20 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=40 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=80 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=10 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=20 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=40 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=80 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=10 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=20 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=40 --threads=16 >> lab2b_list.csv
	./lab2_list --sync=s --yield=id --lists=4 --iterations=80 --threads=16 >> lab2b_list.csv
	#mutex
	./lab2_list --sync=m --lists=4 --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --lists=8 --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --lists=16 --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=m --lists=4 --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --lists=8 --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --lists=16 --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=m --lists=4 --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --lists=8 --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --lists=16 --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=m --lists=4 --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --lists=8 --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --lists=16 --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=m --lists=4 --iterations=1000 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --lists=8 --iterations=1000 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=m --lists=16 --iterations=1000 --threads=12 >> lab2b_list.csv
	#spin
	./lab2_list --sync=s --lists=4 --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --lists=8 --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --lists=16 --iterations=1000 --threads=1 >> lab2b_list.csv
	./lab2_list --sync=s --lists=4 --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --lists=8 --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --lists=16 --iterations=1000 --threads=2 >> lab2b_list.csv
	./lab2_list --sync=s --lists=4 --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --lists=8 --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --lists=16 --iterations=1000 --threads=4 >> lab2b_list.csv
	./lab2_list --sync=s --lists=4 --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --lists=8 --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --lists=16 --iterations=1000 --threads=8 >> lab2b_list.csv
	./lab2_list --sync=s --lists=4 --iterations=1000 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --lists=8 --iterations=1000 --threads=12 >> lab2b_list.csv
	./lab2_list --sync=s --lists=16 --iterations=1000 --threads=12 >> lab2b_list.csv
	
profile: lab2_list
	-rm -f ./raw
	LD_PRELOAD=/usr/lib64/libprofiler.so.0 CPUPROFILE=./raw ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text ./lab2_list ./raw > profile.out
	pprof --list=listThread ./lab2_list ./raw >> profile.out
	rm -f ./raw

clean:
	rm -f lab2_list lab2b-123456789.tar.gz
