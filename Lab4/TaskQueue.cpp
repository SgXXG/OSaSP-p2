#ifndef TASK_QUEUE_CPP
#define TASK_QUEUE_CPP

#include "TaskQueue.h"

_LABA4_START

TaskQueue::TaskQueue()
{
    this->taskQueue = new std::queue<TASK_DATA>();

    InitializeConditionVariable(&this->locker);
    InitializeCriticalSection(&this->section);
}


void TaskQueue::EnqueueTask(TASK_DATA taskData)
{
    printf("I thread %d enter on critical section of enqueue task\n", GetThreadId(GetCurrentThread()));
    EnterCriticalSection(&this->section);

    this->taskQueue->push(taskData);

    LeaveCriticalSection(&this->section);
    printf("I thread %d leave on critical section of enqueue task\n", GetThreadId(GetCurrentThread()));
    WakeConditionVariable(&this->locker);
}

void TaskQueue::DequeueTask(TASK_DATA* pointerOnTaskData)
{
    
    printf("I thread %d enter on critical section of DEqueue task\n", GetThreadId(GetCurrentThread()));
    EnterCriticalSection(&this->section);

    while (this->taskQueue->empty())
    {
        printf("I thread %d Sleep of DEqueue task\n", GetThreadId(GetCurrentThread()));
        while (!SleepConditionVariableCS(&this->locker, &this->section, INFINITE));
        
        printf("I thread %d Woke up of DEqueue task\n", GetThreadId(GetCurrentThread()));
        printf("I thread %d enter on critical section of DEqueue task\n", GetThreadId(GetCurrentThread()));
        // EnterCriticalSection(&this->section);
    }

    *pointerOnTaskData = this->taskQueue->front();
    this->taskQueue->pop();
    LeaveCriticalSection(&this->section);
    printf("I thread %d leave on critical section of DEqueue task\n", GetThreadId(GetCurrentThread()));
}

TaskQueue::~TaskQueue()
{
    delete this->taskQueue;
    DeleteCriticalSection(&this->section);
}

_LABA4_END

#endif