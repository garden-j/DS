#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

static int trieList_main( TRIE *root, char *dic[], int count);

/* prints all entries starting with str (as prefix) in trie
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p; // # of permuterms
	int num_words = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	//permute_trie = trieCreateNode(); // trie for permuterm index
    for (int i = 0; i < 100; i++) {
        permuterms[i] = NULL;
    }

	while (fscanf( fp, "%s", str) != EOF)
	{	
		num_p = make_permuterms( str, permuterms);
		
		for (int i = 0; i < num_p; i++)
			trieInsert( permute_trie, permuterms[i], num_words);

		clear_permuterms( permuterms, num_p);
		
		dic[num_words++] = strdup( str);
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			//ret = trieSearch( permute_trie, str);
			
			//if (ret == -1) printf( "[%s] not found!\n", str);
			//else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < num_words; i++)
		free( dic[i]);
	
	//trieDestroy( permute_trie);
	
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[])
{
	if (root == NULL) return;

	trieList_main(root, dic, 0);
}

static int trieList_main( TRIE *root, char *dic[], int count){
    // condition to stop
    if(root == NULL) return count;


    for(int i = 0; i < MAX_DEGREE; i++){
        trieList_main(root->subtrees[i], dic, count);
        if(root->index != -1){
            printf("[%d] %s", count, dic[root->index]);
            count++;
        }
    }
    return count;
}

/* prints all entries starting with str (as prefix) in trie
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]){
    // '*' remove. only word.
    int i = 0;
    while(str[i] != '*') {
        root = root->subtrees[getIndex(str[i])];
        i++;
    }
    // print all entries using function
    trieList(root, dic);
}

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]){
    // add '$' to the query
    int i = 0;
    while(str+i != NULL){
        i++;
    }
    str[i] = '$';

    // move * to the last
    while(str[i] != '*'){
        char tmp = str[0];
        for(int k = 0; k < i; k++){
            str[k] = str[k+1];
        }
        str[i] = tmp;
    }
    triePrefixList(root, str, dic);
}

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
	return	node pointer
			NULL if overflow
index = -1;
all ptr = NULL; initialize
*/
TRIE *trieCreateNode(void){
    TRIE *new = malloc(sizeof(TRIE));
    if(new == NULL)
        return NULL;
    new->index = -1;
    for(int i = 0; i < MAX_DEGREE; i++)
        new->subtrees[i] = NULL;
    return new;
}

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index){
    // insert
    int i = 0;
    while(str[i] != 0){
        // upper check
        if(isupper(str[i]) != 0)
            str[i] = tolower(str[i]);

        // overlap check

        int num = getIndex(str[i]);
        // alloc to next char
        if(root->subtrees[num] == NULL){
            root->subtrees[num] = malloc(sizeof(TRIE));// check it! (when it must stop)
            root->subtrees[num]->index = -1;
        }
        // go to next node
        root = root->subtrees[num];

        i++;
    }

    if(root->index == -1){
        root->index = dic_index;
        return 1;
    }
    // remove overlap
    else{
        return 0;
    }
}

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);




/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]){
    int length = strlen(str);
    str[length] = '$';
    str[length+1] = '\0';

    for (int i = 0; i < length + 1; i++) {
        permuterms[i] = calloc(100, sizeof(char));

        char tmp = str[0];
        strcpy(permuterms[i],str);
        for (int j = 0; j < length; j++) {
            str[j] = str[j + 1];
        }
        str[length] = tmp;
    }

    return length;
}

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size){
    for(int i = 0; i < size; i++){
        free(permuterms[i]);
    }
}



