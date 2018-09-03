/*Oliver Goch
 the.oliver.goch@gmail.com
 123456789*/
#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <sched.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	if(list == NULL || element->key == NULL)
		return;
	//get first element
	SortedListElement_t* current = list->next;
	//if the current element is not blank (the head) and the element is greater than current
	while(current->key != NULL && strcmp(element->key,current->key) > 0)
		current = current->next;

	if(opt_yield & INSERT_YIELD)
		sched_yield();
	//previous item now points to this item
	current->prev->next = element;
	//previous points to current previous item
	element->prev = current->prev;
	//next item is current item
	element->next = current;
	//current item previous is new item
	current->prev = element;
}

int SortedList_delete( SortedListElement_t *element)
{
	if(element == NULL || element->next->prev != element->prev->next)
		return 1;

	if(opt_yield & DELETE_YIELD)
		sched_yield();
	//previous element next is next
	element->prev->next = element->next;
	//next item previous points to previous
	element->next->prev = element->prev;
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
	if(list == NULL || key == NULL)
		return NULL;
	SortedListElement_t* current = list->next;

	while(current != list)
	{
		if(strcmp(key,current->key) == 0)
			return current;
		
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		current = current->next;
	}

	return NULL;
}

int SortedList_length(SortedList_t *list)
{
	if(list == NULL)
		return -1;

	SortedListElement_t* current = list->next;	
	int result = 0;
	while(current != list)
	{
		//if the pointers are messed up return -1
		if(current->next->prev != current->prev->next)
			return -1;
		result++;
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		current = current->next;
	}

	return result;
}
