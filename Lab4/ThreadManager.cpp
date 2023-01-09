#ifndef THREAD_MANAGER_CPP
#define THREAD_MANAGER_CPP

#include "ThreadManager.h"
#include <exception>
#include <iostream>

_LABA4_START

ThreadManager::ThreadManager(int threadsCount)
{
    this->threadsList = new std::list<HANDLE>(threadsCount);
    
}

DWORD WINAPI ThreadManager::ThreadProcedure(void * selfObject)
{
    DWORD result = 0;
    try
    {
        ThreadManager* threadManager = (ThreadManager*) selfObject;
        TASK_DATA taskDataPtr;
        
        while (true)
        {
            threadManager->DequeueTask(&taskDataPtr);
            if (taskDataPtr.function != nullptr)
            {
                try
                {
                    taskDataPtr.function(taskDataPtr.params);
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }

            }
            else
            {
                break;
            }
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << "Error on working thread: " << e.what() << '\n';
        result = -1;
    }

    return result;
}

void ThreadManager::StartThreadsWork()
{

    if (!this->isThreadsWork)
    {
        this->isThreadsWork = true;
        for (auto &&i : *this->threadsList)
        {
            i = CreateThread(NULL, 0, ThreadManager::ThreadProcedure, this, CREATE_SUSPENDED, NULL);
            ResumeThread(i);
        }
    }
    
}

void ThreadManager::EndThreadsWork()
{   
    if (this->isThreadsWork)
    {
        for (int i = 0; i < this->threadsList->size(); i++)
        {
            TASK_DATA taskData = {NULL, NULL};
            this->EnqueueTask(taskData);
        }

        for (auto &&i : *this->threadsList)
        {
            WaitForSingleObject(i, INFINITE);
            CloseHandle(i);
        }
        
        this->isThreadsWork = false;
    }

    std::cout << "End work of threads";

}

ThreadManager::~ThreadManager()
{
    this->EndThreadsWork();
    delete this->threadsList;
}

_LABA4_END

#endif
