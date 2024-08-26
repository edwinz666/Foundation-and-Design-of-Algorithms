#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
main(int argc, char *argv[]){
	
	char str1[6] = "Hello";
	char str2[4];
	
	strncpy(str2, str1, sizeof(str2));
	printf("%s", str2);
	return 0;
	}