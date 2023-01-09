#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "TaskQueue.h"
#include "Windows.h"
#include "list"

typedef void CallBackOnEmptyQueue();


_LABA4_START

class ThreadManager : public TaskQueue
{

private:

    std::list<HANDLE>* threadsList;
    bool isThreadsWork = false;
    
    static DWORD WINAPI ThreadProcedure(void * selfObject);
    
public:

    ThreadManager(int threadsCount);
    void StartThreadsWork();
    void EndThreadsWork();
    ~ThreadManager();

};

_LABA4_END

#include "ThreadManager.cpp"


#endif
