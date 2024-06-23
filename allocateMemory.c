#include "stdio.h"
#include "string.h"
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned long uint64;
typedef unsigned short uint16;
#define BUFFER_SIZE_BYTES 1024UL
typedef struct
{
    void* nextNode;
    void* preNode;
    uint32 size;
    uint8* dataPtr;
} Node;

uint8 isInit = 0;
void* listEntryPtr;
uint8* bufferEntryPtr;
void init()
{
    static uint8 stack_buffer[BUFFER_SIZE_BYTES];
    listEntryPtr = NULL;
    bufferEntryPtr = (uint8*)stack_buffer;
    memset(bufferEntryPtr,0,BUFFER_SIZE_BYTES);
}
uint32 calculateArea(Node* firstNodePtr, Node* secondNodePtr)
{
    uint32 retsize;
    uint8* secondaddress;
    uint8* firstaddress;
    secondaddress = (uint8*)&(secondNodePtr->nextNode);
    firstaddress = (uint8*)&(firstNodePtr->nextNode);
    retsize = secondaddress - (firstaddress + sizeof(Node) + firstNodePtr->size);
    return retsize;
}
uint8* allocateMemory(uint32 size)
{
    Node* currentNodePtr;
    Node* allocatedNodePtr = NULL;
    uint8* retPtr;
    uint32 endPoint;
    /*First Node*/
    if (NULL == listEntryPtr)
    {
        if (size < (BUFFER_SIZE_BYTES - sizeof(Node)))
        {
            listEntryPtr = bufferEntryPtr;
            allocatedNodePtr = (Node*)listEntryPtr;
            allocatedNodePtr->dataPtr = (uint8*)allocatedNodePtr + sizeof(Node);
            allocatedNodePtr->nextNode = NULL;
            allocatedNodePtr->preNode = NULL;  
            allocatedNodePtr->size = size;
        }  
    }
    else
    {
        currentNodePtr = (Node*)listEntryPtr;
        /*First, find the valid area between Nodes if possible*/
        while (NULL != currentNodePtr->nextNode)
        {
            if (size < calculateArea(currentNodePtr, currentNodePtr->nextNode))
            {
                allocatedNodePtr = (Node*)((uint8*)currentNodePtr + sizeof(Node) + currentNodePtr->size);
                allocatedNodePtr->nextNode = currentNodePtr->nextNode;
                allocatedNodePtr->preNode = currentNodePtr;
                allocatedNodePtr->dataPtr = (uint8*)allocatedNodePtr + sizeof(Node);
                ((Node*)(currentNodePtr->nextNode))->preNode = allocatedNodePtr;
                currentNodePtr->nextNode = allocatedNodePtr;
                allocatedNodePtr->size = size;
                break;
            }
            currentNodePtr = currentNodePtr->nextNode;
        }
        if (NULL == allocatedNodePtr)
        {
            endPoint = ((uint8*)currentNodePtr + sizeof(Node) + currentNodePtr->size) + sizeof(Node) + size;
            if ( endPoint < (bufferEntryPtr + BUFFER_SIZE_BYTES))
            {
                allocatedNodePtr = ((uint8*)currentNodePtr + sizeof(Node) + currentNodePtr->size);
                allocatedNodePtr->nextNode = NULL;
                allocatedNodePtr->preNode = currentNodePtr;
                allocatedNodePtr->dataPtr = (uint8*)allocatedNodePtr + sizeof(Node);
                currentNodePtr->nextNode = allocatedNodePtr;
                allocatedNodePtr->size = size;
            }
        }
    }
    if (NULL != allocatedNodePtr)
    {
        return allocatedNodePtr->dataPtr;
    }
    else
    {
        return NULL;
    }
}

uint8 freeMemory(uint8* dataPtr)
{
    Node* currentNodePtr;
    uint8 ret;
    currentNodePtr = (Node*)listEntryPtr;
    ret  = 1;
    while (NULL != currentNodePtr->nextNode)
    {
        if (dataPtr == currentNodePtr->dataPtr)
        {
            /*Find out corresponding Node*/
            break;
        }
        currentNodePtr = currentNodePtr->nextNode;
    }
    if (dataPtr == currentNodePtr->dataPtr)
    {
        if (NULL != currentNodePtr->preNode)
        {
            ((Node*)currentNodePtr->preNode)->nextNode = currentNodePtr->nextNode;
        }
        if (NULL != currentNodePtr->nextNode)
        {
            ((Node*)currentNodePtr->nextNode)->preNode = currentNodePtr->preNode;
        }
        currentNodePtr->dataPtr = NULL;
        currentNodePtr->nextNode = NULL;
        currentNodePtr->preNode = NULL;
        currentNodePtr->size = 0;
        ret = 0;
    }
    return ret;
}

void printData()
{
    Node* currentNodePtr;
    currentNodePtr = (Node*)listEntryPtr;
    uint32 index = 0;
    while (NULL != currentNodePtr)
    {
        for (index = 0; index < currentNodePtr->size;index++ )
        {
            printf("%d ", currentNodePtr->dataPtr[index]);
        }
        printf("\n");
        currentNodePtr = currentNodePtr->nextNode;
    }
}
void main()
{
    uint8* memoryPtr[5];
    if ( 1 != isInit )
    {
        init();
        isInit = 1;
    }
    memoryPtr[1] = allocateMemory(32);
    memset(memoryPtr[1], 123, 32);
    memoryPtr[2] = allocateMemory(64);
    memset(memoryPtr[2], 125, 64);
    memoryPtr[3] = allocateMemory(8);
    memset(memoryPtr[3], 127, 8);
    freeMemory(memoryPtr[2]);
    memoryPtr[4] = allocateMemory(32);
    memset(memoryPtr[4], 129, 64);
    printData();
}