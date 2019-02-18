#include "stdlib.h"
#include "stdio.h"

int main(int argc, char **argv) {
	printf("File entered: %s\n", argv[1]);
	FILE* fp = fopen(argv[1], "r");

	fclose(fp);
	return 0;
}