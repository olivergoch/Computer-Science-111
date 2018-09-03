#! /usr/bin/gnuplot
#
#Oliver Goch
#the.oliver.goch@gmail.com
#123456789
#
# purpose:
#	 generate data reduction graphs for the multi-threaded list project
#
# input: lab2b_list.csv
#	1. test name
#	2. # threads
#	3. # iterations per thread
#	4. # lists
#	5. # operations performed (threads x iterations x (ins + lookup + delete))
#	6. run time (ns)
#	7. run time per operation (ns)
#
# output:
#	lab2_list-1.png ... cost per operation vs threads and iterations
#	lab2_list-2.png ... threads and iterations that run (un-protected) w/o failure
#	lab2_list-3.png ... threads and iterations that run (protected) w/o failure
#	lab2_list-4.png ... cost per operation vs number of threads
#
# Note:
#	Managing data is simplified by keeping all of the results in a single
#	file.  But this means that the individual graphing commands have to
#	grep to select only the data they want.
#
#	Early in your implementation, you will not have data for all of the
#	tests, and the later sections may generate errors for missing data.
#

# general plot parameters
set terminal png
set datafile separator ","

#png1: thread 1-24 total number of ops
set title "List-1: Mutex vs Spin Lock Total Number of Operations"
set xlabel "Number of Threads"
set xrange [0:30]
set ylabel "Length-adjusted Cost Per Operation (ns)"
set logscale y 10
set output 'lab2b_1.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list w/mutex' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000/($7)) \
	title 'list w/spin-lock' with linespoints lc rgb 'green'


#png2: thread 1-24 wait time
set title "List-2: Wait Time vs Completion Time"
set xlabel "Number of Threads"
set xrange [0:30]
set ylabel "Average Time Per Operations (ns)"
set logscale y 10
set output 'lab2b_2.png'
set key left top
plot \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($8) \
	title 'Mutex Lock Time for Lock' with linespoints lc rgb 'blue', \
     "< grep 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):($7) \
	title 'Mutex Lock Total Time' with linespoints lc rgb 'green'

#png3: sublists
set title "List-3: Sublists Synched vs Unsynched"
set xlabel "Number of Threads"
set xrange [0:4]
set ylabel "Successful Iterations"
set logscale y 10
set output 'lab2b_3.png'
set key left top
plot \
   "< grep 'list-id-none' lab2b_list.csv" using (1):($3) \
	title "No Synchronization" with points lc rgb "blue", \
   "< grep 'list-id-s' lab2b_list.csv" using (2):($3) \
	title "Mutex" with points lc rgb "green", \
   "< grep 'list-id-m' lab2b_list.csv" using (3):($3) \
	title "Spin" with points lc rgb "red"


 # unset the kinky x axis
 unset xtics
 set xtics

#png4: mutexes
set title "List-4: Throughput of Mutexes vs Threads"
set xlabel "Number of Threads"
set xrange [0:15]
set ylabel "Aggregated Throughput"
set logscale y 10
set output 'lab2b_4.png'
set key left top
plot \
     "< grep -e 'list-none-m,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '1 List' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-m,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '4 Lists' with linespoints lc rgb 'green', \
    "< grep -e 'list-none-m,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '8 Lists' with linespoints lc rgb 'red', \
    "< grep -e 'list-none-m,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '16 Lists' with linespoints lc rgb 'yellow'

#png5: spin
set title "List-5: Throughput of Spin Locks vs. Threads"
set xlabel "Number of Threads"
set xrange [0:15]
set ylabel " Throughput"
set logscale y 10
set output 'lab2b_5.png'
set key left top
plot \
     "< grep -e 'list-none-s,[0-9]*,1000,1,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '1 List' with linespoints lc rgb 'blue', \
     "< grep -e 'list-none-s,[0-9]*,1000,4,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '4 Lists' with linespoints lc rgb 'green', \
    "< grep -e 'list-none-s,[0-9]*,1000,8,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '8 Lists' with linespoints lc rgb 'red', \
    "< grep -e 'list-none-s,[0-9]*,1000,16,' lab2b_list.csv" using ($2):(1000000000 / ($7)) \
	title '16 Lists' with linespoints lc rgb 'yellow'
