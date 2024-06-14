#define BALANCING

#include <stdlib.h> // malloc
#include <stdio.h>

#include "avlt.h"

#define max(x, y)	(((x) > (y)) ? (x) : (y))

// internal functions (not mandatory)
// used in AVLT_Insert
// return 	pointer to root
static NODE *_insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *), int *duplicated);

// used in AVLT_Insert
static NODE *_makeNode( void *dataInPtr);

// used in AVLT_Destroy
static void _destroy( NODE *root, void (*callback)(void *));

// used in AVLT_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *));

// used in AVLT_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *));

// used in AVLT_Traverse
static void _traverse( NODE *root, void (*callback)(const void *));

// used in AVLT_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *));

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *));

// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight( NODE *root);

// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes
// return	new root
static NODE *rotateRight( NODE *root);

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft( NODE *root);

static NODE *_insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *), void (*callback)(void *), int *duplicated){
    if(root == NULL)
        return newPtr;

    int cmp = compare(newPtr->dataPtr, root->dataPtr);

    if(cmp == 0){
        callback(root->dataPtr);
        *duplicated = 2;
        return root;
    }
    if(cmp < 0){
        root->left = _insert(root->left, newPtr, compare, callback, duplicated);
    }
    else{
        root->right = _insert(root->right, newPtr, compare, callback, duplicated);
    }

    // Update height
    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;

    // Rebalance
    int balance = getHeight(root->left) - getHeight(root->right);

    if(balance > 1 && compare(newPtr->dataPtr, root->left->dataPtr) < 0)
        return rotateRight(root);
    if(balance < -1 && compare(newPtr->dataPtr, root->right->dataPtr) > 0)
        return rotateLeft(root);
    if(balance > 1 && compare(newPtr->dataPtr, root->left->dataPtr) > 0){
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    if(balance < -1 && compare(newPtr->dataPtr, root->right->dataPtr) < 0){
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

// used in AVLT_Insert
static NODE *_makeNode( void *dataInPtr){
    NODE *newNode = malloc(sizeof(NODE));
    newNode->dataPtr = dataInPtr;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->height = 1;
    return newNode;
}

// used in AVLT_Destroy
static void _destroy( NODE *root, void (*callback)(void *)){
    if(root == NULL) return;

    _destroy(root->left, callback);
    callback(root->dataPtr);
    _destroy(root->right, callback);
    free(root);
}


// used in AVLT_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)){
    if (root == NULL)
        return NULL;

    int cmp = compare(keyPtr, root->dataPtr);
    if (cmp > 0)
        root->right = _delete(root->right, keyPtr, dataOutPtr, compare);
    else if (cmp < 0)
        root->left = _delete(root->left, keyPtr, dataOutPtr, compare);
    else {
        *dataOutPtr = root->dataPtr;
        if (root->left == NULL) {
            NODE *tmp = root->right;
            free(root);
            return tmp;
        } else if (root->right == NULL) {
            NODE *tmp = root->left;
            free(root);
            return tmp;
        } else {
            NODE *sml = root->right;
            while (sml->left != NULL) {
                sml = sml->left;
            }
            root->dataPtr = sml->dataPtr;
            void *datainout;
            root->right = _delete(root->right, sml->dataPtr, &datainout, compare);
        }
    }

    // Update height of the current node
    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;

    return root;
}


// used in AVLT_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)){
    if(root == NULL)
        return NULL;

    int cmp = compare(keyPtr, root->dataPtr);
    if(cmp == 0)
        return root;

    if(cmp > 0)
        return _search(root->right, keyPtr, compare);
    else
        return _search(root->left, keyPtr, compare);
}

// used in AVLT_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)){
    if(root == NULL) return;

    _traverse(root->left, callback);
    callback(root->dataPtr);
    _traverse(root->right, callback);
}

// used in AVLT_TraverseR
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
    for(int i = 0; i < level; i++)
        printf("\t");
    callback(root->dataPtr);
    _inorder_print(root->left, level, callback);
}

// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight( NODE *root){

    if(root == NULL) return 0;
    return root->height;
}

// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes
// return	new root
static NODE *rotateRight( NODE *root){
    NODE *pNode = root->left;
    root->left = pNode->right;
    pNode->right = root;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    pNode->height = max(getHeight(pNode->left), getHeight(pNode->right)) + 1;

    return pNode;
}

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft( NODE *root){
    NODE *pNode = root->right;
    root->right = pNode->left;
    pNode->left = root;

    root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
    pNode->height = max(getHeight(pNode->left), getHeight(pNode->right)) + 1;

    return pNode;
}

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *AVLT_Create( int (*compare)(const void *, const void *)){
    TREE *new = malloc((sizeof(TREE)));
    if(new == NULL)
        return NULL;
    new->compare = compare;
    new->root = NULL;
    new->count = 0;
    return new;
}

/* Deletes all data in tree and recycles memory
*/
void AVLT_Destroy( TREE *pTree, void (*callback)(void *)){
    _destroy(pTree->root, callback);
    free(pTree);
}

/* Inserts new data into the tree
	callback은 이미 트리에 존재하는 데이터를 발견했을 때 호출하는 함수
	return	1 success
			0 overflow
			2 if duplicated key
*/
int AVLT_Insert( TREE *pTree, void *dataInPtr, void (*callback)(void *)){
    NODE *new = _makeNode(dataInPtr);
    if(new == NULL)
        return 0;

    if(pTree->root == NULL){
        pTree->root = new;
        pTree->count++;
        return 1;
    }

    int duplicated = 0;
    pTree->root = _insert(pTree->root, new, pTree->compare, callback, &duplicated);

    if(duplicated == 2){
        free(new);
        return 2;
    }
    else {
        pTree->count++;
        return 1;
    }
}

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Delete( TREE *pTree, void *keyPtr){
    void *dataOut;
    pTree->root = _delete(pTree->root, keyPtr, &dataOut, pTree->compare);
    if (dataOut != NULL) {
        pTree->count--;
    }
    return dataOut;
}

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Search( TREE *pTree, void *keyPtr){
    NODE *searched = _search(pTree->root, keyPtr, pTree->compare);
    return searched ? searched->dataPtr : NULL;
}

/* prints tree using inorder traversal
*/
void AVLT_Traverse( TREE *pTree, void (*callback)(const void *)){
    _traverse(pTree->root, callback);
}

/* prints tree using right-to-left inorder traversal
*/
void AVLT_TraverseR( TREE *pTree, void (*callback)(const void *)){
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
int AVLT_Count( TREE *pTree){
    return pTree->count;
}

/* returns height of the tree
*/
int AVLT_Height( TREE *pTree){
    return getHeight(pTree->root);
}


