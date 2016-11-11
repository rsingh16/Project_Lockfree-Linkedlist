all:
	g++ -o mylocks -std=c++11 ListUsingLocks_4_thread.cpp -lpthread
	g++ -o mylockfree -std=c++11 LockFreeListFinal_4_thread.cpp -lpthread
lock: ListUsingLocks_4_thread.cpp
	g++ -o mylocks -std=c++11 ListUsingLocks_4_thread.cpp -lpthread
    
lockfree: LockFreeListFinal_4_thread.cpp
	g++ -o mylockfree -std=c++11 LockFreeListFinal_4_thread.cpp -lpthread
