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
#include "SortedList.h"
#include <signal.h>

//global variables
int iNum = 1;
int tNum = 1;
int lNum = 1;
int opt_yield = 0;
int spinFlag = 0;
int mutexFlag = 0;
pthread_mutex_t* listMutexes;
int* spinLocks;
long lockAcqTime = 0;
int* numInList;
int numList;
//Sorted List
SortedList_t* lists;
SortedListElement_t* listElements;

void sighandler(int signum)
{
  fprintf(stderr, "Caught segmentation fault, list is corrupted\n");
  fprintf(stderr, strerror(signum));
  fprintf(stderr, "\n");
  exit(2);
}

void clockError(int numErr)
{
	int clockError = numErr;
	fprintf(stderr, "Error on clock_gettime:\n");
	fprintf(stderr, strerror(clockError));
 	fprintf(stderr, "\n");
  	exit(1);
}

void* listThread(void* beginning)
{
	int i;
	struct timespec startLockTime, endLockTime;
	//insert all the elements
	//printf("about to insert\n");
	for(i = *(int*)beginning; i < numList; i += tNum)
	{
		if(mutexFlag)
		{
			if(clock_gettime(CLOCK_MONOTONIC, &startLockTime) < 0)
				clockError(errno);
			//printf("About to lock\n");
			pthread_mutex_lock(&listMutexes[numInList[i]]);
			//printf("Locked\n");
			if(clock_gettime(CLOCK_MONOTONIC, &endLockTime) < 0)
				clockError(errno);
			long lockTime = endLockTime.tv_sec - startLockTime.tv_sec;
			long nLockTime = lockTime * 1000000000L + (endLockTime.tv_nsec - startLockTime.tv_nsec);
			lockAcqTime += nLockTime;

			SortedList_insert(&lists[numInList[i]], &listElements[i]);
			//printf("About to unlock\n");
			pthread_mutex_unlock(&listMutexes[numInList[i]]);
			//printf("unlock\n");			
		}
		else if(spinFlag)
		{
			if(clock_gettime(CLOCK_MONOTONIC, &startLockTime) < 0)
				clockError(errno);

			while(__sync_lock_test_and_set(&spinLocks[numInList[i]], 1) == 1)
			{}

			if(clock_gettime(CLOCK_MONOTONIC, &endLockTime) < 0)
				clockError(errno);
			long lockTime = endLockTime.tv_sec - startLockTime.tv_sec;
			long nLockTime = lockTime * 1000000000L + (endLockTime.tv_nsec - startLockTime.tv_nsec);
			lockAcqTime += nLockTime;

			SortedList_insert(&lists[numInList[i]], &listElements[i]);

			__sync_lock_release(&spinLocks[numInList[i]]);	
		}
		else
			SortedList_insert(&lists[numInList[i]], &listElements[i]);
	}
	//get length of each sublist and add it to list
	//printf("about to get length\n");
	int length = 0;
	for(i = 0; i < lNum; i++)
	{
		if(mutexFlag)
		{
			
				if(clock_gettime(CLOCK_MONOTONIC, &startLockTime) < 0)
						clockError(errno);

				pthread_mutex_lock(&listMutexes[i]);

				if(clock_gettime(CLOCK_MONOTONIC, &endLockTime) < 0)
						clockError(errno);
				long lockTime = endLockTime.tv_sec - startLockTime.tv_sec;
				long nLockTime = lockTime * 1000000000L + (endLockTime.tv_nsec - startLockTime.tv_nsec);
				lockAcqTime += nLockTime;

				int copy = SortedList_length(&lists[i]);
				if(copy == -1)
				{
					fprintf(stderr, "Error on length: list is corrupted\n");
					exit(2);
				}
				length += copy;
				pthread_mutex_unlock(&listMutexes[i]);
		}
		else if(spinFlag)
		{
				if(clock_gettime(CLOCK_MONOTONIC, &startLockTime) < 0)
						clockError(errno);

				while(__sync_lock_test_and_set(&spinLocks[i], 1) == 1)
				{}

				if(clock_gettime(CLOCK_MONOTONIC, &endLockTime) < 0)
						clockError(errno);
				long lockTime = endLockTime.tv_sec - startLockTime.tv_sec;
				long nLockTime = lockTime * 1000000000L + (endLockTime.tv_nsec - startLockTime.tv_nsec);
				lockAcqTime += nLockTime;

				int copy = SortedList_length(&lists[i]);
				if(copy == -1)
				{
					fprintf(stderr, "Error on length: list is corrupted\n");
					exit(2);
				}
				length += copy;
				__sync_lock_release(&spinLocks[i]);
		}
		else
		{
				int copy = SortedList_length(&lists[i]);
				if(copy == -1)
				{
					fprintf(stderr, "Error on length: list is corrupted\n");
					exit(2);
				}
				length += copy;
		}
	}
	//lookup and delete every element
	//printf("about to delete\n");
	SortedListElement_t* foundElement;
	int deleted;
	for(i = *(int*)beginning; i < numList; i += tNum)
	{		
		if(mutexFlag)
		{
			if(clock_gettime(CLOCK_MONOTONIC, &startLockTime) < 0)
				clockError(errno);

			pthread_mutex_lock(&listMutexes[numInList[i]]);

			if(clock_gettime(CLOCK_MONOTONIC, &endLockTime) < 0)
				clockError(errno);
			long lockTime = endLockTime.tv_sec - startLockTime.tv_sec;
			long nLockTime = lockTime * 1000000000L + (endLockTime.tv_nsec - startLockTime.tv_nsec);
			lockAcqTime += nLockTime;

			foundElement = SortedList_lookup(&lists[numInList[i]], listElements[i].key);
			//if the element could not be found
			if(foundElement == NULL)
			{
				fprintf(stderr, "Error on lookup: list is corrupted\n");
				exit(2);
			}
			deleted = SortedList_delete(foundElement);
			if(deleted == 1)
			{
				fprintf(stderr, "Error on delete: list is corrupted\n");
				exit(2);
			}

			pthread_mutex_unlock(&listMutexes[numInList[i]]);

		}
		else if(spinFlag)
		{
			if(clock_gettime(CLOCK_MONOTONIC, &startLockTime) < 0)
				clockError(errno);

			while(__sync_lock_test_and_set(&spinLocks[numInList[i]], 1) == 1)
			{}

			if(clock_gettime(CLOCK_MONOTONIC, &endLockTime) < 0)
				clockError(errno);
			long lockTime = endLockTime.tv_sec - startLockTime.tv_sec;
			long nLockTime = lockTime * 1000000000L + (endLockTime.tv_nsec - startLockTime.tv_nsec);
			lockAcqTime += nLockTime;

			foundElement = SortedList_lookup(&lists[numInList[i]], listElements[i].key);
			//if the element could not be found
			if(foundElement == NULL)
			{
				fprintf(stderr, "Error on lookup: list is corrupted\n");
				exit(2);
			}
			deleted = SortedList_delete(foundElement);
			if(deleted == 1)
			{
				fprintf(stderr, "Error on delete: list is corrupted\n");
				exit(2);
			}
			__sync_lock_release(&spinLocks[numInList[i]]);
		}
		else
		{
			foundElement = SortedList_lookup(&lists[numInList[i]], listElements[i].key);
			//if could not find the element
			if(foundElement == NULL)
			{
				fprintf(stderr, "Error on lookup: list is corrupted\n");
				exit(2);
			}
			deleted = SortedList_delete(foundElement);
			if(deleted == 1)
			{
				fprintf(stderr, "Error on delete: list is corrupted\n");
				exit(2);
			}
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{

	static struct option longopts[] = {
    { "threads=", required_argument, NULL, 't'},
    { "iterations=", required_argument, NULL, 'i'},
    { "yield", required_argument, NULL, 'y'},
    { "sync", required_argument, NULL, 's'},
    { "lists", required_argument, NULL, 'l'},
    {0, 0, 0, 0}
	};

	int c;
	int iFlag = 0, dFlag = 0, lFlag = 0;
	while((c = getopt_long(argc, argv, "t:i:y:s:l:", longopts, NULL)) != -1)
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
			unsigned int i;
			for(i = 0; i < strlen(optarg); i++)
			{
				if(optarg[i] == 'i')
				{
					opt_yield = opt_yield | INSERT_YIELD;
					iFlag = 1;
				}
				else if(optarg[i] == 'd')
				{
				  	opt_yield = opt_yield | DELETE_YIELD;
				  	dFlag = 1;
				}
				else if(optarg[i] == 'l')
				{
				  	opt_yield = opt_yield | LOOKUP_YIELD;
				  	lFlag = 1;
				}
				else 
				{
					fprintf(stderr, "Did not provide correct argument for yield\n");
					exit(1);
				}
			}
			break;
			case 'l':
			lNum = atoi(optarg);
			if(lNum <= 0)
			{
				fprintf(stderr, "Did not give valid number of lists\n");
				exit(1);
			}
			break;
			case 's':
			if(strcmp(optarg, "m") == 0)
				mutexFlag = 1;
			else if(strcmp(optarg, "s") == 0)
				spinFlag = 1;
			else
			{
				fprintf(stderr, "Did not give valid argument for yield or too many arguments\n");
				exit(1);
			}
			break;
			default:
			exit(1);
			break;
		}
	}
	//create locks
	int i;
	if(mutexFlag)
	{
		listMutexes = malloc(lNum*sizeof(pthread_mutex_t));
		for(i = 0; i < lNum; i++)
					pthread_mutex_init(&listMutexes[i], NULL);
	}
	if(spinFlag)
	{
		spinLocks = malloc(lNum*sizeof(int));
		for(i = 0; i < lNum; i++)
					spinLocks[i] = 0;
	}
	//initialize signal handler to catch segmentation faults
    signal(SIGSEGV, sighandler);

	//Create elements
	numList = tNum*iNum;
	//initialize lists, set key to null and set head to point to itself
	lists = malloc(lNum*sizeof(SortedList_t));
	for(i = 0; i < lNum; i++)
	{
		lists[i].key = NULL;
		lists[i].next = &lists[i];
		lists[i].prev = &lists[i];
	}
	//initialize elements
	listElements = malloc(numList*sizeof(SortedListElement_t));
	numInList = malloc(numList*sizeof(int));
	//time to get keys
	srand(time(NULL));
	int len; //set length of key to be 64
	for(i=0; i < numList; i++)
	{
		//length is between 10 and 64
		len = (rand()%54)+10;
		char* key = malloc(len*sizeof(char));
		int j;
		//save last element for null byte
		for(j = 0; j < len-2; j++)
			key[j] = 'a' + rand()%26;
		key[len-1] = '\0';
		//set each list element key equal to random key
		listElements[i].key = key;
		numInList[i] = (listElements[i].key[0]%lNum); 
	}
	
	//allocate enough threads
	pthread_t *tids = malloc(tNum*sizeof(pthread_t));
	int* position = malloc(tNum*sizeof(int));
	//initialzie timers
	struct timespec startTimer, endTimer;
	if(clock_gettime(CLOCK_MONOTONIC, &startTimer) < 0)
	{
		clockError(errno);
	}
	for(i = 0; i < tNum; i++)
	{
		position[i] = i;
		if(pthread_create(&tids[i], NULL, listThread, &position[i]) != 0)
		{
			int createError = errno;
			fprintf(stderr, "Error on pthread_create:\n");
			fprintf(stderr, strerror(createError));
	      	fprintf(stderr, "\n");
	      	exit(1);
	  	}

	}

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

	if(clock_gettime(CLOCK_MONOTONIC, &endTimer) < 0)
	{
		clockError(errno);
	}

	//check if list size in end is 0
	int allListsLength = 0;
	for(i = 0; i < lNum; i++)
		allListsLength += SortedList_length(&lists[i]);

	if(allListsLength != 0)
	{
		fprintf(stderr, "List was not entirely deleted\n");
		exit(2);
	}

	free(lists);
	free(listElements);
	free(tids);
	free(listMutexes);
	//print values
	printf("list-");
	//yield flags
	if(iFlag)
		printf("i");
	if(dFlag)
		printf("d");
	if(lFlag)
		printf("l");
	if(opt_yield == 0)
		printf("none");
	//sync flags
	if(spinFlag)
		printf("-s,");
	else if(mutexFlag)
		printf("-m,");
	else 
		printf("-none,");

	printf("%d,", tNum);
	printf("%d,", iNum);
	printf("%d,", lNum);
	int opNum = tNum*iNum*3;
	printf("%d,", opNum);
	long runTime = endTimer.tv_sec - startTimer.tv_sec;
	long nRunTime = runTime * 1000000000L + (endTimer.tv_nsec - startTimer.tv_nsec);
	printf("%li,", nRunTime);
	long avgOpTime = nRunTime/opNum;
	printf("%li,", avgOpTime);
	long avgLockTime = lockAcqTime/opNum;
	printf("%li\n", avgLockTime);
	return 0;
}
