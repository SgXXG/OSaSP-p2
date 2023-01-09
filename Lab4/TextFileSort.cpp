#include "Windows.h"
#include "stdio.h"
#include "ThreadManager.h"

#define BUF_SIZE 1024
typedef struct _string_array
{
    char** arrayPointer;
    LONGLONG arrayLength;
} stringArray;

typedef struct _sortBoards
{
    int leftBoard;
    int rightBoard;
    _sortBoards* ptOnNext;
} sortBoards;

typedef struct _sortLayers
{
    _sortLayers* ptOnNextLayer;
    sortBoards* ptOnSortBoards;
    int countBoards;
} sortLayers;


HANDLE hFile = INVALID_HANDLE_VALUE;
HANDLE hMappedFile = INVALID_HANDLE_VALUE;
void* fileMapPointer = NULL;
LONGLONG fileSize = 0;
stringArray stringsForSorting;

volatile unsigned int currentBoardProcess = 0; 
CONDITION_VARIABLE sortCondVariable;
CRITICAL_SECTION sortCritSection;

bool InitializeMemoryView(char* fileName)
{
    hFile = CreateFileA(
        fileName, 
        GENERIC_READ | GENERIC_WRITE,
        0,                              //Check, maybe error in mapObject creating
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );

    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Error in file open! May be incorrect path\n");
        return false;
    }


    LARGE_INTEGER fileSizeStruct = {0};
    GetFileSizeEx(hFile, &fileSizeStruct);
    fileSize = fileSizeStruct.QuadPart;

    if (fileSize == 0){
        printf("Error! Your file is empty\n");
        return false;
    }
    
    
    hMappedFile = CreateFileMappingA(
        hFile,
        NULL,
        PAGE_READWRITE,
        0,
        0,
        "laba4filemap"
    );

    if (hMappedFile == NULL)
    {
        printf("Error on creating file mapped object!\n");
        return false;
    }

    fileMapPointer = MapViewOfFile(
        hMappedFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0
    );

    if (fileMapPointer == NULL)
    {
        printf("Error on file mapping memory\n");
        return false;
    }

    return true;
}

void freeResources()
{
    if (fileMapPointer != NULL)
        {
            UnmapViewOfFile(fileMapPointer);
            fileMapPointer = NULL;
        }

        if (hMappedFile != NULL)
        {
            CloseHandle(hMappedFile);
            fileMapPointer = NULL;
        }
        
        if (hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hFile);
            hFile = INVALID_HANDLE_VALUE;
        }

}

int strLineLength(char* str, bool * isLineEnd)
{
    int res = 0;
    while (str[res] != '\0' && (str[res] != '\r' || str[res + 1] != '\n'))
        res++;

    if (isLineEnd)
    {
        *isLineEnd = str[res] ==  '\0';
    }
    return res;
}

stringArray FormArrayWithStrings()
{
    stringArray result;
    int arrayAllocatedSize = 100;
    int realArraySize = 0;

    char** array = (char**) malloc(arrayAllocatedSize * sizeof(char*));
    for (LONGLONG i = 0; i < fileSize; )
    {   
        char* strPointer = (char*)fileMapPointer + i;

        if (realArraySize + 1 == arrayAllocatedSize)
        {
            arrayAllocatedSize <<= 1;
            array = (char**)realloc(array, arrayAllocatedSize * sizeof(char*));
        } 

        array[realArraySize] = strPointer;
        realArraySize++;

        LONGLONG strLength = strLineLength(strPointer, NULL);
        i += strLength + 2;
    
    }

    array = (char**)realloc(array, realArraySize*(sizeof(char*)));
    result.arrayPointer = array;
    result.arrayLength = realArraySize;
    return result;
}

sortLayers* FormSortLayers(int numberOfElements)
{
    if (numberOfElements <= 1)
    {
        return NULL;
    }

    sortLayers* result;
    result = (sortLayers*)malloc(sizeof(sortLayers));
    result->ptOnNextLayer = NULL;
    result->ptOnSortBoards = (sortBoards*)malloc(sizeof(sortBoards));
    
    result->ptOnSortBoards->ptOnNext = NULL;
    result->ptOnSortBoards->leftBoard = 0;
    result->ptOnSortBoards->rightBoard = numberOfElements - 1;
    result->countBoards = 1;

    sortLayers* nextLayer = NULL;
    do
    {
        sortBoards* boardOnLayer = result->ptOnSortBoards;
        nextLayer = NULL;
        while(boardOnLayer != NULL)
        {
            int count = (boardOnLayer->rightBoard - boardOnLayer->leftBoard + 1);
            int newCount = count / 2;

            
            sortBoards* tempBoardsPointer;
            //Left brunch
            if (newCount > 1)
            {
                if (nextLayer == NULL)
                {
                    nextLayer = (sortLayers*)malloc(sizeof(sortLayers));
                    nextLayer->ptOnNextLayer = result;
                    nextLayer->ptOnSortBoards = NULL;
                    nextLayer->countBoards = 0;
                }
                
                tempBoardsPointer = (sortBoards*)malloc(sizeof(sortBoards));
                tempBoardsPointer->leftBoard = boardOnLayer->leftBoard;
                tempBoardsPointer->rightBoard = boardOnLayer->leftBoard + newCount - 1;
                tempBoardsPointer->ptOnNext = nextLayer->ptOnSortBoards;

                nextLayer->ptOnSortBoards = tempBoardsPointer;
                nextLayer->countBoards++;
            }

            //Right brunch
            if (count - newCount > 1)
            {
                if (nextLayer == NULL)
                {
                    nextLayer = (sortLayers*)malloc(sizeof(sortLayers));
                    nextLayer->ptOnNextLayer = result;
                    nextLayer->ptOnSortBoards = NULL;
                    nextLayer->countBoards = 0;
                }

                tempBoardsPointer = (sortBoards*)malloc(sizeof(sortBoards));
                tempBoardsPointer->leftBoard = boardOnLayer->leftBoard + newCount;
                tempBoardsPointer->rightBoard = boardOnLayer->rightBoard;
                tempBoardsPointer->ptOnNext = nextLayer->ptOnSortBoards;

                nextLayer->ptOnSortBoards = tempBoardsPointer; 
                nextLayer->countBoards++;
            }

            boardOnLayer = boardOnLayer->ptOnNext;
        }

        if (nextLayer != NULL)
        {
            result = nextLayer;
        }

    } while (nextLayer != NULL);
    

    return result;
}


typedef struct _manageBoardsParams
{
    Laba4::ThreadManager * threadManager;
    sortLayers* sortLayerParam;
} manageBoardsParams;


int strLineCmp(const char* str1, const char* str2)
{
    unsigned char* _str1 = (unsigned char*)str1;
    unsigned char* _str2 = (unsigned char*)str2;

    int res = _str1[0] - _str2[0];

    while ((_str1[0] != '\0' && (_str1[0] != '\r' || _str1[1] != '\n'))  &&
           (_str2[0] != '\0' && (_str2[0] != '\r' || _str2[1] != '\n')) && res == 0)
    {
        res = _str1++[0] - _str2++[0];
    }

    return res;
}

int MergeBoards(void * boards)
{

    sortBoards* boardsArg = (sortBoards*) boards;
    int leftBoard = boardsArg->leftBoard;
    int rightBoard = boardsArg->rightBoard;
    int count = (rightBoard - leftBoard + 1);
    int delta = count / 2;
    
    int threadId = GetThreadId(GetCurrentThread());
    printf("I thread %d start sort %d and %d boards\n", threadId, leftBoard, rightBoard);

    char** arrayStrings = stringsForSorting.arrayPointer;
   
    bool isLineEnd;
    int rightBoardStrLenght = strLineLength(arrayStrings[rightBoard], &isLineEnd);
    int bufLength = arrayStrings[rightBoard] + rightBoardStrLenght - arrayStrings[leftBoard] + (isLineEnd ? 1 : 2);
    char* stringsBuffer = (char*) malloc(bufLength);

    int rightBrunchOffset = leftBoard + delta;
    int leftBrunchRightBoard = leftBoard + delta;
    
    char* leftBoardPointer = arrayStrings[leftBoard];
    char* tempPointerOffset = leftBoardPointer;
    char* bufferOffset = stringsBuffer;
    for (int i = leftBoard; i <= rightBoard; i++)
    {
        int countByteForWriting;
        char* srcForWrite = NULL;
        if (i < leftBrunchRightBoard && rightBrunchOffset <= rightBoard)
        {
            int compResult = strLineCmp(arrayStrings[i], arrayStrings[rightBrunchOffset]); 
            if (compResult > 0)
            {
                srcForWrite = arrayStrings[rightBrunchOffset];
                countByteForWriting = strLineLength(srcForWrite, NULL);

                char* tempValue = arrayStrings[rightBrunchOffset];
                for (int j = rightBrunchOffset - 1; j >= i; j-- )
                {
                    arrayStrings[j + 1] = arrayStrings[j];
                }

                leftBrunchRightBoard++;
                rightBrunchOffset++;
            }
            else
            {
                srcForWrite = arrayStrings[i];
                countByteForWriting = strLineLength(srcForWrite, NULL);
            }
        }
        else
        {
            srcForWrite = arrayStrings[i];
            countByteForWriting = strLineLength(srcForWrite, NULL);
        }

        memcpy(bufferOffset, srcForWrite, countByteForWriting);
        
        bufferOffset += countByteForWriting + 2;
        memcpy(bufferOffset - 2, "\r\n", 2);
        
        arrayStrings[i] = tempPointerOffset;
        tempPointerOffset += countByteForWriting + 2;
        
    }

    memcpy(leftBoardPointer, stringsBuffer, bufLength);

    // Wake up
    EnterCriticalSection(&sortCritSection);
    int countProcessing = InterlockedDecrement(&currentBoardProcess);
    if (!countProcessing)
    {
        WakeConditionVariable(&sortCondVariable);
    }
    LeaveCriticalSection(&sortCritSection);

    free(stringsBuffer);
    printf("I thread %d end sort %d and %d boards\n", threadId, leftBoard, rightBoard);
    return 0;
}

int ManageBoards(void * manageBoardsArgs)
{
    sortLayers* layer = ((manageBoardsParams *)manageBoardsArgs)->sortLayerParam;
    Laba4::ThreadManager* threadManager = ((manageBoardsParams *)manageBoardsArgs)->threadManager;
    threadManager->StartThreadsWork();

    while (layer != NULL)
    {
        sortBoards* boards = layer->ptOnSortBoards;

        currentBoardProcess = layer->countBoards;
        
        EnterCriticalSection(&sortCritSection);
        while (boards != NULL)
        {
            TASK_DATA taskData;
            taskData.function = MergeBoards;
            taskData.params = boards;
            threadManager->EnqueueTask(taskData);
 
            boards = boards->ptOnNext;
        }

        SleepConditionVariableCS(&sortCondVariable, &sortCritSection, INFINITE);
        LeaveCriticalSection(&sortCritSection);

        layer = layer->ptOnNextLayer;
    }
    
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Error! Incorrect count of arguments\nExample: [file name]\n");
        return 1;
    }

    //Free resources
    if (!InitializeMemoryView(argv[1]))
    {
        freeResources();
        return 1;
    }

    stringsForSorting = FormArrayWithStrings();
    InitializeConditionVariable(&sortCondVariable);
    InitializeCriticalSection(&sortCritSection);
    Laba4::ThreadManager * threadManager = new Laba4::ThreadManager(4);
    sortLayers* layersForArraySorting = FormSortLayers(stringsForSorting.arrayLength);      
    manageBoardsParams argsForBoardsManager = { threadManager,  layersForArraySorting }; 
  
    ManageBoards(&argsForBoardsManager);    

    printf("sort ended\n");
    
    delete threadManager;
    freeResources();
    return 0;
}
