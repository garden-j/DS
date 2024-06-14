#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free
#include <string.h>

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
   for heap_Insert function
*/
static void _reheapUp( HEAP *heap, int index){
    if(index != 0){
        int parent = (index-1)/2;
        int cmp = heap->compare(heap->heapArr[index], heap->heapArr[parent]);
        if(cmp > 0){
            // index랑 parent랑 값 바꾸기
            void* tmp = heap->heapArr[index];
            heap->heapArr[index] = heap->heapArr[parent];
            heap->heapArr[parent] = tmp;
            _reheapUp(heap, parent);
        }
    }
}

/* Reestablishes heap by moving data in root down to its correct location in the heap
   for heap_Delete function
*/
static void _reheapDown( HEAP *heap, int index){
    int child = 2 * index + 1;

    // subtree 있는지 여부 검사
    if(child > heap->last) return;

    if(child == heap->last){
        int compare = heap->compare(heap->heapArr[index], heap->heapArr[child]);
        if(compare < 0){
            void* tmp = heap->heapArr[index];
            heap->heapArr[index] = heap->heapArr[child];
            heap->heapArr[child] = tmp;
        }
        return;
    }

    int cmp = heap->compare(heap->heapArr[child], heap->heapArr[child+1]);
    if(cmp < 0) child++;
    int compare = heap->compare(heap->heapArr[index], heap->heapArr[child]);
    if(compare < 0){
        void* tmp = heap->heapArr[index];
        heap->heapArr[index] = heap->heapArr[child];
        heap->heapArr[child] = tmp;
        _reheapDown(heap, child);
    }
}



/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
The initial capacity of the heap should be 10
*/
HEAP *heap_Create( int (*compare) (const void *arg1, const void *arg2)){
    HEAP* new = malloc(sizeof(HEAP));
    if(new == NULL) return NULL;
    new->compare = compare;
    new->capacity = 10;
    new->last = -1;
    new->heapArr = malloc(sizeof(void*) * new->capacity);
    if(new->heapArr == NULL) return NULL;
    return new;
}

/* Free memory for heap
*/
void heap_Destroy( HEAP *heap, void (*remove_data)(void *ptr)){
    for(int i = 0; i<heap->last+1; i++){
        remove_data(heap->heapArr[i]);
    }
    free(heap->heapArr);
    free(heap);
}

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heap_Insert( HEAP *heap, void *dataPtr){
    // capacity check!
    heap->last++;
    if(heap->last == heap->capacity){
        heap->capacity *= 2;
        void ** newarr = realloc(heap->heapArr, sizeof(void*) * heap->capacity);
        if(newarr == NULL){
            return 0;
        }
        heap->heapArr = newarr;
    }


    heap->heapArr[heap->last] = dataPtr;
    _reheapUp(heap, heap->last);

    return 1;
}

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete( HEAP *heap, void **dataOutPtr){
    // if heap is empty
    if(heap->last == -1) return 0;

    // is it right?
    *dataOutPtr = heap->heapArr[0]; // memory alloc?

    void* tmp = heap->heapArr[0];
    heap->heapArr[0] = heap->heapArr[heap->last];
    heap->heapArr[heap->last] = tmp;
    heap->last--;
    _reheapDown(heap, 0);
    return 1;
}

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(  HEAP *heap){
    if(heap->last == -1)
        return 1;
    else return 0;
}

/* Print heap array */
void heap_Print( HEAP *heap, void (*print_func) (const void *data)){
    for(int i = 0; i < heap->last+1; i++){
        print_func(heap->heapArr[i]);
    }
    printf("\n");
}



