
//#define PerCouMon
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#ifdef PerCouMon
#include "cpucounters.h"
#endif
#include <time.h>
#include <iostream>

typedef struct node
{
	int data;
	struct node* next;
}node_t;


void * thread1(void *);
void * thread2(void *);
void * thread3(void *);
void * thread4(void *);


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void insert(int data,node_t * head)
{

	pthread_mutex_lock(&mutex);
	node_t * next = head->next;
	node_t * curr = head;


	while(next->data <= data)
	{
		curr = next;
		next = next->next;
	}
	if(curr->data != data)
	{
		node_t * n = (node_t *)malloc(sizeof(struct node));
		n->data = data;
		n->next = next;
		curr->next = n;
	}
	pthread_mutex_unlock(&mutex);
}

void printlist(node_t * head)
{
	head = head->next;
	while(head->data != 500000)
	{
		printf("%d\t",head->data);
		head = head->next;
	}
}

int deleteElement(int data, node_t *head)
{
	pthread_mutex_lock(&mutex);

	node_t * next = head->next;
	node_t * curr = head;

	while(next->data <= data-0.2)
	{
		curr = next;
		next = next->next;
	}
	if(next->data != data)
		return -1;
	curr->next = next->next;
	free(next);

	pthread_mutex_unlock(&mutex);

	return 1;
}

node_t *init_list()
{
	node_t * head = (node_t *)malloc(sizeof(node_t));
	node_t * tail = (node_t *)malloc(sizeof(node_t));
	head->next = tail;
	head->data = -500000;
	tail->data = 500000;
	return head;
}

void destroy(node_t * head)
{
	node_t * next;
	node_t * curr = head;
	while(curr != 0)
	{
		next = curr->next;
		free(curr);
		curr = next;
	}
}

int main(void)
{
	#ifdef PerCouMon	
	PCM * m = PCM::getInstance();
	PCM::ErrorCode returnResult = m->program();
	if (returnResult != PCM::Success){
	  std::cerr << "Intel's PCM couldn't start" << std::endl;
	  std::cerr << "Error code: " << returnResult << std::endl;
	  exit(1);
	}
	#endif
	
	node_t * head = init_list();

	pthread_t t1, t2, t3, t4;

	#ifdef PerCouMon
	SystemCounterState before_sstate = getSystemCounterState();
	#endif
		
	clock_t begin = clock();	
	pthread_create (&t1, NULL, thread1, (void *)head);
	pthread_create (&t2, NULL, thread2, (void *)head);
	pthread_create (&t3, NULL, thread3, (void *)head);
	pthread_create (&t4, NULL, thread4, (void *)head);
	
	pthread_join (t1, NULL);
	pthread_join (t2, NULL);
	pthread_join (t3, NULL);
	pthread_join (t4, NULL);
	clock_t end = clock();
	#ifdef PerCouMon	
	SystemCounterState after_sstate = getSystemCounterState();
	#endif
	destroy(head);
	double time_s = (double)(end-begin)/ CLOCKS_PER_SEC;
	
	#ifdef PerCouMon
	std::cout << "CPU Time:" <<time_s << std::endl;
	std::cout << "Instructions per clock:" << getIPC(before_sstate,after_sstate) << std::endl;
	std::cout << "Bytes read:" << getBytesReadFromMC(before_sstate,after_sstate) << std::endl;
	std::cout << "Bytes Written:" << getBytesWrittenToMC(before_sstate,after_sstate) << std::endl;
	std::cout << "L2 Misses:" << getL2CacheMisses(before_sstate,after_sstate)<< std::endl;
	std::cout << "L2 HIts:" << getL2CacheHits(before_sstate,after_sstate)<< std::endl;
	std::cout << "L3 Misses:" << getL3CacheMisses(before_sstate,after_sstate)<< std::endl;
	std::cout << "L3 HIts:" << getL3CacheHits(before_sstate,after_sstate)<< std::endl;
	std::cout << "Cycles:" << getCycles(before_sstate,after_sstate)<<std::endl;
	std::cout << "Cycles Lost Due L3 Cache Misses:" << getCyclesLostDueL3CacheMisses(before_sstate,after_sstate)<<std::endl;
	std::cout << "Cycles Lost Due L2 Cache Misses:" << getCyclesLostDueL2CacheMisses(before_sstate,after_sstate)<<std::endl;
	#endif
	
	return 0;
}


void * thread1(void * args)

void * thread2(void * args)

void * thread3(void * args)

void * thread4(void * args)

