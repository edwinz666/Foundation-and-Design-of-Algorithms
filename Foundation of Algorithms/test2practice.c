#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int most_common(int A[], int n);
int num_distinct(int A[], int n);
int max_repeats(int A[], int n);

int
main(int argc, char *argv[]){
	int A[7]={3,4,2,2,6,4,4};
	int B[8]={3,4,8,3,3,7,6,4};
	printf("%d\n%d\n", most_common(A, 5), most_common(A,7));
	printf("%d\n%d\n", num_distinct(B, 6), num_distinct(B, 8));
	printf("%d\n%d\n", max_repeats(A, 5), max_repeats(A, 7));
	return 0;
}


int most_common(int A[], int n){
	int freq[n], maxval, maxelem, i, j;
	
	for (i=0; i<n; i++){
		freq[i] = 1;
		for (j=0; j<n && j!=i; j++){
			if (A[i] == A[j]){
				freq[i] += 1;
			}
		}
	}
	
	maxval = freq[0];
	maxelem = 0;
	for (i=1; i<n; i++){
		if (freq[i] > maxval){
			maxval = freq[i];
			maxelem = i;
		}
	}
	return A[maxelem];
}

int num_distinct(int A[], int n){
	int distinct[n], i, j, count=0;
	
	for (i=0; i<n; i++){
		distinct[i] = 1;
	}
	
	for (i=0; i<n; i++){
		if (distinct[i]){
			count += 1;
			for (j=i+1; j<n; j++){
				if (A[i] == A[j]){
				distinct[j] = 0;
				}
			}
		}
	}
	
	return count;
}



int max_repeats(int A[], int n){
	int freq[n], maxfreq, i, j;
	
	for (i=0; i<n; i++){
		freq[i] = 1;
		for (j=0; j<n && j!=i; j++){
			printf("i=%d j=%d\n", i ,j);
			
			if (A[i] == A[j]){
				freq[i] += 1;
			}
		}
	}
	
	maxfreq = freq[0];
	for (i=1; i<n; i++){
		if (freq[i] > maxfreq){
			maxfreq = freq[i];
	
		}
	}
	return maxfreq;
}