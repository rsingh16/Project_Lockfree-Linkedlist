
//#define PerCouMon
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#ifdef PerCouMon
#include "cpucounters.h"
#endif
#include <sys/time.h>
#include <iostream>
#define THREADLIMIT 804 
#define THREAD1LIMIT 100
#define THREAD2LIMIT 402
#define THREAD3LIMIT 606
#define THREAD4LIMIT 810

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

	struct timeval start, end;
	//gettimeofday(&start, NULL);
	while(next->data <= data)
	{
		curr = next;
		next = next->next;
	}
	//gettimeofday(&end, NULL);
	//printf("Searchtime=%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
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
	while(head->data != 20000000)
	{
		printf("%d\t",head->data);
		head = head->next;
	}
}

int deleteElement(node_t *head, int data)
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
	tail->data = 20000000;
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
int threadlimit;
int main(int argc, char *argv[])
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
	if(argc > 2)
		printf("\nError! Pass one argument");
	threadlimit = std::stoi(argv[1]);
	node_t * head = init_list();

	struct timeval start, end;

	pthread_t t1, t2, t3, t4;

	#ifdef PerCouMon
	SystemCounterState before_sstate = getSystemCounterState();
	#endif
		
	//clock_t begin = clock();
	gettimeofday(&start, NULL);	
	pthread_create (&t1, NULL, thread1, (void *)head);
	pthread_create (&t2, NULL, thread2, (void *)head);
	pthread_create (&t3, NULL, thread3, (void *)head);
	pthread_create (&t4, NULL, thread4, (void *)head);
	
	pthread_join (t1, NULL);
	pthread_join (t2, NULL);
	pthread_join (t3, NULL);
	pthread_join (t4, NULL);
	gettimeofday(&end, NULL);
	//clock_t end = clock();
	#ifdef PerCouMon	
	SystemCounterState after_sstate = getSystemCounterState();
	#endif
	printf("Time=%ld\n", ((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec)));
	//printlist(head);
	destroy(head);
	
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

{
	node_t * head = (node_t *)args;
	unsigned int i = 0;
	for(i=0; i <= ((threadlimit/4)*4); i=i+4)
	{
		insert(i ,head);
	}
}
void * thread2(void * args)

{
	node_t * head = (node_t *)args;
	unsigned int i = 202;
	for(i=1; i <= ((threadlimit/4)+1)*4; i=i+4)
	{
		insert(i ,head);
	}
}
void * thread3(void * args)

{
	node_t * head = (node_t *)args;
	unsigned int i = 404;
	for(i = 2; i <= ((threadlimit/4)+2)*4; i=i+4)
	{
		insert(i ,head);
	}
}
void * thread4(void * args)

{
	node_t * head = (node_t *)args;
	unsigned int i = 608;
	for(i=3; i <= ((threadlimit/4)+3)*4; i=i+4)
	{
		insert(i ,head);
	}
}

