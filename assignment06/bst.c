#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"


// internal functions (not mandatory)
// used in BST_Insert
static int _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *));


// used in BST_Insert
static NODE *_makeNode( void *dataInPtr){
    NODE* newNode = malloc(sizeof(NODE));
    if(newNode == NULL) return NULL;
    newNode->dataPtr = dataInPtr;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *)){
    if(root == NULL) return;
    _destroy(root->left, callback);
    callback(root->dataPtr);
    _destroy(root->right, callback);
    free(root);
}


// used in BST_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *));

// used in BST_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *));

// used in BST_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)){ // check it!
    if(root == NULL) return;
    _traverse(root->left, callback);
    callback(root->dataPtr);
    _traverse(root->right, callback);
}

// used in BST_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *)){
    if(root == NULL) return;
    _traverseR(root->right, callback);
    callback(root->dataPtr);
    _traverseR(root->left, callback);
}

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)){
    if(root == NULL) return;
    level++;

    _inorder_print(root->right, level, callback);
    for(int i = 0; i<level; i++){
        printf("\t");
    }
    callback(root->dataPtr);
    _inorder_print(root->left, level, callback);
}


// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( int (*compare)(const void *, const void *)){
    TREE *newtree = malloc(sizeof(TREE));
    if(newtree == NULL)
        return NULL;
    newtree->count = 0;
    newtree->root = NULL;
    newtree->compare = compare;
    return newtree;
}

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree, void (*callback)(void *)){
    _destroy(pTree->root, callback);
    free(pTree);
}

/* Inserts new data into the tree
	callback은 이미 트리에 존재하는 데이터를 발견했을 때 호출하는 함수
	return	0 overflow
			1 success
			2 if duplicated key
*/
int BST_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *)){
    NODE * newNode = _makeNode(dataInPtr);
    if(newNode == NULL) return 0;

    NODE* pNode = pTree->root;

    // empty tree
    if(pTree->root == NULL){
        pTree->root = newNode;
        pTree->count++;
        return 1;
    }
    // non-empty tree
    int cmp;
    while(1){ // check free!
        cmp = pTree->compare(newNode->dataPtr, pNode->dataPtr);
        if(cmp > 0){
            if(pNode->right == NULL){
                pNode->right = newNode;
                pTree->count++;
                return 1;
            } else pNode = pNode->right;
        }
        else if(cmp < 0){
            if(pNode->left == NULL){
                pNode->left = newNode;
                pTree->count++;
                return 1;
            } else pNode = pNode->left;
        }
        else{
            callback(pNode->dataPtr);
            free(newNode);
            return 2;
        }
    }
}

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Delete( TREE *pTree, void *keyPtr){ // check free when you delete it.
    NODE* pPre = NULL;
    NODE* pNode = pTree->root;
    NODE* out = NULL;
    int cmp;
    while(pNode != NULL){
        cmp = pTree->compare(keyPtr, pNode->dataPtr);

        if(cmp == 0) {
            out = pNode->dataPtr;

            if(pNode->left != NULL && pNode->right != NULL){ // case1. both r and l have a child.
                NODE* min= pNode->right;
                NODE* minPre = pNode;
                while(min->left != NULL){
                    minPre = min;
                    min = min->left;
                }
                pNode->dataPtr = min->dataPtr;
                if(minPre->left == min)
                    minPre->left = min->right;
                else
                    minPre->right = min->right;
                free(min);
            }
            else if (pNode->left == NULL && pNode->right == NULL) { // case2. leaf node
                if (pPre == NULL) {
                    pTree->root = NULL;
                } else if (pPre->left == pNode) {
                    pPre->left = NULL;
                } else {
                    pPre->right = NULL;
                }
                free(pNode);
            } else { // case3. one child
                NODE *child = (pNode->left != NULL) ? pNode->left : pNode->right;
                if (pPre == NULL) {
                    pTree->root = child;
                } else if (pPre->left == pNode) {
                    pPre->left = child;
                } else {
                    pPre->right = child;
                }
                free(pNode);
            }
            pTree->count--;
            return out;
        }
        else if(cmp > 0){
            pPre = pNode;
            pNode = pNode->right;
        }
        else {
            pPre = pNode;
            pNode = pNode->left;
        }
    }
    return NULL;
}

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Search( TREE *pTree, void *keyPtr){
    NODE* pNode = pTree->root;
    int cmp;
    while(pNode != NULL){
        cmp = pTree->compare(keyPtr, pNode->dataPtr);
        if(cmp == 0){
            return pNode->dataPtr;
        }
        else if(cmp > 0) pNode = pNode->right;
        else pNode = pNode->left;
    }
    return NULL;
}

/* prints tree using inorder traversal
*/
void BST_Traverse( TREE *pTree, void (*callback)(const void *)){
    _traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
*/
void BST_TraverseR( TREE *pTree, void (*callback)(const void *)){
    _traverseR(pTree->root, callback);
}

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)){
    int level = 0;
    _inorder_print(pTree->root, level, callback);
}

/* returns number of nodes in tree
*/
int BST_Count( TREE *pTree){
    return pTree->count;
}

