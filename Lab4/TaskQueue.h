#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#include <queue>
#include <list>
#include "Windows.h"
#define _LABA4_START namespace Laba4 {
#define _LABA4_END }

typedef int (*TaskFunction) (void * params) ;

typedef struct _funcAndParams
{
    TaskFunction function;
    void* params;
} TASK_DATA;

_LABA4_START

class TaskQueue
{
    
    private:

        std::queue<TASK_DATA> * taskQueue;
        
        CONDITION_VARIABLE locker;
        CRITICAL_SECTION section;


    public:

        TaskQueue();
        ~TaskQueue();
        void EnqueueTask(TASK_DATA taskData);
        void DequeueTask(TASK_DATA* pointerOnTaskData);
        

};

_LABA4_END

#include "TaskQueue.cpp"

#endif