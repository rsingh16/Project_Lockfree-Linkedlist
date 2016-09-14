//#define PerCouMon
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#ifdef PerCouMon
#include "cpucounters.h"
#endif
#include <time.h>
#include <iostream>

#define EPSILON 0.2
#define setMark(address) ((node_lf *)((uintptr_t)address | 2))
#define setFlag(address) ((node_lf *)((uintptr_t)address | 1))
#define getMark(address) ((int)((uintptr_t)address & 0x00000002) == 2 ?1:0)
#define getFlag(address) ((int)((uintptr_t)address & 0x00000001))
#define getNodeAddress(address) ((node_lf *)((uintptr_t)address & -4))
typedef struct node
{
	int data;
	struct node * next;
	struct node * backlink;
}node_lf;


struct list
{
	node_lf * head;
	node_lf * (*init)();
	int (*insert)(int k,node_lf* head);
	int (*delete_node)(int,node_lf *);
	void (*print)(node_lf *);
	void (*destructor)(node_lf *);
};
typedef struct list * List;
typedef struct searchfrom
{
	node_lf * current;
	node_lf * next;
}return_sf;

typedef struct csArg
{
	int * node;
}cs_arg;

typedef struct return_tryFlag
{
	node_lf * node;
	int result;
}return_tf;

void HelpFlagged(node_lf * prev, node_lf * del_node);



/*node_lf * getNodeAddress(node_lf * p)
{
	return (node_lf * )((uintptr_t)p & 0xFFFFFFFC);
}*/


node_lf * constructArgs(node_lf * node, int mark, int flag)
{
	node_lf * temp = getNodeAddress(node);
	if(mark == 1)
		(temp) = setMark(temp);
	if(flag == 1)
		(temp) = setFlag(temp);
	return temp;
}

void HelpMarked(node_lf * prev, node_lf * del_node)
{
	node_lf * next = getNodeAddress(del_node)->next;

	node_lf * del = constructArgs(del_node,0,1);
	node_lf * next_node = constructArgs(next,0,0);

	__sync_val_compare_and_swap ((unsigned long long *)&(getNodeAddress(prev)->next), (unsigned long long)del, (unsigned long long)next_node);

	free(getNodeAddress(del_node));

}

return_sf SearchFrom(int k,node_lf * curr)
{
	return_sf  s;// = (return_sf *)malloc(sizeof(return_sf));
	node_lf * next = curr->next;
	while(getNodeAddress(next)->data <= k)
	{
		while(getMark(next) == 1 && (getMark(curr) == 0 || getNodeAddress(curr)->next != getNodeAddress(next)))
		{
			if(getNodeAddress(curr)->next == getNodeAddress(next))
			{
				HelpMarked(curr,next);
			}
			next = getNodeAddress(curr)->next;
		}
		if(getNodeAddress(next)->data <= k)
		{
			curr = next;
			next = getNodeAddress(curr)->next;
		}

	}
	s.current = curr;
	s.next = next;
	return s;
}

void TryMark(node_lf * del_node)
{
	node_lf * next;
	node_lf * result;
	do
	{
		next = getNodeAddress(del_node)->next;

		node_lf * next_node = constructArgs(next,0,0);
		node_lf * next_node1 = constructArgs(next,1,0);

		result = (node_lf *)__sync_val_compare_and_swap((unsigned long long *)&(getNodeAddress(del_node)->next),
														(unsigned long long)(next_node),(unsigned long long)(next_node1));

		if(getMark(result) == 0 && getFlag(result) == 1)
			HelpFlagged(del_node,result);
	}while(getMark(getNodeAddress(del_node)->next) != 1);
}

void HelpFlagged(node_lf * prev, node_lf * del_node)
{
	getNodeAddress(del_node)->backlink = prev;
	if(getMark(getNodeAddress(del_node)->next) == 0)
		TryMark(del_node);
	HelpMarked(prev,del_node);
}

int insert(int k, node_lf * head)
{
	return_sf s = SearchFrom(k,head);
	node_lf * prev;
	node_lf * next;
	node_lf * prev_succ;
	prev = s.current;
	next = s.next;
	//free(s);
	if(getNodeAddress(prev)->data == k)
		return -1;
	node_lf * newNode = (node_lf *)malloc(sizeof(node_lf));
	newNode->data = k;
	while(1)
	{
		prev_succ = getNodeAddress(prev)->next;
		if(getFlag(prev_succ) == 1)
			HelpFlagged(prev,getNodeAddress(prev_succ)->next);
		else
		{
			newNode->next = next;

			node_lf * next_node = constructArgs(next,0,0);
			node_lf * new_Node = constructArgs(newNode,0,0);

			node_lf * result = (node_lf *)__sync_val_compare_and_swap((unsigned long long *)&(getNodeAddress(prev)->next),
																		(unsigned long long)(next_node),(unsigned long long)(new_Node));

			if(result == next)// || next == NULL)
			{
				return 0;
			}
			else
			{
				if(getFlag(result) == 1)
					HelpFlagged(prev,(result)->next);
				while(getMark(prev) == 1)
					prev = getNodeAddress(prev)->backlink;
			}

		}
		s = SearchFrom(k,prev);
		if(getNodeAddress(s.current)->data == k)
		{
			free(newNode);
			//free(s);
			return -1;
		}
	}
	//free(s);
	//return 0;
}

return_tf TryFlag(node_lf * prev, node_lf * target)
{
	return_tf r ;//= (return_tf *)malloc(sizeof(return_tf));
	while(1)
	{
		if(getNodeAddress(prev)->next == target && getMark(getNodeAddress(prev)->next) == 0 && getFlag(getNodeAddress(prev)->next) == 1) // Already Flagged. Some other process would delete_node it.
		{
			r.node = prev;
			r.result = 0;
			return r;
		}
		node_lf * target_node = constructArgs(target,0,0);
		node_lf * target_node_new = constructArgs(target,0,1);

		node_lf * result = (node_lf *)__sync_val_compare_and_swap((unsigned long long *)&(getNodeAddress(prev)->next),
																	(unsigned long long)(target_node),(unsigned long long)(target_node_new));
		if(result == target && getMark(result) == 0 && getFlag(result) == 0)
		{
			r.node = prev;
			r.result = 1;
			return r;
		}
		if(result == target && getMark(result) == 0 && getFlag(result) == 1)
		{
			r.node = prev;
			r.result = 0;
			return r;
		}
		while(getMark(getNodeAddress(prev)->next) == 1)
			prev = getNodeAddress(prev)->backlink;
	}
	return_sf s = SearchFrom((getNodeAddress(target)->data) - EPSILON,  prev);
	if(s.next != target)
	{
		r.node = NULL;
		r.result = 0;
		//free(s);
		return r;
	}
	//free(s);
}

int delete_node(int k, node_lf * head)
{
	return_sf s = SearchFrom(k - EPSILON,head);
	node_lf * prev = s.current;
	node_lf * del = s.next;
	//free(s);
	if(getNodeAddress(del)->data != k)
		return -1;
	return_tf tf = TryFlag(prev, del);
	prev = tf.node;
	if(prev != NULL)
		HelpFlagged(prev, del);
	if(tf.result == 0)
	{
		//free(tf);
		return -1;
	}
	//free(tf);
	return 1;
}

node_lf * init_LF_list()
{
	node_lf * head = (node_lf *)malloc(sizeof(node_lf));
	node_lf * tail = (node_lf *)malloc(sizeof(node_lf));
	head->next = tail;
	head->data = -500000;
	tail->next = NULL;
	tail->data = 500000;
	return head;
}

void printlist(node_lf * head)
{
	head = getNodeAddress(head)->next;
	while(getNodeAddress(head)->data != 500000)
	{
		printf("\t%d",getNodeAddress(head)->data);
		head = head->next;
	}
}

void destroy(node_lf * head)
{
	node_lf * next;
	node_lf * curr = getNodeAddress(head);
	while(getNodeAddress(curr) != 0)
	{
		next = getNodeAddress(curr)->next;
		free(curr);
		curr = next;
	}
}

List list_init()
{
	List temp = (List)malloc(sizeof(struct list));
	temp->init = init_LF_list;
	temp->delete_node = delete_node;
	temp->insert = insert;
	temp->print = printlist;
	temp->destructor = destroy;
	return temp;
}

void *thread1(void *);
void *thread2(void *);
void *thread3(void *);
void *thread4(void *);

int main()
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
	
	List mylist = list_init(); //initialize the list object. Can be called as constructor
	mylist->head = mylist->init(); //initialize the list;

	mylist->insert(1,mylist->head);
	mylist->insert(25000,mylist->head);
	mylist->insert(50000,mylist->head);
	mylist->insert(75000,mylist->head);
	pthread_t t1,t2,t3,t4;
	
	#ifdef PerCouMon
	SystemCounterState before_sstate = getSystemCounterState();
	#endif
	
	clock_t begin = clock();	
	pthread_create (&t1, NULL, thread1, (void *)mylist);
	pthread_create (&t2, NULL, thread2, (void *)mylist);
	pthread_create (&t3, NULL, thread3, (void *)mylist);
	pthread_create (&t4, NULL, thread4, (void *)mylist);

	pthread_join (t1, NULL);
	pthread_join (t2, NULL);
	pthread_join (t3, NULL);
	pthread_join (t4, NULL);
	clock_t end = clock();
	
	#ifdef PerCouMon
	SystemCounterState after_sstate = getSystemCounterState();
	#endif
	
	double time_s = (double)(end-begin)/ CLOCKS_PER_SEC;
	
	mylist->destructor(mylist->head);
	free(mylist);
	
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
	List list = (List)args;
	list->insert(1,list->head);
	list->insert(6,list->head);
	list->insert(11,list->head);
	list->insert(16,list->head);
	list->insert(21,list->head);
	list->insert(26,list->head);
	list->insert(31,list->head);
	list->insert(36,list->head);
	list->insert(41,list->head);
	list->insert(46,list->head);
	list->insert(51,list->head);
	list->insert(56,list->head);
	list->insert(61,list->head);
	list->insert(66,list->head);
	list->insert(71,list->head);
	list->insert(76,list->head);
	list->insert(81,list->head);
	list->insert(86,list->head);
	list->insert(91,list->head);
	list->insert(96,list->head);
	list->insert(101,list->head);
	list->insert(106,list->head);
	list->insert(111,list->head);
	list->insert(116,list->head);
	list->insert(121,list->head);
	list->insert(126,list->head);
	list->insert(131,list->head);
	list->insert(136,list->head);
	list->insert(141,list->head);
	list->insert(146,list->head);
	list->insert(151,list->head);
	list->insert(156,list->head);
	list->insert(161,list->head);
	list->insert(166,list->head);
	list->insert(171,list->head);
	list->insert(176,list->head);
	list->insert(181,list->head);
	list->insert(186,list->head);
	list->insert(191,list->head);
	list->insert(196,list->head);
	list->insert(201,list->head);
	list->insert(206,list->head);
	list->insert(211,list->head);
	list->insert(216,list->head);
	list->insert(221,list->head);
	list->insert(226,list->head);
	list->insert(231,list->head);
	list->insert(236,list->head);
	list->insert(241,list->head);
	list->insert(246,list->head);
	list->insert(251,list->head);
	list->insert(256,list->head);
	list->insert(261,list->head);
	list->insert(266,list->head);
	list->insert(271,list->head);
	list->insert(276,list->head);
	list->insert(281,list->head);
	list->insert(286,list->head);
	list->insert(291,list->head);
	list->insert(296,list->head);
	list->insert(301,list->head);
	list->insert(306,list->head);
	list->insert(311,list->head);
	list->insert(316,list->head);
	list->insert(321,list->head);
	list->insert(326,list->head);
	list->insert(331,list->head);
	list->insert(336,list->head);
	list->insert(341,list->head);
	list->insert(346,list->head);
	list->insert(351,list->head);
	list->insert(356,list->head);
	list->insert(361,list->head);
	list->insert(366,list->head);
	list->insert(371,list->head);
	list->insert(376,list->head);
	list->insert(381,list->head);
	list->insert(386,list->head);
	list->insert(391,list->head);
	list->insert(396,list->head);
	list->insert(401,list->head);
	list->insert(406,list->head);
	list->insert(411,list->head);
	list->insert(416,list->head);
	list->insert(421,list->head);
	list->insert(426,list->head);
	list->insert(431,list->head);
	list->insert(436,list->head);
	list->insert(441,list->head);
	list->insert(446,list->head);
	list->insert(451,list->head);
	list->insert(456,list->head);
	list->insert(461,list->head);
	list->insert(466,list->head);
	list->insert(471,list->head);
	list->insert(476,list->head);
	list->insert(481,list->head);
	list->insert(486,list->head);
	list->insert(491,list->head);
	list->insert(496,list->head);
}

void * thread2(void * args)

{
	List list = (List)args;
	list->insert(25000,list->head);
	list->insert(25005,list->head);
	list->insert(25010,list->head);
	list->insert(25015,list->head);
	list->insert(25020,list->head);
	list->insert(25025,list->head);
	list->insert(25030,list->head);
	list->insert(25035,list->head);
	list->insert(25040,list->head);
	list->insert(25045,list->head);
	list->insert(25050,list->head);
	list->insert(25055,list->head);
	list->insert(25060,list->head);
	list->insert(25065,list->head);
	list->insert(25070,list->head);
	list->insert(25075,list->head);
	list->insert(25080,list->head);
	list->insert(25085,list->head);
	list->insert(25090,list->head);
	list->insert(25095,list->head);
	list->insert(25100,list->head);
	list->insert(25105,list->head);
	list->insert(25110,list->head);
	list->insert(25115,list->head);
	list->insert(25120,list->head);
	list->insert(25125,list->head);
	list->insert(25130,list->head);
	list->insert(25135,list->head);
	list->insert(25140,list->head);
	list->insert(25145,list->head);
	list->insert(25150,list->head);
	list->insert(25155,list->head);
	list->insert(25160,list->head);
	list->insert(25165,list->head);
	list->insert(25170,list->head);
	list->insert(25175,list->head);
	list->insert(25180,list->head);
	list->insert(25185,list->head);
	list->insert(25190,list->head);
	list->insert(25195,list->head);
	list->insert(25200,list->head);
	list->insert(25205,list->head);
	list->insert(25210,list->head);
	list->insert(25215,list->head);
	list->insert(25220,list->head);
	list->insert(25225,list->head);
	list->insert(25230,list->head);
	list->insert(25235,list->head);
	list->insert(25240,list->head);
	list->insert(25245,list->head);
	list->insert(25250,list->head);
	list->insert(25255,list->head);
	list->insert(25260,list->head);
	list->insert(25265,list->head);
	list->insert(25270,list->head);
	list->insert(25275,list->head);
	list->insert(25280,list->head);
	list->insert(25285,list->head);
	list->insert(25290,list->head);
	list->insert(25295,list->head);
	list->insert(25300,list->head);
	list->insert(25305,list->head);
	list->insert(25310,list->head);
	list->insert(25315,list->head);
	list->insert(25320,list->head);
	list->insert(25325,list->head);
	list->insert(25330,list->head);
	list->insert(25335,list->head);
	list->insert(25340,list->head);
	list->insert(25345,list->head);
	list->insert(25350,list->head);
	list->insert(25355,list->head);
	list->insert(25360,list->head);
	list->insert(25365,list->head);
	list->insert(25370,list->head);
	list->insert(25375,list->head);
	list->insert(25380,list->head);
	list->insert(25385,list->head);
	list->insert(25390,list->head);
	list->insert(25395,list->head);
	list->insert(25400,list->head);
	list->insert(25405,list->head);
	list->insert(25410,list->head);
	list->insert(25415,list->head);
	list->insert(25420,list->head);
	list->insert(25425,list->head);
	list->insert(25430,list->head);
	list->insert(25435,list->head);
	list->insert(25440,list->head);
	list->insert(25445,list->head);
	list->insert(25450,list->head);
	list->insert(25455,list->head);
	list->insert(25460,list->head);
	list->insert(25465,list->head);
	list->insert(25470,list->head);
	list->insert(25475,list->head);
	list->insert(25480,list->head);
	list->insert(25485,list->head);
	list->insert(25490,list->head);
	list->insert(25495,list->head);
}

void * thread3(void * args)

{
	List list = (List)args;
	list->insert(50000,list->head);
	list->insert(50005,list->head);
	list->insert(50010,list->head);
	list->insert(50015,list->head);
	list->insert(50020,list->head);
	list->insert(50025,list->head);
	list->insert(50030,list->head);
	list->insert(50035,list->head);
	list->insert(50040,list->head);
	list->insert(50045,list->head);
	list->insert(50050,list->head);
	list->insert(50055,list->head);
	list->insert(50060,list->head);
	list->insert(50065,list->head);
	list->insert(50070,list->head);
	list->insert(50075,list->head);
	list->insert(50080,list->head);
	list->insert(50085,list->head);
	list->insert(50090,list->head);
	list->insert(50095,list->head);
	list->insert(50100,list->head);
	list->insert(50105,list->head);
	list->insert(50110,list->head);
	list->insert(50115,list->head);
	list->insert(50120,list->head);
	list->insert(50125,list->head);
	list->insert(50130,list->head);
	list->insert(50135,list->head);
	list->insert(50140,list->head);
	list->insert(50145,list->head);
	list->insert(50150,list->head);
	list->insert(50155,list->head);
	list->insert(50160,list->head);
	list->insert(50165,list->head);
	list->insert(50170,list->head);
	list->insert(50175,list->head);
	list->insert(50180,list->head);
	list->insert(50185,list->head);
	list->insert(50190,list->head);
	list->insert(50195,list->head);
	list->insert(50200,list->head);
	list->insert(50205,list->head);
	list->insert(50210,list->head);
	list->insert(50215,list->head);
	list->insert(50220,list->head);
	list->insert(50225,list->head);
	list->insert(50230,list->head);
	list->insert(50235,list->head);
	list->insert(50240,list->head);
	list->insert(50245,list->head);
	list->insert(50250,list->head);
	list->insert(50255,list->head);
	list->insert(50260,list->head);
	list->insert(50265,list->head);
	list->insert(50270,list->head);
	list->insert(50275,list->head);
	list->insert(50280,list->head);
	list->insert(50285,list->head);
	list->insert(50290,list->head);
	list->insert(50295,list->head);
	list->insert(50300,list->head);
	list->insert(50305,list->head);
	list->insert(50310,list->head);
	list->insert(50315,list->head);
	list->insert(50320,list->head);
	list->insert(50325,list->head);
	list->insert(50330,list->head);
	list->insert(50335,list->head);
	list->insert(50340,list->head);
	list->insert(50345,list->head);
	list->insert(50350,list->head);
	list->insert(50355,list->head);
	list->insert(50360,list->head);
	list->insert(50365,list->head);
	list->insert(50370,list->head);
	list->insert(50375,list->head);
	list->insert(50380,list->head);
	list->insert(50385,list->head);
	list->insert(50390,list->head);
	list->insert(50395,list->head);
	list->insert(50400,list->head);
	list->insert(50405,list->head);
	list->insert(50410,list->head);
	list->insert(50415,list->head);
	list->insert(50420,list->head);
	list->insert(50425,list->head);
	list->insert(50430,list->head);
	list->insert(50435,list->head);
	list->insert(50440,list->head);
	list->insert(50445,list->head);
	list->insert(50450,list->head);
	list->insert(50455,list->head);
	list->insert(50460,list->head);
	list->insert(50465,list->head);
	list->insert(50470,list->head);
	list->insert(50475,list->head);
	list->insert(50480,list->head);
	list->insert(50485,list->head);
	list->insert(50490,list->head);
	list->insert(50495,list->head);
}
void * thread4(void * args)

{
	List list = (List)args;
	list->insert(75000,list->head);
	list->insert(75005,list->head);
	list->insert(75010,list->head);
	list->insert(75015,list->head);
	list->insert(75020,list->head);
	list->insert(75025,list->head);
	list->insert(75030,list->head);
	list->insert(75035,list->head);
	list->insert(75040,list->head);
	list->insert(75045,list->head);
	list->insert(75050,list->head);
	list->insert(75055,list->head);
	list->insert(75060,list->head);
	list->insert(75065,list->head);
	list->insert(75070,list->head);
	list->insert(75075,list->head);
	list->insert(75080,list->head);
	list->insert(75085,list->head);
	list->insert(75090,list->head);
	list->insert(75095,list->head);
	list->insert(75100,list->head);
	list->insert(75105,list->head);
	list->insert(75110,list->head);
	list->insert(75115,list->head);
	list->insert(75120,list->head);
	list->insert(75125,list->head);
	list->insert(75130,list->head);
	list->insert(75135,list->head);
	list->insert(75140,list->head);
	list->insert(75145,list->head);
	list->insert(75150,list->head);
	list->insert(75155,list->head);
	list->insert(75160,list->head);
	list->insert(75165,list->head);
	list->insert(75170,list->head);
	list->insert(75175,list->head);
	list->insert(75180,list->head);
	list->insert(75185,list->head);
	list->insert(75190,list->head);
	list->insert(75195,list->head);
	list->insert(75200,list->head);
	list->insert(75205,list->head);
	list->insert(75210,list->head);
	list->insert(75215,list->head);
	list->insert(75220,list->head);
	list->insert(75225,list->head);
	list->insert(75230,list->head);
	list->insert(75235,list->head);
	list->insert(75240,list->head);
	list->insert(75245,list->head);
	list->insert(75250,list->head);
	list->insert(75255,list->head);
	list->insert(75260,list->head);
	list->insert(75265,list->head);
	list->insert(75270,list->head);
	list->insert(75275,list->head);
	list->insert(75280,list->head);
	list->insert(75285,list->head);
	list->insert(75290,list->head);
	list->insert(75295,list->head);
	list->insert(75300,list->head);
	list->insert(75305,list->head);
	list->insert(75310,list->head);
	list->insert(75315,list->head);
	list->insert(75320,list->head);
	list->insert(75325,list->head);
	list->insert(75330,list->head);
	list->insert(75335,list->head);
	list->insert(75340,list->head);
	list->insert(75345,list->head);
	list->insert(75350,list->head);
	list->insert(75355,list->head);
	list->insert(75360,list->head);
	list->insert(75365,list->head);
	list->insert(75370,list->head);
	list->insert(75375,list->head);
	list->insert(75380,list->head);
	list->insert(75385,list->head);
	list->insert(75390,list->head);
	list->insert(75395,list->head);
	list->insert(75400,list->head);
	list->insert(75405,list->head);
	list->insert(75410,list->head);
	list->insert(75415,list->head);
	list->insert(75420,list->head);
	list->insert(75425,list->head);
	list->insert(75430,list->head);
	list->insert(75435,list->head);
	list->insert(75440,list->head);
	list->insert(75445,list->head);
	list->insert(75450,list->head);
	list->insert(75455,list->head);
	list->insert(75460,list->head);
	list->insert(75465,list->head);
	list->insert(75470,list->head);
	list->insert(75475,list->head);
	list->insert(75480,list->head);
	list->insert(75485,list->head);
	list->insert(75490,list->head);
	list->insert(75495,list->head);
}


