# This python file needs 2 command line arguments:
# argv[1] = name of the base file
# argv[2] = number of insert operation per thread.
# Version: 1.2
# Version 2.0 will take 3 arguments, the new arg will be the number of threads.

from sys import argv

fptr = open(argv[1], "r")
foutput = open(argv[1][:-4]+"_"+argv[2]+"_queries_per_thread.cpp", "w")


thread1 = 1
thread2 = 25000
thread3 = 50000
thread4 = 75000

for line in fptr.readlines():
    if "free" in argv[1] or "Free" in argv[1]:
        if "mylist->head = mylist->init()" in line:
            foutput.write(line)
            foutput.write("\n\tmylist->insert(" + str(thread1) + ",mylist->head);")
            foutput.write("\n\tmylist->insert(" + str(thread2) + ",mylist->head);")
            foutput.write("\n\tmylist->insert(" + str(thread3) + ",mylist->head);")
            foutput.write("\n\tmylist->insert(" + str(thread4) + ",mylist->head);")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tlist->insert(" + str(thread1) + ",mylist->head);")
                thread1 += 5
            foutput.write("\n}")
            thread1 = 1
        elif "void * thread1(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tList list = (List)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tlist->delete(" + str(thread1) + ",list->head);")
                thread1 += 5
            foutput.write("\n}")
        elif "void * thread2(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tList list = (List)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tlist->insert(" + str(thread2) + ",list->head);")
                thread2 += 5
            foutput.write("\n}")
        elif "void * thread3(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tList list = (List)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tlist->insert(" + str(thread3) + ",list->head);")
                thread3 += 5
            foutput.write("\n}")
        elif "void * thread4(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tList list = (List)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tlist->insert(" + str(thread4) + ",list->head);")
                thread4 += 5
            foutput.write("\n}")
        elif "mylist->head = mylist->init()" not in line and "void * thread4(void * args)" != line and \
                "void * thread3(void * args)" != line and "void * thread2(void * args)" != line \
                and "void * thread1(void * args)" != line:
            foutput.write(line)
    else:
        if "node_t * head = init_list();" in line:
            foutput.write(line)
            foutput.write("\n\tinsert(" + str(thread1) + ",head);")
            foutput.write("\n\tinsert(" + str(thread2) + ",head);")
            foutput.write("\n\tinsert(" + str(thread3) + ",head);")
            foutput.write("\n\tinsert(" + str(thread4) + ",head);")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tinsert(" + str(thread1) + ",head);")
                thread1 += 5
            foutput.write("\n}")
        elif "void * thread1(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tnode_t * head = (node_t *)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tdeleteElement(" + str(thread1) + ",head);")
                thread1 += 5
            foutput.write("\n}")
        elif "void * thread2(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tnode_t * head = (node_t *)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tinsert(" + str(thread2) + ",head);")
                thread2 += 5
            foutput.write("\n}")
        elif "void * thread3(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tnode_t * head = (node_t *)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tinsert(" + str(thread3) + ",head);")
                thread3 += 5
            foutput.write("\n}")
        elif "void * thread4(void * args)" in line:
            foutput.write(line)
            foutput.write("\n{\n\tnode_t * head = (node_t *)args;")
            for i in range(1, int(argv[2])+1):
                foutput.write("\n\tinsert(" + str(thread4) + ",head);")
                thread4 += 5
            foutput.write("\n}")
        elif "node_t * head = init_list();" not in line and "void * thread4(void * args)" != line and \
                "void * thread3(void * args)" != line and "void * thread2(void * args)" != line \
                and "void * thread1(void * args)" != line:
            foutput.write(line)
fptr.close()
foutput.close()

