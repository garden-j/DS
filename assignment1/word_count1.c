#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free, qsort
#include <string.h> // strdup, strcmp

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

//	// 정렬 (빈도 내림차순, 빈도가 같은 경우 단어순)
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
    char str[20];
    dic->len = 0;

    while(fscanf(fp, "%s\n", str)!=EOF){
        // 공간 없으면 malloc
        if(dic->len>=dic->capacity){
            dic->capacity+=1000;
            // dic->data는 tWord* 형태로 cast.
            dic->data = (tWord*) realloc(
                    dic->data, dic->capacity * sizeof(tWord));
        }

        // 처음 단어 넣기
        if(dic->len == 0){
            dic->data[dic->len].word = (char *) malloc(20 * sizeof(char));
            strcpy(dic->data[dic->len].word, str);
            dic->data[dic->len].freq = 1;
            dic->len++;
            continue;
        }

        // 이전 단어와 같으면 빈도수만 증가
        if(strcmp(str, dic->data[dic->len - 1].word)==0){
            dic->data[dic->len - 1].freq++;
        }

        // 새로운 단어가 나오면 word 자리 할당해주고 새 단어 넣어주고 사전 len++
        else {
            dic->data[dic->len].word = (char *) malloc(20 * sizeof(char));
            strcpy(dic->data[dic->len].word, str);
            dic->data[dic->len].freq = 1;
            dic->len++;
        }
    }
}

void print_dic( tWordDic *dic){
    for(int i=0; i<dic->len; i++){
        printf("%s\t%d\n", dic->data[i].word, dic->data[i].freq);
    }
}


void destroy_dic(tWordDic *dic){
    // word는 각 data 돌아가면서 free
    for(int i=0; i<dic->len; i++){
        free(dic->data[i].word);
    }
    free(dic->data);
    free(dic);
}

int compare_by_freq( const void *n1, const void *n2){
    if((*(tWord*)n1).freq == (*(tWord*)n2).freq){
        return 0;
    }
    else if((*(tWord*)n1).freq < (*(tWord*)n2).freq){
        return 1;
    }
    else
        return -1;
}