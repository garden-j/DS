#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free, qsort
#include <string.h> // strdup, strcmp, memmove

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 구조체 선언
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

// 사전(dictionary) 구조체
typedef struct {
	int		len;		// 배열에 저장된 단어의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 단어의 수)
	tWord	*data;		// 단어 구조체 배열에 대한 포인터
} tWordDic;

////////////////////////////////////////////////////////////////////////////////
// 함수 원형 선언(declaration)

// 단어를 사전에 저장
// 새로 등장한 단어는 사전에 추가
// 이미 사전에 존재하는(저장된) 단어는 해당 단어의 빈도를 갱신(update)
// capacity는 1000으로부터 시작하여 1000씩 증가 (1000, 2000, 3000, ...)
void word_count( FILE *fp, tWordDic *dic);

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( tWordDic *dic);

// 사전에 할당된 메모리를 해제
void destroy_dic(tWordDic *dic);

// qsort를 위한 비교 함수
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2);

// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2);

////////////////////////////////////////////////////////////////////////////////
// 이진탐색 함수
// found : key가 발견되는 경우 1, key가 발견되지 않는 경우 0
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
				int (*compare)(const void *, const void *), int *found);
				
////////////////////////////////////////////////////////////////////////////////
// 함수 정의 (definition)

// 사전을 초기화 (빈 사전을 생성, 메모리 할당)
// len를 0으로, capacity를 1000으로 초기화
// return : 구조체 포인터
tWordDic *create_dic(void)
{
	tWordDic *dic = (tWordDic *)malloc( sizeof(tWordDic));
	
	dic->len = 0;
	dic->capacity = 1000;
	dic->data = (tWord *)malloc(dic->capacity * sizeof(tWord));

	return dic;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tWordDic *dic;
	int option;
	FILE *fp;
	
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
	
	// 사전 초기화
	dic = create_dic();

	// 입력 파일 열기
	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	// 입력 파일로부터 단어와 빈도를 사전에 저장
	word_count( fp, dic);

	fclose( fp);

	// 정렬 (빈도 내림차순, 빈도가 같은 경우 단어순)
	if (option == SORT_BY_FREQ) {
		qsort( dic->data, dic->len, sizeof(tWord), compare_by_freq);
	}
		
	// 사전을 화면에 출력
	print_dic( dic);


	// 사전 메모리 해제
	destroy_dic( dic);
	
	return 0;
}


void word_count( FILE *fp, tWordDic *dic){
    char str[20] = {0};
    dic->len = 0;

    while(fscanf(fp, "%s\n", str)!=EOF){
        if(dic->len >= dic->capacity){
            dic->capacity+=1000;
            dic->data = (tWord*) realloc(
                    dic->data, dic->capacity * sizeof(tWord));
        }
        // first insert
        if(dic->len == 0) {
            dic->data[dic->len].word = (char *) malloc(20 * sizeof(char));
            strcpy(dic->data[dic->len].word, str);
            dic->data[0].freq++;
            dic->len++;
        }
        else{
            int fnd;
            int index = binary_search( str, dic->data, dic->len, sizeof(tWord),
                           compare_by_word, &fnd);
            if(fnd == 1){
                dic->data[index].freq++;
            }
            else{
                memmove(&dic->data[index + 1], &dic->data[index], sizeof(tWord)*((dic->len) - index));
                dic->data[index].word = (char *) malloc(20 * sizeof(char));
                dic->data[index].freq = 0;
                strcpy(dic->data[index].word, str);
                dic->data[index].freq++;
                dic->len++;
            }
        }
    }
}

//input index finder
int binary_search( const void *key, const void *base, size_t nmemb, size_t size,
                   int (*compare)(const void *, const void *), int *found){

    int l = 0;
    int r = nmemb-1;
    int cmp;
    int m;

    const tWord *data = (const tWord *) base;

    while(l < r){
        m = (l+r)/2;
        cmp = compare(key, data[m].word);

        if(cmp < 0){
            r = m-1;
        }
        else if(cmp > 0){
            l = m+1;
        }
        else{
            *found = 1;
            return m;
        }
    }
    if(l >= r){
        m = (l+r)/2;
        cmp = compare(key, data[m].word);
        if(cmp == 0){
            *found = 1;
            return m;
        }
        else if(cmp > 0){
            *found = 0;
            return m+1;
        }
        else{
            *found = 0;
            return m;
        }
    }
}

int compare_by_word( const void *n1, const void *n2){
     return strcmp((char*)n1,(char*)n2);
}

void print_dic( tWordDic *dic){
    for(int i = 0; i<dic->len; i++){
        printf("%s\t%d\n", dic->data[i].word, dic->data[i].freq);
    }
}

int compare_by_freq( const void *n1, const void *n2){
    if((*(tWord*)n1).freq == (*(tWord*)n2).freq){
        return 0;
    }
    else if((*(tWord*)n1).freq < (*(tWord*)n2).freq){
        return -1;
    }
    else
        return 1;
}

void destroy_dic(tWordDic *dic){
    for(int i=0; i<dic->len; i++){
        free(dic->data[i].word);
    }
    free(dic->data);
    free(dic);
}

