//  LIST.C
//  Copyright (C) 2000 by YG Corp.
//  All rights reserved
//  Author: ZL
//  Date:   07-Mar-2000

#define WANTVXDWRAPS
#include    <basedef.h>
#include    <vmm.h>
//#include    <debug.h>
#include    <vxdwraps.h>
#include    "list.h"

PYG_LIST    YG_List_Create(DWORD flags,WORD size)
{
    PYG_LIST	pList;

    pList = (PYG_LIST)_HeapAllocate(sizeof(YG_LIST),flags);
    if(pList)
    {
	memset(pList,0,sizeof(YG_LIST));
	pList->flags = flags;
	pList->size = size;
    }
    return  pList;
}   //	YG_List_Create


PVOID	YG_List_Allocate(PYG_LIST pList)
{
    PBYTE   pNode;

    pNode = (PBYTE)_HeapAllocate(pList->size + sizeof(YG_HEAD),pList->flags);
    if(pNode)
    {
	pNode = pNode + sizeof(YG_HEAD);
    }
    return  pNode;
}   //	YG_List_Allocate


VOID	YG_List_Attach(PYG_LIST pList,PVOID pUser)
{
    PYG_HEAD	pSys = (PYG_HEAD)((PBYTE)pUser - sizeof(YG_HEAD));

    ASSERT(pSys->Sign == NODE_SIGNATURE);

    pSys->pPrev = NULL;
    pSys->pNext = pList->pHead;

    if(pList->pHead)
	((PYG_HEAD)(pList->pHead))->pPrev = pSys;

    pList->pHead = pSys;

    if(!pList->pTail)
	pList->pTail = pSys;

    return;
}   //	YG_List_Attach


VOID	YG_List_Attach_Tail(PYG_LIST pList,PVOID pUser)
{
    PYG_HEAD	pSys = (PYG_HEAD)((PBYTE)pUser - sizeof(YG_HEAD));

    ASSERT(pSys->Sign == NODE_SIGNATURE);

    pSys->pPrev = pList->pTail;
    pSys->pNext = NULL;
    if(!pList->pHead)
	pList->pHead = pSys;

    if(pList->pTail)
	((PYG_HEAD)(pList->pTail))->pNext = pSys;

    pList->pTail = pSys;

    return;
}   //	YG_List_Attach_Tail



VOID	YG_List_Deallocate(PYG_LIST pList,PVOID pUser)
{
    PYG_HEAD	pSys = (PYG_HEAD)((PBYTE)pUser - sizeof(YG_HEAD));

    ASSERT(pSys->Sign == NODE_SIGNATURE);

    _HeapFree(pSys,0);
    return;
}   //	YG_List_Deallocate


VOID	YG_List_Destroy(PYG_LIST pList)
{
    PYG_HEAD	pCur,pNext;

    pCur = (PYG_HEAD)pList->pHead;
    while(pCur)
    {
	pNext = (PYG_HEAD)pCur->pNext;
	_HeapFree(pCur,0);
	pCur = pNext;
    }
    _HeapFree(pList,0);
    return;
}   //	YG_List_Destroy


PVOID	YG_List_Get_First(PYG_LIST pList)
{
    PBYTE   pFirst;

    pFirst = (PBYTE)(pList->pHead);
    if(pFirst)
	pFirst = pFirst + sizeof(YG_HEAD);

    return  pFirst;
}   //	YG_List_Get_First


PVOID	YG_List_Get_Next(PYG_LIST pList,PVOID pUser)
{
    PYG_HEAD	pSys = (PYG_HEAD)((PBYTE)pUser - sizeof(YG_HEAD));
    PBYTE	pNext;

    ASSERT(pSys->Sign == NODE_SIGNATURE);

    pNext = (PBYTE)pSys->pNext;
    if(pNext)
	pNext = pNext + sizeof(YG_HEAD);

    return  pNext;
}   //	YG_List_Get_Next



PVOID	YG_List_Get_Prev(PYG_LIST pList,PVOID pUser)
{
    PYG_HEAD	pSys = (PYG_HEAD)((PBYTE)pUser - sizeof(YG_HEAD));
    PBYTE	pPrev;

    ASSERT(pSys->Sign == NODE_SIGNATURE);

    pPrev = (PBYTE)pSys->pPrev;
    if(pPrev)
	pPrev = pPrev + sizeof(YG_HEAD);

    return  pPrev;
}   //	YG_List_Get_Prev


VOID	YG_List_Insert(PYG_LIST pList,PVOID pUserNode,PVOID pUserPrev)
{
    PYG_HEAD	pSysNode,pSysPrev,pNext;

    if(NULL == pUserPrev)   //	insert pUserNode to list head
    {
	YG_List_Attach(pList,pUserNode);
	return;
    }

    pSysPrev = (PYG_HEAD)((PBYTE)pUserPrev - sizeof(YG_HEAD));
    ASSERT(pSysPrev->Sign == NODE_SIGNATURE);

    if(pList->pTail == pSysPrev) //  insert pUserNode to list tail
    {
	YG_List_Attach_Tail(pList,pUserNode);
	return;
    }

    pSysNode = (PYG_HEAD)((PBYTE)pUserNode - sizeof(YG_HEAD));
    ASSERT(pSysNode->Sign == NODE_SIGNATURE);

    pNext = pSysPrev->pNext;
    pSysNode->pPrev = pSysPrev;
    pSysNode->pNext = pNext;
    pSysPrev->pNext = pSysNode;
    pNext->pPrev = pSysNode;
    return;
}   //	YG_List_Insert


VOID	YG_List_Remove(PYG_LIST pList,PVOID pUser)
{
    PYG_HEAD	pSys = (PYG_HEAD)((PBYTE)pUser - sizeof(YG_HEAD));

    ASSERT(pSys->Sign == NODE_SIGNATURE);

    if(pSys == pList->pHead) //  remove the first node
	pList->pHead = pSys->pNext;

    if(pSys == pList->pTail) //  remove  the last node
	pList->pTail = pSys->pPrev;

    if(pSys->pPrev)
	((PYG_HEAD)(pSys->pPrev))->pNext = pSys->pNext;

    if(pSys->pNext)
	((PYG_HEAD)(pSys->pNext))->pPrev = pSys->pPrev;

    return;
}   //	YG_List_Remove
