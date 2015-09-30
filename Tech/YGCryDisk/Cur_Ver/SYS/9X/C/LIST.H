//  LIST.H
//  Copyright (C) 2000 by YG Corp.
//  All rights reserved
//  Author: ZL
//  Date:   07-Mar-2000

#ifndef LIST_H_
#define LIST_H_


#define LISTFIXED	0
#define LISTLOCKEDIFDP	HEAPLOCKEDIFDP
#define LISTINIT	HEAPINIT
#define LISTSWAP	HEAPSWAP
#define LISTZEROINIT	HEAPZEROINIT


typedef struct	tagYG_LIST
{
    PVOID	pHead;
    PVOID	pTail;
    DWORD	flags;
    WORD	size;
} YG_LIST,*PYG_LIST;


#define NODE_SIGNATURE	0x5453494C

typedef struct	tagYG_HEAD
{
    PVOID	pPrev;
    PVOID	pNext;
    #ifdef  DEBUG
    DWORD	Sign;	    //	"LIST"	== 0x5453494C
    #endif
} YG_HEAD,*PYG_HEAD;


PYG_LIST    YG_List_Create(DWORD flags,WORD size);
PVOID	    YG_List_Allocate(PYG_LIST pList);
VOID	    YG_List_Attach(PYG_LIST pList,PVOID pNode);
VOID	    YG_List_Attach_Tail(PYG_LIST pList,PVOID pNode);
VOID	    YG_List_Deallocate(PYG_LIST pList,PVOID pNode);
VOID	    YG_List_Destroy(PYG_LIST pList);
PVOID	    YG_List_Get_First(PYG_LIST pList);
PVOID	    YG_List_Get_Next(PYG_LIST pList,PVOID pNode);
PVOID	    YG_List_Get_Prev(PYG_LIST pList,PVOID pNode);
VOID	    YG_List_Insert(PYG_LIST pList,PVOID pNode,PVOID pPrev);
VOID	    YG_List_Remove(PYG_LIST pList,PVOID pNode);

#endif	//  LIST_H_
