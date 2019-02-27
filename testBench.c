#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

int main(int argc, char **argv){
  //Metrics
  clock_t start, end;
  double elapsed;
  //Directory of files to use
  char *corpus = "FileCorpus/";
  DIR *dir;
  struct dirent *dent;

  dir = opendir(corpus);

  //Compile program to run
  system("gcc sample.c -o s");

  if(dir != NULL){
    //Loops through all file names in directory
    while((dent = readdir(dir)) != NULL){


      //Attempt to ignore . and .. items
      if(strcmp(".", dent->d_name) == 0 || strcmp("..", dent->d_name) == 0 )
        continue;

      //printf("%s\n", dent->d_name);
      char run[100] = ".\\s ";
      strcat(run, dent->d_name);
      printf("%s\n", run);
      start = clock();
      system(run);
      end = clock();

      elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
      printf("Ran in %lf ms\n\n", elapsed);
    }
  }

  else{
    printf("%s directory non-existent\n", corpus);
  }
}
