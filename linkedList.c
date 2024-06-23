#include "stdio.h"
#include "string.h"
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef unsigned short uint16;
#define BUFFER_SIZE_BYTES 1024UL
typedef struct
{
    void* nextNode;
    void* preNode;
    uint32 data;
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

Node* allocateNode()
{
    Node* currentNodePtr;
    uint8 step;
    step = sizeof(Node);
    /*Node is free when Node is not linked with another Node*/
    for (uint8 index = 0; index < BUFFER_SIZE_BYTES; index = index + step)
    {
        currentNodePtr = (Node*)(&bufferEntryPtr[index]);
        if ((NULL == currentNodePtr->nextNode) && (NULL == currentNodePtr->preNode))
        {
            /*when we only have one Node, this Node shall be not linked with another Node*/
            /*Avoid wrong detection*/
            if ( currentNodePtr != listEntryPtr)
            {
                /*This node is free*/
                break;
            }
        }
    }
    if ((NULL == currentNodePtr->nextNode) && (NULL == currentNodePtr->preNode))
    {
        /*Full of stack*/
    }
    else
    {
        currentNodePtr = NULL; 
    }
    return currentNodePtr;
}

uint8 insertNewData(uint32 idata)
{
    Node* currentNodePtr;
    Node* allocatedNodePtr;
    uint8 ret;
    uint8 isHigher = 0;
    ret = 0;
    currentNodePtr = (Node*)listEntryPtr;

    /*first Node..............AllocatedNode.................last Node*/
    /*List is empty*/
    if ( NULL == currentNodePtr)
    {
        allocatedNodePtr = allocateNode();
        allocatedNodePtr->nextNode = NULL;
        allocatedNodePtr->preNode = NULL;
        allocatedNodePtr->data = idata;
        listEntryPtr = allocatedNodePtr;
    }
    else
    {
        /*Scan all duplicated Node*/
        while ( NULL != currentNodePtr->nextNode )
        {
            if ( currentNodePtr->data == idata )
            {
                /*Duplicated data*/
                ret = 1;
                break;
            }
            else if ( (currentNodePtr->data > idata))
            {
                isHigher = 1;
                /*Keep current node*/
                break;
            }
            currentNodePtr = (Node*)currentNodePtr->nextNode;
        }
        if (currentNodePtr->data > idata)
        {
            isHigher = 1;
        }
        /*There is no duplicated data*/
        if ( 0 == ret )
        {
            allocatedNodePtr = allocateNode();
            if (NULL == allocatedNodePtr)
            {
                ret = 1;
                printf("Full stack");
            }
            else
            {
                if ( 1 == isHigher )
                {
                    allocatedNodePtr->data = idata;
                    allocatedNodePtr->nextNode = currentNodePtr;
                    /*first Node is higher Node*/
                    if ( NULL == currentNodePtr->preNode )
                    {
                        allocatedNodePtr->preNode = NULL;
                        currentNodePtr->preNode = allocatedNodePtr;
                        listEntryPtr = allocatedNodePtr;
                    }
                    else
                    {
                        allocatedNodePtr->preNode = currentNodePtr->preNode;
                        ((Node*)(currentNodePtr->preNode))->nextNode = allocatedNodePtr;
                        currentNodePtr->preNode = allocatedNodePtr;
                    }
                }
                else /* append allocated Node to last list */
                {
                    allocatedNodePtr->data = idata;
                    allocatedNodePtr->nextNode = NULL;
                    allocatedNodePtr->preNode = currentNodePtr;
                    currentNodePtr->nextNode = allocatedNodePtr;
                    
                }
            }
        }

    }
    return ret;
}
uint8 removeNode(uint32 idata)
{
    uint8 ret = 1;
    Node* currentNodePtr;
    currentNodePtr = (Node*)listEntryPtr;
    if (NULL == currentNodePtr)
    {
        ret = 1;
    }
    else
    {
        while (NULL != currentNodePtr)
        {
            if (idata == currentNodePtr->data)
            {
                if (NULL != currentNodePtr->nextNode)
                {
                    ((Node*)(currentNodePtr->nextNode))->preNode = currentNodePtr->preNode;
                }
                if (NULL != currentNodePtr->preNode)
                {
                    ((Node*)(currentNodePtr->preNode))->nextNode = currentNodePtr->nextNode;
                }
                /*Unlink for this node*/
                currentNodePtr->nextNode = NULL;
                currentNodePtr->preNode = NULL;
                ret = 0;
                break;
            }
            currentNodePtr = currentNodePtr->nextNode;
        }
    }
    return ret;
}
void printList()
{
    Node* currentNodePtr;;
    if (NULL != listEntryPtr)
    {
        currentNodePtr = (Node*)listEntryPtr;
        while ( NULL != currentNodePtr )
        {
            printf("%d ", currentNodePtr->data);
            currentNodePtr = currentNodePtr->nextNode;
        }
    }
}
void main()
{
    if ( 1 != isInit )
    {
        init();
        isInit = 1;
    }
    insertNewData(14);
    insertNewData(1);
    insertNewData(3);
    insertNewData(15);
    insertNewData(16);
    insertNewData(5);
    removeNode(16);
    printList();
}