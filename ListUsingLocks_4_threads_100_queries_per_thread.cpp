
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

	insert(1,head);
	insert(25000,head);
	insert(50000,head);
	insert(75000,head);
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

{
	node_t * head = (node_t *)args;
	insert(1,head);
	insert(6,head);
	insert(11,head);
	insert(16,head);
	insert(21,head);
	insert(26,head);
	insert(31,head);
	insert(36,head);
	insert(41,head);
	insert(46,head);
	insert(51,head);
	insert(56,head);
	insert(61,head);
	insert(66,head);
	insert(71,head);
	insert(76,head);
	insert(81,head);
	insert(86,head);
	insert(91,head);
	insert(96,head);
	insert(101,head);
	insert(106,head);
	insert(111,head);
	insert(116,head);
	insert(121,head);
	insert(126,head);
	insert(131,head);
	insert(136,head);
	insert(141,head);
	insert(146,head);
	insert(151,head);
	insert(156,head);
	insert(161,head);
	insert(166,head);
	insert(171,head);
	insert(176,head);
	insert(181,head);
	insert(186,head);
	insert(191,head);
	insert(196,head);
	insert(201,head);
	insert(206,head);
	insert(211,head);
	insert(216,head);
	insert(221,head);
	insert(226,head);
	insert(231,head);
	insert(236,head);
	insert(241,head);
	insert(246,head);
	insert(251,head);
	insert(256,head);
	insert(261,head);
	insert(266,head);
	insert(271,head);
	insert(276,head);
	insert(281,head);
	insert(286,head);
	insert(291,head);
	insert(296,head);
	insert(301,head);
	insert(306,head);
	insert(311,head);
	insert(316,head);
	insert(321,head);
	insert(326,head);
	insert(331,head);
	insert(336,head);
	insert(341,head);
	insert(346,head);
	insert(351,head);
	insert(356,head);
	insert(361,head);
	insert(366,head);
	insert(371,head);
	insert(376,head);
	insert(381,head);
	insert(386,head);
	insert(391,head);
	insert(396,head);
	insert(401,head);
	insert(406,head);
	insert(411,head);
	insert(416,head);
	insert(421,head);
	insert(426,head);
	insert(431,head);
	insert(436,head);
	insert(441,head);
	insert(446,head);
	insert(451,head);
	insert(456,head);
	insert(461,head);
	insert(466,head);
	insert(471,head);
	insert(476,head);
	insert(481,head);
	insert(486,head);
	insert(491,head);
	insert(496,head);
}
void * thread2(void * args)

{
	node_t * head = (node_t *)args;
	insert(25000,head);
	insert(25005,head);
	insert(25010,head);
	insert(25015,head);
	insert(25020,head);
	insert(25025,head);
	insert(25030,head);
	insert(25035,head);
	insert(25040,head);
	insert(25045,head);
	insert(25050,head);
	insert(25055,head);
	insert(25060,head);
	insert(25065,head);
	insert(25070,head);
	insert(25075,head);
	insert(25080,head);
	insert(25085,head);
	insert(25090,head);
	insert(25095,head);
	insert(25100,head);
	insert(25105,head);
	insert(25110,head);
	insert(25115,head);
	insert(25120,head);
	insert(25125,head);
	insert(25130,head);
	insert(25135,head);
	insert(25140,head);
	insert(25145,head);
	insert(25150,head);
	insert(25155,head);
	insert(25160,head);
	insert(25165,head);
	insert(25170,head);
	insert(25175,head);
	insert(25180,head);
	insert(25185,head);
	insert(25190,head);
	insert(25195,head);
	insert(25200,head);
	insert(25205,head);
	insert(25210,head);
	insert(25215,head);
	insert(25220,head);
	insert(25225,head);
	insert(25230,head);
	insert(25235,head);
	insert(25240,head);
	insert(25245,head);
	insert(25250,head);
	insert(25255,head);
	insert(25260,head);
	insert(25265,head);
	insert(25270,head);
	insert(25275,head);
	insert(25280,head);
	insert(25285,head);
	insert(25290,head);
	insert(25295,head);
	insert(25300,head);
	insert(25305,head);
	insert(25310,head);
	insert(25315,head);
	insert(25320,head);
	insert(25325,head);
	insert(25330,head);
	insert(25335,head);
	insert(25340,head);
	insert(25345,head);
	insert(25350,head);
	insert(25355,head);
	insert(25360,head);
	insert(25365,head);
	insert(25370,head);
	insert(25375,head);
	insert(25380,head);
	insert(25385,head);
	insert(25390,head);
	insert(25395,head);
	insert(25400,head);
	insert(25405,head);
	insert(25410,head);
	insert(25415,head);
	insert(25420,head);
	insert(25425,head);
	insert(25430,head);
	insert(25435,head);
	insert(25440,head);
	insert(25445,head);
	insert(25450,head);
	insert(25455,head);
	insert(25460,head);
	insert(25465,head);
	insert(25470,head);
	insert(25475,head);
	insert(25480,head);
	insert(25485,head);
	insert(25490,head);
	insert(25495,head);
}
void * thread3(void * args)

{
	node_t * head = (node_t *)args;
	insert(50000,head);
	insert(50005,head);
	insert(50010,head);
	insert(50015,head);
	insert(50020,head);
	insert(50025,head);
	insert(50030,head);
	insert(50035,head);
	insert(50040,head);
	insert(50045,head);
	insert(50050,head);
	insert(50055,head);
	insert(50060,head);
	insert(50065,head);
	insert(50070,head);
	insert(50075,head);
	insert(50080,head);
	insert(50085,head);
	insert(50090,head);
	insert(50095,head);
	insert(50100,head);
	insert(50105,head);
	insert(50110,head);
	insert(50115,head);
	insert(50120,head);
	insert(50125,head);
	insert(50130,head);
	insert(50135,head);
	insert(50140,head);
	insert(50145,head);
	insert(50150,head);
	insert(50155,head);
	insert(50160,head);
	insert(50165,head);
	insert(50170,head);
	insert(50175,head);
	insert(50180,head);
	insert(50185,head);
	insert(50190,head);
	insert(50195,head);
	insert(50200,head);
	insert(50205,head);
	insert(50210,head);
	insert(50215,head);
	insert(50220,head);
	insert(50225,head);
	insert(50230,head);
	insert(50235,head);
	insert(50240,head);
	insert(50245,head);
	insert(50250,head);
	insert(50255,head);
	insert(50260,head);
	insert(50265,head);
	insert(50270,head);
	insert(50275,head);
	insert(50280,head);
	insert(50285,head);
	insert(50290,head);
	insert(50295,head);
	insert(50300,head);
	insert(50305,head);
	insert(50310,head);
	insert(50315,head);
	insert(50320,head);
	insert(50325,head);
	insert(50330,head);
	insert(50335,head);
	insert(50340,head);
	insert(50345,head);
	insert(50350,head);
	insert(50355,head);
	insert(50360,head);
	insert(50365,head);
	insert(50370,head);
	insert(50375,head);
	insert(50380,head);
	insert(50385,head);
	insert(50390,head);
	insert(50395,head);
	insert(50400,head);
	insert(50405,head);
	insert(50410,head);
	insert(50415,head);
	insert(50420,head);
	insert(50425,head);
	insert(50430,head);
	insert(50435,head);
	insert(50440,head);
	insert(50445,head);
	insert(50450,head);
	insert(50455,head);
	insert(50460,head);
	insert(50465,head);
	insert(50470,head);
	insert(50475,head);
	insert(50480,head);
	insert(50485,head);
	insert(50490,head);
	insert(50495,head);
}
void * thread4(void * args)

{
	node_t * head = (node_t *)args;
	insert(75000,head);
	insert(75005,head);
	insert(75010,head);
	insert(75015,head);
	insert(75020,head);
	insert(75025,head);
	insert(75030,head);
	insert(75035,head);
	insert(75040,head);
	insert(75045,head);
	insert(75050,head);
	insert(75055,head);
	insert(75060,head);
	insert(75065,head);
	insert(75070,head);
	insert(75075,head);
	insert(75080,head);
	insert(75085,head);
	insert(75090,head);
	insert(75095,head);
	insert(75100,head);
	insert(75105,head);
	insert(75110,head);
	insert(75115,head);
	insert(75120,head);
	insert(75125,head);
	insert(75130,head);
	insert(75135,head);
	insert(75140,head);
	insert(75145,head);
	insert(75150,head);
	insert(75155,head);
	insert(75160,head);
	insert(75165,head);
	insert(75170,head);
	insert(75175,head);
	insert(75180,head);
	insert(75185,head);
	insert(75190,head);
	insert(75195,head);
	insert(75200,head);
	insert(75205,head);
	insert(75210,head);
	insert(75215,head);
	insert(75220,head);
	insert(75225,head);
	insert(75230,head);
	insert(75235,head);
	insert(75240,head);
	insert(75245,head);
	insert(75250,head);
	insert(75255,head);
	insert(75260,head);
	insert(75265,head);
	insert(75270,head);
	insert(75275,head);
	insert(75280,head);
	insert(75285,head);
	insert(75290,head);
	insert(75295,head);
	insert(75300,head);
	insert(75305,head);
	insert(75310,head);
	insert(75315,head);
	insert(75320,head);
	insert(75325,head);
	insert(75330,head);
	insert(75335,head);
	insert(75340,head);
	insert(75345,head);
	insert(75350,head);
	insert(75355,head);
	insert(75360,head);
	insert(75365,head);
	insert(75370,head);
	insert(75375,head);
	insert(75380,head);
	insert(75385,head);
	insert(75390,head);
	insert(75395,head);
	insert(75400,head);
	insert(75405,head);
	insert(75410,head);
	insert(75415,head);
	insert(75420,head);
	insert(75425,head);
	insert(75430,head);
	insert(75435,head);
	insert(75440,head);
	insert(75445,head);
	insert(75450,head);
	insert(75455,head);
	insert(75460,head);
	insert(75465,head);
	insert(75470,head);
	insert(75475,head);
	insert(75480,head);
	insert(75485,head);
	insert(75490,head);
	insert(75495,head);
}

