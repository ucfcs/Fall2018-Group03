#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BLOCK 64
#define ALPHA_LEN 256

typedef struct{
  char symbol;
  char numLess;
}Lex;


void BWT(char* infile, char* outfile);
void RBWT(char* infile, char* outfile);
int cmp(char* p1, char* p2, int len);
char* cyclicRotator(int lRotations, char* arr, int N);
void printArr(char **arr, int N);
int compare(const void *a, const void *b);
int findLess(char symbol, Lex* finds, int size);
void printLex(Lex* finds, int size);
int findUniq(int* matches, int size);
Lex* initLex(Lex* finds, int size, int* matches, int N, char* L);
void mtfHelper(int index, char* symbols);
int search(char c, char* symbols);
void MTF(char* infile, char* outfile);
void put_bit(unsigned char b);
void rice_code(unsigned char x, int k);
int rice_len(unsigned char x, int k);

FILE *fp;
unsigned char buff = 0;
int filled = 0;
size_t cmp_size = 0;


int main(int argv, char **argc)
{
	unsigned int fs, i, k, outsize, best_size = 0, best_k;
	unsigned char *buf;
	struct stat stat;

	if (argv != 3)
	{
		printf("usage: %s <infile> <outfile>\n", argc[0]);
		return 1;
	}


  /*Transformations*/
  BWT(argc[1], "bwt.dat");
  MTF("bwt.dat", "mtf.dat");

  fp = fopen("mtf.dat", "rb");


	fstat(fileno(fp), &stat);
	fs = (unsigned int)stat.st_size;


	buf = (unsigned char*) malloc(fs);

  fread(buf, fs, 1, fp);

	fclose(fp);

	/* find length */
	for (k=0; k<8; k++)
	{
		fflush(stdout);

		outsize = 0;
		for (i=0; i<fs; i++)
			outsize += rice_len(buf[i], k);


		if (!best_size || outsize < best_size)
		{
			best_size = outsize;
			best_k = k;
		}
	}

	/* output */
	fp = fopen(argc[2], "wb");


	put_bit((best_k >> 2) & 1);
	put_bit((best_k >> 1) & 1);
	put_bit((best_k     ) & 1);

	for (i=0; i<fs; i++)
		rice_code(buf[i], best_k);

	/* flush */
	i = 8 - filled;
	while (i--) put_bit(1);

	fclose(fp);
	free(buf);
	return 0;
}
void BWT(char* infile, char* outfile){
  short x,y, I=0;
  size_t size;
  FILE *input, *output;

  if((input = fopen(infile, "rb")) == NULL){
	perror("Error reading input file");
    exit(errno);
  }
  if((output = fopen(outfile, "wb")) == NULL){
    perror("Error writing output file");
    exit(errno);
  }

  char original[BLOCK];

  int once = 0;
  while((size = cmp_size = fread(original, sizeof(char), BLOCK, input)) != 0){

    char** strings = (char**)calloc(size, sizeof(char*));
    for(x=0; x<size; x++)
      strings[x] = (char*)calloc(size, sizeof(char));

    char temp[size];
    //Write read size to file
    fwrite(&size, sizeof(short), 1, output);

    //Copying read values to temp
    for(x=0; x<size; x++)
      temp[x] = original[x];

    for(x=0; x<size; x++){
      cyclicRotator(1, temp, size);
      for(y=0; y<size; y++)
        strings[x][y] = temp[y];
    }

    //printArr(strings, N);
    qsort(strings, size, sizeof(char *), compare);
    //printArr(strings, N);

    for(x=0; x<size; x++){
      if(memcmp(strings[x], original, size)==0){
        I = x;
        break;
      }
    }

    for(x=0; x<size; x++)
      fwrite(&strings[x][size-1], sizeof(char), 1, output);
    fwrite(&I, sizeof(short), 1, output);
	
	for(x=0; x<size; x++)
		free(strings[x]);
	free(strings);
  }
  fclose(output);
  fclose(input);
}

void RBWT(char* infile, char* outfile){
  short x,y,uniq=0,I;
  uniq=0;
  FILE *input, *output;

  if((input = fopen(infile, "rb")) == NULL){}
    ferror(input);
  if((output = fopen(outfile, "wb")) == NULL)
    ferror(output);

  Lex* finds;
  short size;

  while(!feof(input)){

    fread(&size, sizeof(short), 1, input);
    int* matches = (int*)calloc(size, sizeof(int));
    char L[size];
    char string[size];
    //printf("size: %d\n", size);

    fread(L, sizeof(char), size, input);
    fread(&I, sizeof(short), 1, input);

    // printf("Size: %d, I: %d\n L: ",size, I);
    // for(x=0; x<size; x++)
    //   printf("%c", L[x]);
    // printf("\n");

    for(x=size-1; x>=0; x--){
      for(y=0; y<x; y++){
        if(L[y]==L[x])
          matches[x]++;
      }
    }
    uniq = findUniq(matches, size);
    // printf("Unique %d\n", uniq);
    // printf("Printing matches..\n");
    // for(x=0; x<size; x++)
    //   printf("%d ", matches[x]);
    // printf("\n");

    finds = initLex(finds, uniq, matches, size, L);
    //printLex(finds, uniq);

    string[size-1] = L[I];
    //printf("string[%d] = L[%d] = %c\n", size-1, I, L[I]);
    int a,b,lex=1,v=I;
    for(x=size-2; x>=0;x--){
      //printf("Symbol: %c\n", L[v]);
      a = matches[v];
      b = findLess(L[v], finds, uniq);
    //  printf("a: %d b: %d\n", a,b);
      v=a+b;
      string[x] = L[v];
    }
    for(x=0; x<size; x++)
      fwrite(&string[x], sizeof(char), 1, output);

	free(matches);
  }
  fclose(output);
  fclose(input);
}

int cmp(char* p1, char* p2, int len){
  int x, success = 0;
  for(x=0; x< len; x++)
    if(p1[x] != p2[x])
      return -1;
  return success;
}

int findUniq(int* matches, int size){
  int x,uniq=0;
  for(x=0; x<size; x++)
      if(matches[x]==0)
        uniq++;
  return uniq;
}

Lex* initLex(Lex* finds, int size, int* matches, int N, char* L){
  finds = (Lex*)calloc(size, sizeof(Lex));
  int lex=0,y,x;
  for(y=0; y<N; y++){
    //printf("Match[%d]: %d\n", y, matches[y]);
    if(matches[y]==0){
      finds[lex].symbol = L[y];
      for(x=0; x<N; x++) {
        if(L[x] < finds[lex].symbol)
          finds[lex].numLess++;
      }
      //printf("symbol: %c numless: %d\n", finds[x].symbol, finds[x].numLess);
      lex++;
    }
  }
  return finds;
}

void printLex(Lex* finds, int size){
  int x;
  for(x=0; x<size; x++){
    printf("symbol: %c numless: %d\n", finds[x].symbol, finds[x].numLess);
  }
  printf("\n");
}

int findLess(char symbol, Lex* finds, int size){
  int x;
  for(x=0; x<size; x++)
    if(finds[x].symbol==symbol)
      return finds[x].numLess;
  return 0;
}

char* cyclicRotator(int lRotations, char* arr, int N){
  int x,y;
  char first;
  for(y=0; y<lRotations; y++){
    first = arr[0];
    for(x=0; x<N-1; x++){
      arr[x] = arr[x+1];
    }
    arr[N-1] = first;
  }

  return arr;
}

void printArr(char **arr, int N){
  int x,y;
  for(x=0; x<N; x++){
    for(y=0; y< N; y++)
      printf("%c", arr[x][y]);
    printf("\n");
  }
}

int compare(const void *a, const void *b){
  return (memcmp(*(char**)a, *(char**)b, cmp_size)); //compare cmp_size bytes from each block of memory
}

void MTF(char* infile, char* outfile){
  short x,y, I=0;
  size_t size;
  FILE *input, *output;
  char alpha[ALPHA_LEN];
  char symbols[BLOCK];
  short out[BLOCK];
  size_t len;

  //creates alphabet
  for(x=0; x<ALPHA_LEN; x++)
    alpha[x] = x;

  if((input = fopen(infile, "rb")) == NULL){
    perror("Error reading input file");
    exit(errno);
  }if((output = fopen(outfile, "wb")) == NULL){
    perror("Error writing output file");
    exit(errno);
  }

    while((len = fread(symbols, sizeof(char), BLOCK, input)) > 0){
      for(x=0; x<len; x++){
        out[x] = (short)search(symbols[x], alpha); //Index is an int, but out wants to have 2-byte numbers

        mtfHelper(out[x], alpha);
      }
      fwrite(out, sizeof(short), len, output);
    }


  fclose(output);
  fclose(input);
}

void mtfHelper(int index, char* symbols){
    char* record = (char*)malloc(sizeof(char) * ALPHA_LEN);
    memcpy(record, symbols, ALPHA_LEN);

    // Characters pushed one position right
    // in the list up until curr_index
    memcpy(symbols + 1, record, index);

    // Character at curr_index stored at 0th position
    symbols[0] = record[index];
	free(record);
}

int search(char c, char* symbols){
  int x;
   for (x = 0; x < ALPHA_LEN; x++) {
       if (symbols[x] == c) {
           return x;
       }
   }
   printf("If you are reading this, search is returning unitialized memory.\n");
}

void put_bit(unsigned char b){
	buff = buff | ((b & 1) << filled);
	if (filled == 7)
	{
		if (!fwrite(&buff, 1, 1, fp))
		{
			printf("\nerror writing to file!\n");
			exit(1);
		}

		buff = 0;
		filled = 0;
	}
	else
		filled++;
}

void rice_code(unsigned char x, int k){
	int m = 1 << k;
	int q = x / m;
	int i;

	for (i=0; i<q; i++) put_bit(1);
	put_bit(0);

	for (i=k-1; i>=0; i--) put_bit( (x >> i) & 1 );
}

int rice_len(unsigned char x, int k){
	int m = 1 << k;
	int q = x / m;
	return q + 1 + k;
}
