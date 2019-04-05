#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct{
  char symbol;
  char numLess;
}Lex;

void BWT(int N, char* infile, char* outfile);
void RBWT(int N, char* infile, char* outfile);
char* cyclicRotator(int lRotations, char* arr, int N);
void printArr(char **arr, int N);
int compare(const void *a, const void *b);
int findLess(char symbol, Lex* finds, int size);
void printLex(Lex* finds, int size);
int findUniq(int* matches, int size);
Lex* initLex(Lex* finds, int size, int* matches, int N, char* L);

char* symbols;
char** strings;
FILE *input, *output;


int main(){
  BWT(15, "book1.dat", "bwtbook1.dat");
  RBWT(15, "bwtbook1.dat", "rbwtbook1.dat");
}

void BWT(int N, char* infile, char* outfile){
  int x,y, I=-1;
  input = fopen(infile, "r");
  output = fopen(outfile, "w");
  char* temp = (char*)calloc(N, sizeof(char));
  strings = (char**)calloc(N, sizeof(char*));
  symbols = (char*)calloc(N, sizeof(char));

  for(x=0; x<N; x++)
    strings[x] = (char*)calloc(N, sizeof(char));

while(!feof(input)){
    //Grabs N bytes from file
    for(x=0; x<N; x++){
      char c = fgetc(input);
      temp[x]=symbols[x] = c;
    }


    for(x=0; x<N; x++){
      cyclicRotator(1, temp, N);


      for(y=0; y<N; y++)
        strings[x][y] = temp[y];
    }
    //printArr(strings, N);
    qsort(strings, N, sizeof(char *), compare);
    //printArr(strings, N);

    for(x=0; x<N; x++){
      if(strcmp(strings[x], symbols)==0){
        I = x;
        break;
      }
    }


    for(x=0; x<N; x++)
      fputc(strings[x][N-1],output);
    fprintf(output, "%d", I);


  }
  free(temp);
  free(strings);
  free(symbols);
  fclose(output);
  fclose(input);
}

void RBWT(int N, char* infile, char* outfile){
  int x,y,uniq=0,I;
  uniq=0;
  input = fopen(infile, "r");
  output = fopen(outfile, "w");
  Lex* finds;
  int* matches;
  char* L, *string;

  while(!feof(input)){
    matches = (int*)calloc(N, sizeof(int));
    L = (char*)calloc(N, sizeof(char));
    string = (char*)calloc(N, sizeof(char));

    for(x=0; x<N; x++)
      L[x] = fgetc(input);
    fscanf(input, "%d", &I);

    for(x=0; x<N; x++)
      printf("%c",L[x]);
    printf(" %d\n", I);

    for(x=N-1; x>=0; x--){
      for(y=0; y<x; y++){
        if(L[y]==L[x])
          matches[x]++;
      }
    }
    uniq = findUniq(matches, N);
    printf("Unique %d\n", uniq);
    printf("Printing matches..\n");
    for(x=0; x<N; x++)
      printf("%d ", matches[x]);
    printf("\n");

    finds = initLex(finds, uniq, matches, N, L);
    printLex(finds, uniq);

    string[N-1] = L[I];
    printf("string[%d] = L[%d] = %c\n", N-1, I, L[I]);
    int a,b,lex=1,v=I;
    for(x=N-2; x>=0;x--){
      printf("Symbol: %c\n", L[v]);
      a = matches[v];
      b = findLess(L[v], finds, uniq);
      printf("a: %d b: %d\n", a,b);
      v=a+b;
      string[x] = L[v];
    }
    for(x=0; x<N; x++)
      fputc(string[x],output);

  free(matches);
  free(L);
  free(string);
  }
  fclose(output);
  fclose(input);
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
