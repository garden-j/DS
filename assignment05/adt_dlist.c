#include <stdlib.h> // malloc

#include "adt_dlist.h"



// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr){ //here is the problem point. dataInPtr is a tWord pointer maybe..?
    NODE * newNode = malloc(sizeof(NODE));
    if(newNode == NULL)
        return 0;
    newNode->dataPtr = dataInPtr;
    newNode->rlink = NULL;
    newNode->llink = NULL;

    int empty = emptyList(pList);

    if(empty == 1){
        pList->head = newNode;
        pList->rear = newNode;
    }

    else if(pPre == NULL){ // first
        newNode->rlink = pList->head;

        pList->head->llink = newNode;
        pList->head = newNode;
    }

    else if(pPre == pList->rear){ // last
        newNode->llink = pList->rear;

        pList->rear->rlink = newNode;
        pList->rear = newNode;
    }
    else{ // middle
        newNode->llink = pPre;
        newNode->rlink = pPre->rlink;

        pPre->rlink->llink = newNode;
        pPre->rlink = newNode;
    }
    pList->count++;
    return 1;
}

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr){
    if(pPre == NULL){ // delete first
        pList->head = pLoc->rlink;
        pLoc->rlink->llink = NULL;
    }
    else if(pLoc == pList->rear){ // delete last
        pList->rear = pLoc->llink;
        pLoc->llink->rlink = NULL;
    }

    else{ // delete middle
        pLoc->rlink->llink = pPre;
        pPre->rlink = pLoc->rlink;
    }

    *dataOutPtr = pLoc->dataPtr;
    pLoc->rlink = NULL;
    pLoc->llink = NULL;
    free(pLoc);
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu) {
    *pPre = NULL;
    *pLoc = pList->head;

    if (pList->head == NULL) // In insert function, this case must be handled in other case.
        return 0;
    int result;
    while (*pLoc != NULL) {
        result = pList->compare((*pLoc)->dataPtr, pArgu); //need to be fixed
        if (result < 0) {
            *pPre = *pLoc;
            *pLoc = (*pLoc)->rlink;
        }else break;
    }
    if(result == 0){
        return 1;
    }
    else
        return 0;
}


////////////////////////////////////////////////////////////////////////////////
// function declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)){
    LIST * nList = malloc(sizeof(LIST));
    nList->head = NULL;
    nList->rear = NULL;
    nList->count = 0;
    nList->compare = (*compare);
    return nList;
}

//  이름 리스트에 할당된 메모리를 해제 (head node, data node)
void destroyList( LIST *pList, void (*callback)(void *)){
    NODE *pNode = pList->head;
    NODE *tmp;
    while(pNode != NULL){
        tmp = pNode;
        callback(pNode->dataPtr);
        pNode = pNode->rlink;
        free(tmp);
    }
    free(pList);
}

// Inserts data into list
// callback은 이미 리스트에 존재하는 데이터를 발견했을 때 호출하는 함수
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr, void (*callback)(const void *)){

    NODE * newNode = malloc(sizeof(NODE));
    if(newNode == NULL)
        return 0;
    newNode->rlink = NULL;
    newNode->llink = NULL;
    newNode->dataPtr = dataInPtr;

    NODE *pPre, *pLoc;

    int searched = _search(pList, &pPre, &pLoc, dataInPtr);

    if(searched == 1){
        callback(pLoc->dataPtr);// check cast requirement
        free(newNode);
        return 2;
    }
    else{
        _insert(pList, pPre, dataInPtr);
        free(newNode);
        return 1;
    }
}

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr){
    NODE *pPre, *pLoc;
    int searched = _search(pList, &pPre, &pLoc, keyPtr);
    if(searched == 0)
        return 0;
    else{
        *dataOutPtr = keyPtr; //check one more time.
        _delete(pList, pPre, pLoc, dataOutPtr);
        pList->count--;
        return 1;
    }
}

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr){
    NODE *pPre, *pLoc;
    int searched = _search(pList, &pPre, &pLoc, pArgu);
    if(searched == 0)
        return 0;
    else{
        *dataOutPtr = pLoc->dataPtr;
        return 1;
    }
}

// returns number of nodes in list
int countList( LIST *pList){
    int num = pList->count;
    return num;
}

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList){
    if(pList->count == 0)
        return 1;
    else
        return 0;
}

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)){
    NODE *pPre = pList->head;
    while(pPre != NULL){
        callback(pPre->dataPtr);
        pPre = pPre->rlink;
    }
}

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)){
    NODE *pPre = pList->rear;
    while(pPre != NULL){
        callback(pPre->dataPtr);
        pPre = pPre->llink;
    }
}



