#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BLOCK 64

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



int main(){
  BWT("xml.dat", "bwtxml.dat");
  RBWT("bwtxml.dat", "rbwtxml.dat");
}

void BWT(char* infile, char* outfile){
  short x,y, I=0;
  size_t size;
  FILE *input, *output;

  if((input = fopen(infile, "rb")) == NULL){}
    ferror(input);
  if((output = fopen(outfile, "wb")) == NULL)
    ferror(output);


  char original[BLOCK];

  int once = 0;
  while((size = fread(original, sizeof(char), BLOCK, input)) != 0){

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
      if(strcmp(strings[x], original)==0){
        I = x;
        break;
      }
    }

    for(x=0; x<size; x++)
      fwrite(&strings[x][size-1], sizeof(char), 1, output);
    fwrite(&I, sizeof(short), 1, output);
  }
  // free(temp);
  // free(strings);
  //free(original);
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
  free(L);
  free(string);
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
  const char **pa,**pb;
  pa = (const char **)a;
  pb = (const char **)b;

  return (strcmp(*pa, *pb));
}
