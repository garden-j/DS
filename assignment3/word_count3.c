#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// User structure type definition
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tWord		*dataPtr;
	struct node	*link; // 단어순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 단어순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for update_dic function
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu);

// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr);

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
void update_dic( LIST *list, char *word);

// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc);

// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency( LIST *list);

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( LIST *pList); // 단어순
void print_dic_by_freq( LIST *pList); // 빈도순

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word);

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode);

////////////////////////////////////////////////////////////////////////////////
// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;

	return strcmp( p1->word, p2->word);
}
////////////////////////////////////////////////////////////////////////////////
// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;

	int ret = (int) p2->freq - p1->freq;

	if (ret != 0) return ret;

	return strcmp( p1->word, p2->word);
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	char word[1000];

	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_WORD;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	// creates an empty list
	list = createList();

	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	if ((fp = fopen( argv[2], "r")) == NULL)
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	while(fscanf( fp, "%s", word) != EOF)
	{
		// 사전(단어순 리스트) 업데이트
		update_dic( list, word);
	}

	fclose( fp);

	if (option == SORT_BY_WORD) {

		// 단어순 리스트를 화면에 출력
		print_dic( list);
	}
	else { // SORT_BY_FREQ

		// 빈도순 리스트 연결

		connect_by_frequency( list);

		// 빈도순 리스트를 화면에 출력
		print_dic_by_freq( list);
	}

	// 단어 리스트 메모리 해제
	destroyList( list);

	return 0;
}

LIST *createList(void){
    LIST *new = malloc(sizeof(LIST));
    new->count = 0;
    new->head = NULL;
    new->head2 = NULL;
    return new;
}

//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList){
    //list free
    // each node free using count
    NODE *p = pList->head;

    while(p != NULL){
        p->dataPtr->freq = 0;
        destroyWord(p->dataPtr);
        p->dataPtr = NULL;
        p = p->link;
    }
    while(pList->head != NULL){
        p = pList->head;
        pList->head = p->link;
        free(p);
    }
    p = NULL;
    pList->count = 0;
    free(pList);
}

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for update_dic function
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu){
    *pPre = NULL;
    *pLoc = pList->head;

    if(pList->head == NULL){
        return 0;
    }

    while(*pLoc != NULL && compare_by_word(pArgu, (*pLoc)->dataPtr)>0){
        *pPre = *pLoc;
        *pLoc = (*pLoc)->link;
    }

    if(*pLoc == NULL){
        return 0;
    }
    else{
        if(strcmp(pArgu->word, (*pLoc)->dataPtr->word) == 0)
            return 1;
        else
            return 0;
    }
}

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu) {
    *pPre = NULL;
    *pLoc = pList->head2;

    if (pList->head2 == NULL) {
        return 0;
    }

    while (*pLoc != NULL && compare_by_freq(pArgu, (*pLoc)->dataPtr) > 0) {
        *pPre = *pLoc;
        *pLoc = (*pLoc)->link2;
    }
    return 1;
}

// internal insert function
// inserts data into a new node
// for update_dic function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr){

    //할당
    NODE *pNew = malloc(sizeof(NODE));
    //할당 됐는지 확인
    if(pNew == NULL)
        return 0;
    //새 데이터 넣어주고
    pNew->dataPtr = dataInPtr;
    pNew->link = NULL;
    pNew->link2 = NULL;
    //처음에 들어가는 경우
    if(pPre == NULL){
        //새 노드를 기존 첫번째 노드를 가리키게
        pNew->link = pList->head;
        //리스트 헤드가 새 노드를 가리키게
        pList->head = pNew; // 누수 발생했던 원인. pNew->link가 아니라 pNew.
    }
    //중간이나 끝에 들어가는 경우
    else{
        pNew->link = pPre->link;
        pPre->link = pNew;
    }
    return 1;
}

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
void update_dic( LIST *list, char *word){
    tWord *newWord = createWord(word);
    NODE *pPre, *pLoc;

    int searched = _search(list, &pPre, &pLoc, newWord);

    if(searched == 1){
        pLoc->dataPtr->freq++;
        free(newWord->word);
        free(newWord);
    }
    else{
        if(pPre == NULL && pLoc == NULL) {
            NODE *pNew = malloc(sizeof(NODE));
            pNew->link = NULL;
            pNew->link2 = NULL;
            pNew->dataPtr = newWord;
            list->head = pNew;
        }
        else{
            _insert(list, pPre, newWord);
        }
        list->count++;
    }
}



// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency( LIST *list){
    NODE *p = list->head;
    NODE *pPre, *pLoc;
    while(p != NULL){
        int search_freq = _search_by_freq(list, &pPre, &pLoc, p->dataPtr);

        //insert into empty list
        if(search_freq == 0){
            list->head2 = p;
            p->link2 = NULL;
        }
        //insert into first node.
        else{
            if(pPre == NULL){
                p->link2 = list->head2;
                list->head2 = p;
            }
                //insert into the middle of the node
            else{
                p->link2 = pPre->link2;
                pPre->link2 = p;
            }
        }
        p = p->link;
    }
}

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( LIST *pList) {// 단어순
    NODE *p = pList->head;
    while(p != NULL) {
        printf("%s\t%d\n",p->dataPtr->word, p->dataPtr->freq);
        p = p->link;
    }
}

void print_dic_by_freq( LIST *pList) {// 빈도순
    NODE *p = pList->head2;
    while(p != NULL) {
        printf("%s\t%d\n",p->dataPtr->word, p->dataPtr->freq);
        p = p->link2;
    }
}

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for update_dic function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord(char *word){
    tWord *newWord = malloc(sizeof(tWord));
    newWord->word = malloc(sizeof(char)*20);
    strcpy(newWord->word, word);
    newWord->freq = 1;

    return newWord;
}

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode){
    //tWord free
    //word free
    free(pNode->word);
    free(pNode);
}