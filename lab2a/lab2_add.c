/*Oliver Goch
the.oliver.goch@gmail.com
123456789*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sched.h>

//global variables
int iNum = 1;
int opt_yield = 0;
pthread_mutex_t addMutex;
int spinLock = 0;
//flags
int mutexFlag = 0;
int spinFlag = 0;
int compareFlag = 0;

void add(long long *pointer, long long value)
{
	long long sum = *pointer + value;
	if(opt_yield)
		sched_yield();
	*pointer = sum;
}

void mutexAdd(long long *pointer, long long value)
{
	pthread_mutex_lock(&addMutex);
	add(pointer, value);
	pthread_mutex_unlock(&addMutex);
}

void spinAdd(long long *pointer, long long value)
{
	while(__sync_lock_test_and_set(&spinLock, 1) == 1)
	{}
	add(pointer, value);
	__sync_lock_release(&spinLock);
}

void compareAdd(long long *pointer, long long value)
{
	long long oldValue, sum;
	do
	{
		oldValue = *pointer;
		sum = oldValue + value;
		if(opt_yield)
			sched_yield();
	}while(oldValue != __sync_val_compare_and_swap(pointer, oldValue, sum));
}

//have to take in void arg for thread function
void *addThread(void *counter)
{
	int i;
	//have to cast back to long long
	for(i=0; i< iNum; i++)
	{
		if(mutexFlag)
			mutexAdd((long long *) counter, 1);
		else if(spinFlag)
			spinAdd((long long *) counter, 1);
		else if(compareFlag)
			compareAdd((long long *) counter, 1);
		else
			add((long long *) counter, 1);
	}
	for(i=0; i< iNum; i++)
	{
		if(mutexFlag)
			mutexAdd((long long *) counter, -1);
		else if(spinFlag)
			spinAdd((long long *) counter, -1);
		else if(compareFlag)
			compareAdd((long long *) counter, -1);
		else	
			add((long long *) counter, -1);
	}

	return NULL;
}

int main(int argc, char **argv)
{
	static struct option longopts[] = {
    { "threads=", required_argument, NULL, 't'},
    { "iterations=", required_argument, NULL, 'i'},
    { "yield", no_argument, NULL, 'y'},
    { "sync=", required_argument, NULL, 's'},
    {0, 0, 0, 0}
	};
	//Set default values for number of threads and number of iterations
	int c, tNum = 1;
	//Getopt stuff
	while((c = getopt_long(argc, argv, "t:i:ys:", longopts, NULL)) != -1)
	{
		switch(c)
		{
			case 't':
			tNum = atoi(optarg);
			if(tNum <= 0)
			{
				fprintf(stderr, "Did not give valid number of threads\n");
				exit(1);
			}
			break;
			case 'i':
			iNum = atoi(optarg);
			if(iNum <= 0)
			{
				fprintf(stderr, "Did not give valid number of iterations\n");
				exit(1);
			}
			break;
			case 'y':
			opt_yield = 1;
			break;
			case 's':
			if(strcmp(optarg, "m") == 0)
			{
				mutexFlag = 1;
				pthread_mutex_init(&addMutex, NULL);
			}
			else if(strcmp(optarg, "s") == 0)
				spinFlag = 1;
			else if(strcmp(optarg, "c") == 0)
				compareFlag = 1;
			else
			{
				fprintf(stderr, "Did not give valid locking procedure or too many arguments\n");
				exit(1);
			}
			break;
			default:
			exit(1);
			break;
		}
	}
	//initialize counter
	long long counter = 0;
	//initialize clock
	struct timespec startTimer, endTimer;
	if(clock_gettime(CLOCK_MONOTONIC, &startTimer) < 0)
	{
		int clockError = errno;
		fprintf(stderr, "Error on clock_gettime:\n");
		fprintf(stderr, strerror(clockError));
      	fprintf(stderr, "\n");
      	exit(1);
	}
	//allocate enough threads
	pthread_t *tids = malloc(tNum*sizeof(pthread_t));
	int i = 0;
	//create the threads
	for(i = 0; i < tNum; i++)
	{
		if(pthread_create(&tids[i], NULL, addThread, &counter) != 0)
		{
			int createError = errno;
			fprintf(stderr, "Error on pthread_create:\n");
			fprintf(stderr, strerror(createError));
	      	fprintf(stderr, "\n");
	      	exit(1);
	  	}

	}
	//join the threads
	for(i = 0; i < tNum; i++)
	{
		if(pthread_join(tids[i], NULL) != 0)
		{
			int joinError = errno;
			fprintf(stderr, "Error on pthread_join:\n");
			fprintf(stderr, strerror(joinError));
	      	fprintf(stderr, "\n");
	      	exit(1);
	  	}
	}
	//get end time
	if(clock_gettime(CLOCK_MONOTONIC, &endTimer) < 0)
	{
		int clockError = errno;
		fprintf(stderr, "Error on clock_gettime:\n");
		fprintf(stderr, strerror(clockError));
      	fprintf(stderr, "\n");
      	exit(1);
	}
	//get final results
	free(tids);
	int opNum = tNum * iNum * 2;
	long runTime = endTimer.tv_sec - startTimer.tv_sec;
	long nRunTime = runTime * 1000000000L + (endTimer.tv_nsec - startTimer.tv_nsec);
	long avgOpTime = nRunTime/opNum;
	//print out all the results
	printf("add");
	if(opt_yield)
		printf("-yield");
	if(mutexFlag)
		printf("-m,");
	else if(spinFlag)
		printf("-s,");
	else if(compareFlag)
		printf("-c,");
	else
		printf("-none,");
	printf("%d,", tNum);
	printf("%d,", iNum);
	printf("%d,", opNum);
	printf("%li,", nRunTime);
	printf("%li,", avgOpTime);
	printf("%llu\n", counter);
	return 0;
}
