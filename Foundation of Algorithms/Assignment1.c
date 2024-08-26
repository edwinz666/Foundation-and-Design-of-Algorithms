/* Program to assist in the challenge of solving sudoku puzzles.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2021, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: [Name: Edwin Zhu    Student ID: 911277]
   Dated:     [10 September 2021]

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* these #defines provided as part of the initial skeleton */

#define NDIM 3		/* sudoku dimension, size of each inner square */
#define NDIG (NDIM*NDIM)
			/* total number of values in each row */
#define NGRP 3		/* number of sets each cell is a member of */
#define NSET (NGRP*NDIG)
			/* total number of sets in the sudoku */
#define NCLL (NDIG*NDIG)
			/* total number of cells in the sudoku */

#define ERROR	(-1)	/* error return value from some functions */

/* these global constant arrays provided as part of the initial skeleton,
   you may use them in your code but must not alter them in any way,
   regard them as being completely fixed. They describe the relationships
   between the cells in the sudoku and provide a basis for all of the
   sudoku processing loops */

/* there are 27 different different sets of elements that need to be
   checked against each other, this array converts set numbers to cells,
   that's why its called s2c */
int s2c[NSET][NDIM*NDIM] = {
	/* the first group of nine sets describe the sudoku's rows */
	{  0,  1,  2,  3,  4,  5,  6,  7,  8 },
	{  9, 10, 11, 12, 13, 14, 15, 16, 17 },
	{ 18, 19, 20, 21, 22, 23, 24, 25, 26 },
	{ 27, 28, 29, 30, 31, 32, 33, 34, 35 },
	{ 36, 37, 38, 39, 40, 41, 42, 43, 44 },
	{ 45, 46, 47, 48, 49, 50, 51, 52, 53 },
	{ 54, 55, 56, 57, 58, 59, 60, 61, 62 },
	{ 63, 64, 65, 66, 67, 68, 69, 70, 71 },
	{ 72, 73, 74, 75, 76, 77, 78, 79, 80 },
	/* the second group of nine sets describes the sudoku's columns */
	{  0,  9, 18, 27, 36, 45, 54, 63, 72 },
	{  1, 10, 19, 28, 37, 46, 55, 64, 73 },
	{  2, 11, 20, 29, 38, 47, 56, 65, 74 },
	{  3, 12, 21, 30, 39, 48, 57, 66, 75 },
	{  4, 13, 22, 31, 40, 49, 58, 67, 76 },
	{  5, 14, 23, 32, 41, 50, 59, 68, 77 },
	{  6, 15, 24, 33, 42, 51, 60, 69, 78 },
	{  7, 16, 25, 34, 43, 52, 61, 70, 79 },
	{  8, 17, 26, 35, 44, 53, 62, 71, 80 },
	/* the last group of nine sets describes the inner squares */
	{  0,  1,  2,  9, 10, 11, 18, 19, 20 },
	{  3,  4,  5, 12, 13, 14, 21, 22, 23 },
	{  6,  7,  8, 15, 16, 17, 24, 25, 26 },
	{ 27, 28, 29, 36, 37, 38, 45, 46, 47 },
	{ 30, 31, 32, 39, 40, 41, 48, 49, 50 },
	{ 33, 34, 35, 42, 43, 44, 51, 52, 53 },
	{ 54, 55, 56, 63, 64, 65, 72, 73, 74 },
	{ 57, 58, 59, 66, 67, 68, 75, 76, 77 },
	{ 60, 61, 62, 69, 70, 71, 78, 79, 80 },
};


/* there are 81 cells in a dimension-3 sudoku, and each cell is a
   member of three sets, this array gets filled by the function 
   fill_c2s(), based on the defined contents of the array s2c[][] */
int c2s[NCLL][NGRP];

void
fill_c2s() {
	int s=0, d=0, c;
	for ( ; s<NSET; s++) {
		/* record the first set number each cell is part of */
		for (c=0; c<NDIM*NDIM; c++) {
			c2s[s2c[s][c]][d] = s;
		}
		if ((s+1)%(NGRP*NDIM) == 0) {
			d++;
		}
	}
#if 0
	/* this code available here if you want to see the array
	   cs2[][] that gets created, just change that 0 two lines back
	   to a 1 and recompile */
	for (c=0; c<NCLL; c++) {
		printf("cell %2d: sets ", c);
		for (s=0; s<NGRP; s++) {
			printf("%3d", c2s[c][s]);
		}
		printf("\n");
	}
	printf("\n");
#endif
	return;
}

/* find the row number a cell is in, counting from 1
*/
int
rownum(int c) {
	return 1 + (c/(NDIM*NDIM));
}

/* find the column number a cell is in, counting from 1
*/
int
colnum(int c) {
	return 1 + (c%(NDIM*NDIM));
}

/* find the minor square number a cell is in, counting from 1
*/
int
sqrnum(int c) {
	return 1 + 3*(c/NSET) + (c/NDIM)%NDIM;
}

/* If you wish to add further #defines, put them below this comment,
   then prototypes for the functions that you add

   The only thing you should alter above this line is to complete the
   Authorship Declaration 
*/


/****************************************************************/


/****************************************************************/

typedef int cell_options[NDIG+1];

/* an array where each element corresponds to an array of options for a cell, 
   more info at "fill_cell_options" function */
typedef cell_options all_cell_options[NCLL];

void stage1_read_sudoku(int sudoku[]);
void print_sudoku(int num, int position);

int stage2_grid_check(int sudoku[]);
int check_duplicates(int sudoku[], int set);
void print_set_loc(int set);
void int_swap(int *p1, int *p2);
void sort_int_array(int A[], int n);

int stage3_strategy1(int sudoku[]);
void fill_cell_options(all_cell_options all_cells, int cell_num);


    
/* main program controls all the action
*/
int
main(int argc, char *argv[]) {
    int sudoku[NCLL], input, cell_num;
    
    printf("\n");
    
    /* fills cell-to-set array for use */
    fill_c2s();
    
    /* reads input into the sudoku array */
    for (cell_num=0; cell_num<NCLL; cell_num++){
        scanf("%d", &input);
        sudoku[cell_num] = input;
    }
    
    /* prints the input sudoku and reports unknown values */
    stage1_read_sudoku(sudoku);
    
    printf("\n");
    
    /* exit program if a violation in the input has been found 
       during stage 2 */
    if (!stage2_grid_check(sudoku)){
    	return 0;
    }
    
    /* recursion of strategy 1 until no more changes can be made
       the sudoku */
    while(stage3_strategy1(sudoku)){
        printf("\n");
    }
    
    /* re-print sudoku after all changes from strategy 1 has been made,
       and counts remaining unknown values left in the sudoku */
    stage1_read_sudoku(sudoku);
    
	/* all done, so pack up bat and ball and head home */
	return 0;
}

/****************************************************************/

/************************     Stage 1     *************************/
/* counts unknown values and prints the sudoku */
void
stage1_read_sudoku(int sudoku[]){
    int num, cell_num, unknown = 0;
    
    for (cell_num=0; cell_num<NCLL; cell_num++){
        num = sudoku[cell_num];
        unknown += (!num);
        
        print_sudoku(num, cell_num);
    }
    
    printf("%2d cells are unknown\n", unknown);
    
    /* mandatory "ta daa" if the sudoku is solved */
    if (!unknown){
    	printf("\nta daa!!!\n");
    }
    
    return;
}




/* prints sudoku in the required format */
void
print_sudoku(int num, int position){
    
    /* prints unknown quantities in the input sudoku as "." instead */
    if (!num){
        printf(".");
    }
    else{
        printf("%d", num);
    }

    /* prints patterns based on position of the number */ 
    if (position != NCLL - 1){
        if ((position + 1) % NDIM == 0){
            if ((position + 1) % NDIG == 0){
                printf("\n");
                if ((position + 1) % (NDIM*NDIG) == 0){
                    printf("------+-------+------\n");}
            }
            else{
                printf(" | ");
            }
        }
        else{
            printf(" ");
        }
    }

    /* if input is last cell in the sudoku, there is two new line 
       characters instead */
    else{
        printf("\n\n");
    }
    
    return;
}






/************************     Stage 2     ************************/

/* checks the input sudoku for violations, returning 0 if there is a violation
   found, or 1 otherwise */
int
stage2_grid_check(int sudoku[]){
	int i, violations, total_violations = 0, unique_violations = 0;
    
	/* checks every set for violations, and also accounts for multiple 
	   violations within a single set */ 
	for (i=0; i<NSET; i++){
		violations = check_duplicates(sudoku, i);
		total_violations += violations;
		unique_violations += (violations > 0);
	}
    
    /* prints information on unique sets that have violations, and also
       total number of violations across all sets */
	if (total_violations){
		printf("\n%d different sets have violations\n", unique_violations);
		printf("%d violations in total\n", total_violations);
		return 0;
	}
	
	return 1;
}





/* function that checks for duplicates across a given set in the sudoku */
int 
check_duplicates(int sudoku[], int set){
	int i, instances, test_cells[NDIG], violations = 0;
	
	/* copying the cells corresponding to the given set of the sudoku into an 
	   array */
	for (i=0; i<NDIG; i++){
		test_cells[i] = sudoku[s2c[set][i]];
	}
	
    /* sort cells into ascending order */
	sort_int_array(test_cells, NDIG);
	
	/* since it is an ascending order, we can remove 0's at the beginning
	   of the array */
	i = 0;
	while (!test_cells[i]){
			i += 1;
		}
	
	
	/* if the set isn't all 0's, we find a known value in this set and hence
	   there is now an instance of 1 of that known value */
	if 	(i<NDIG){
		instances = 1;
	}
	
	
	for (; i<NDIG-1; i++){
		
		/* another instance of the current known value is found */
		if (test_cells[i] == test_cells[i+1]){
			instances += 1;
		}
		
		/* values don't match, and since the array is in ascending order, we
		   have found all instances of the previous value. */
		else{
            /* there is a violation if there is more than 1 instance 
               of the previous value */
            if (instances > 1){
            
            	violations += 1;
               
			    print_set_loc(set);
			    printf("%d instances of %d\n", instances, test_cells[i]);
            }
            
            /* new value found, so instances of our current number is reset to 
               1 */
			instances = 1;
		}
	}
	
	if (instances > 1){
        violations += 1;
		print_set_loc(set);
		printf("%d instances of %d\n", instances, test_cells[i]);
	}
	
	return violations;
}




/* prints the set and its corresponding location in terms of row, column
   or square number */
void
print_set_loc(int set){
	int num;
	
	if (set < NSET/3){
		num = set + 1;
		printf("set %2d (row %d): ", set, num);
	}
	
	if (NSET/3 <= set && set < NSET*2/3){
		num = set - NSET/3 + 1;
		printf("set %2d (col %d): ", set, num);
	}
	
	if (NSET*2/3 <= set){
		num = set - NSET*2/3 + 1;
		printf("set %2d (sqr %d): ", set, num);
	}
	
	return;
}
	




/* swaps the values at two different memory addresses */
void
int_swap(int *p1, int *p2) {
	int tmp;
	tmp = *p1;
	*p1 = *p2;
	*p2 = tmp;
}
/* =====================================================================
   Program written by Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
   ================================================================== */

   
   

/* sorts an integer array into ascending order */
void
sort_int_array(int A[], int n) {
	int i, j;
	/* assume that A[0] to A[n-1] have valid values */
	for (i=1; i<n; i++) {
		/* swap A[i] left into correct position */
		for (j=i-1; j>=0 && A[j+1]<A[j]; j--) {
			/* not there yet */
			int_swap(&A[j], &A[j+1]);
		}
	}
	/* and that's all there is to it! */
}

/* =====================================================================
   Program written by Alistair Moffat, as an example for the book
   "Programming, Problem Solving, and Abstraction with C", Pearson
   Custom Books, Sydney, Australia, 2002; revised edition 2012,
   ISBN 9781486010974.

   See http://people.eng.unimelb.edu.au/ammoffat/ppsaa/ for further
   information.

   Prepared December 2012 for the Revised Edition.
   ================================================================== */

   
   






/************************     Stage 3     ************************/

/* implements strategy 1 to attempt solving the sudoku */
int
stage3_strategy1(int sudoku[]){
	int cell_num, set_num, set_cell;
	int unavail, changes=0, i;
    all_cell_options all_cells;
    
    /* identify valid options over every cell */
	for (cell_num = 0; cell_num < NCLL; cell_num++){
        
        /* initiates potential valid options of a cell */
        fill_cell_options(all_cells, cell_num);
        
        /* if cell in the sudoku is known, we continue to the next iteration */
		if (sudoku[cell_num]){
			continue;
		}
		
        /* iterate over every set that cell is part of, and over every cell
           that set is part of. */
        for (set_num = 0; set_num < NGRP; set_num++){
			for (set_cell = 0; set_cell < NDIG; set_cell++){
            
                /* The corresponding element in the sudoku is 
                   an unavailable option for that cell */
				unavail = sudoku[s2c[c2s[cell_num][set_num]][set_cell]];
                
                /* remove from valid options of the current unknown cell only 
                   if the identified cell is not an unknown */
                if (unavail){
                    if (all_cells[cell_num][unavail] == 1){
                        all_cells[cell_num][unavail] = 0;
					    all_cells[cell_num][0] -= 1;
                    }
                }
			}
		}
    }
    
    for (cell_num = 0; cell_num < NCLL; cell_num++){
        
        /* identify cells where there is only 1 remaining option */
        if (all_cells[cell_num][0] == 1){
        
            changes += 1;
            
            /* print message only when the first change has been found 
               for a given run of strategy 1 */
            if (changes == 1){
			    printf("strategy one\n");
            }
            
            /* find the single valid option for the cell and assign it */    
            for (i=1; i<=NDIG; i++){
                if (all_cells[cell_num][i] == 1){
                    printf("row %d col %d must be %d\n", rownum(cell_num),
						colnum(cell_num), i);
					sudoku[cell_num] = i;
                    break;
                }
            }
        }    
	}
    
	return changes;
}



/* initiates potential valid options of a cell */
void
fill_cell_options(all_cell_options all_cells, int cell_num){
    int i;
    
    /* all numbers from 1 to 9 (corresponding indexes 1 to 9) 
       are initially available */
    for (i=1; i<=NDIG; i++){
        all_cells[cell_num][i] = 1;
    }
    
    /* index 0 represents total unique options left for that cell */
    all_cells[cell_num][0] = 9;
    
    return;
}

/*************************     end of code     ************************/
/* algorithms are fun */
