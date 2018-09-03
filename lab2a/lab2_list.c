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
int opt_yield = 0;
int spinFlag = 0;
int mutexFlag = 0;
pthread_mutex_t listMutex;
int spinLock = 0;
//Sorted List
SortedList_t* head;
SortedListElement_t* listElements;

void sighandler(int signum)
{
  fprintf(stderr, "Caught segmentation fault, list is corrupted\n");
  fprintf(stderr, strerror(signum));
  fprintf(stderr, "\n");
  exit(2);
}

void *listThread(void* beginning)
{
	SortedListElement_t* beg = beginning;
	int i;
	//insert all the elements

	for(i = 0; i < iNum; i++)
	{
		if(mutexFlag)
		{
			pthread_mutex_lock(&listMutex);
			SortedList_insert(head, beg + i);
			pthread_mutex_unlock(&listMutex);
		}
		else if(spinFlag)
		{
			while(__sync_lock_test_and_set(&spinLock, 1) == 1)
			{}
			SortedList_insert(head, beg + i);
			__sync_lock_release(&spinLock);
		}
		else
			SortedList_insert(head, beg + i);
	}
	//get length of list
	int length;
	if(mutexFlag)
	{
		pthread_mutex_lock(&listMutex);
		length = SortedList_length(head);
		pthread_mutex_unlock(&listMutex);
	}
	else if(spinFlag)
	{
		while(__sync_lock_test_and_set(&spinLock, 1) == 1)
		{}
		length = SortedList_length(head);
		__sync_lock_release(&spinLock);
	}
	else
		length = SortedList_length(head);
	if(length == -1)
	{
		fprintf(stderr, "Error on length: list is corrupted\n");
		exit(2);
	}
	//lookup and delete every element
	for(i = 0; i < iNum; i++)
	{
		SortedListElement_t* foundElement;
		int deleted;
		if(mutexFlag)
		{
			pthread_mutex_lock(&listMutex);
			foundElement = SortedList_lookup(head, beg[i].key);
			//if the element could not be found
			if(foundElement == NULL)
			{
				fprintf(stderr, "Error on lookup: list is corrupted\n");
				exit(2);
			}
			deleted = SortedList_delete(foundElement);
			pthread_mutex_unlock(&listMutex);
		}
		else if(spinFlag)
		{
			while(__sync_lock_test_and_set(&spinLock, 1) == 1)
			{}
			foundElement = SortedList_lookup(head, beg[i].key);
			//if the element could not be found
			if(foundElement == NULL)
			{
				fprintf(stderr, "Error on lookup: list is corrupted\n");
				exit(2);
			}
			deleted = SortedList_delete(foundElement);
			__sync_lock_release(&spinLock);
		}
		else
		{
			foundElement = SortedList_lookup(head, beg[i].key);
			//if could not find the element
			if(foundElement == NULL)
			{
				fprintf(stderr, "Error on lookup: list is corrupted\n");
				exit(2);
			}
			deleted = SortedList_delete(foundElement);
		}
		if(deleted == 1)
		{
			fprintf(stderr, "Error on delete: list is corrupted\n");
			exit(2);
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
    {0, 0, 0, 0}
	};

	int c;
	int iFlag = 0, dFlag = 0, lFlag = 0;
	while((c = getopt_long(argc, argv, "t:i:y:s:", longopts, NULL)) != -1)
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
			case 's':
			if(strcmp(optarg, "m") == 0)
			{
				pthread_mutex_init(&listMutex, NULL);
				mutexFlag = 1;
			}
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

	//initialize signal handler to catch segmentation faults
    signal(SIGSEGV, sighandler);

	//Create elements
	int numList = tNum*iNum;
	//initialize head, set key to null and set head to point to itself
	head = malloc(sizeof(SortedListElement_t));
	head->key = NULL;
	head->next = head;
	head->prev = head;
	//initialize elements
	listElements = malloc(numList*sizeof(SortedListElement_t));
	//time to get keys
	srand(time(NULL));
	int i;
	int len = 64; //set length of key to be 64
	for(i=0; i < numList; i++)
	{
		char key[64];
		int j;
		//save key[63], last element for null byte
		for(j = 0; j < len-2; j++)
			key[j] = 'a' + rand()%26;
		//set each list element key equal to random key
		listElements[i].key = key;
	}
	//initialzie timers
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
	int position;
	for(i = 0; i < tNum; i++)
	{
		position = i*iNum;
		if(pthread_create(&tids[i], NULL, listThread, &listElements[position]) != 0)
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
		int clockError = errno;
		fprintf(stderr, "Error on clock_gettime:\n");
		fprintf(stderr, strerror(clockError));
	  	fprintf(stderr, "\n");
	  	exit(1);
	}

	if(SortedList_length(head) != 0)
	{
		fprintf(stderr, "List was not entirely deleted\n");
		exit(2);
	}
	free(head);
	free(listElements);
	free(tids);
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
	printf("1,");
	int opNum = tNum*iNum*3;
	printf("%d,", opNum);
	long runTime = endTimer.tv_sec - startTimer.tv_sec;
	long nRunTime = runTime * 1000000000L + (endTimer.tv_nsec - startTimer.tv_nsec);
	printf("%li,", nRunTime);
	long avgOpTime = nRunTime/opNum;
	printf("%li\n", avgOpTime);
	return 0;
}
