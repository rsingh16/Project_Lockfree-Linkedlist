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
#include <limits>

//#define debug
//#define sequential

typedef struct node
{
	int data;
	struct node* next;
}node_t;


void * thread1(void *);
/*void * thread2(void *);
void * thread3(void *);
void * thread4(void *);*/


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void insert(int data,node_t * head)
{

	pthread_mutex_lock(&mutex);
	node_t * next = head->next;
	node_t * curr = head;

	struct timeval start, end;
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
	while(head->data != std::numeric_limits<int>::max())
	{
		printf("%d\t",head->data);
		head = head->next;
	}
	printf("\n");
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
	head->data = std::numeric_limits<int>::min();
	tail->data = std::numeric_limits<int>::max();
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


struct arg
{
	node_t * head;
	int thread_id;
	int tnum;

};


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
	node_t * head = init_list();
	struct timeval start, end;
	
	
	#ifndef sequential
	if(argc < 3 )
		printf("\nError! Pass 2 arguments argc%d",argc);
	
	threadlimit = std::stoi(argv[1]);
	int numberOfThreads = std::stoi(argv[2]);

	pthread_t * t = (pthread_t *)malloc(numberOfThreads*sizeof(pthread_t));

	struct arg * tArgs =(arg *)malloc(numberOfThreads*sizeof(arg));
	
	#ifdef PerCouMon
	SystemCounterState before_sstate = getSystemCounterState();
	#endif
		

	gettimeofday(&start, NULL);	
	
	
	for(int i=0;i<numberOfThreads;i++)
	{
		tArgs[i].head = head;
		tArgs[i].thread_id = i;
		tArgs[i].tnum = numberOfThreads;
		pthread_create (&t[i], NULL, thread1, (void *)&tArgs[i]);
	}
	
	for(int i=0;i<numberOfThreads;i++)
	{
		pthread_join (t[i], NULL);
	}
		
	gettimeofday(&end, NULL);
			
	printf("Time=%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	
	#endif
	
	
	//The following code only executes if sequential is defined above.
	#ifdef sequential
	
	int numberOfEntries = std::stoi(argv[1]);
	int i=0;
	gettimeofday(&start, NULL);	
	for(i=0;i<numberOfEntries;i++)
		insert(i, head);
	gettimeofday(&end, NULL);
	printf("Time Seq=%ld\n", ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
	#endif
	
	
	#ifdef PerCouMon	
	SystemCounterState after_sstate = getSystemCounterState();
	#endif
	
	
	#ifdef debug
	printlist(head);
	#endif
	
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
	struct arg * arguments = (struct arg *)args;
	unsigned int i = 0;
	for(i=arguments->thread_id; i < ((threadlimit)*arguments->tnum)+arguments->thread_id; i=i+arguments->tnum)
	{
		
		insert(i, arguments->head);
	}
	
}
/*void * thread2(void * args)

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
}*/

