#include "ThreadManager.h"
#define THREAD_COUNT 5
#define TASKS_SIZE (THREAD_COUNT * 2)*100

volatile LONG taskNum = 0;

int printThreadInfo(void * params)
{
    InterlockedIncrement(&taskNum);
    printf("I thread %d i do procedure work numer %d\n", GetThreadId(GetCurrentThread()), taskNum);
    return 1;
}

int main()
{
    
    Laba4::ThreadManager* threadManager = new Laba4::ThreadManager(THREAD_COUNT);

    TASK_DATA taskData = {printThreadInfo, nullptr};
    for (int i = 0; i < TASKS_SIZE; i++)
    {
        threadManager->EnqueueTask(taskData);
    }

    threadManager->StartThreadsWork();
    threadManager->EndThreadsWork();
    
    delete threadManager;

    return 0; 
}