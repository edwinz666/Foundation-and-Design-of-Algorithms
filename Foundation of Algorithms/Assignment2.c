/* Program to print and play checker games.

  Skeleton program written by Artem Polyvyanyy, artem.polyvyanyy@unimelb.edu.au,
  September 2021, with the intention that it be modified by students
  to add functionality, as required by the assignment specification.

  Student Authorship Declaration:

  (1) I certify that except for the code provided in the initial skeleton file,
  the program contained in this submission is completely my own individual
  work, except where explicitly noted by further comments that provide details
  otherwise. I understand that work that has been developed by another student,
  or by me in collaboration with other students, or by non-students as a result
  of request, solicitation, or payment, may not be submitted for assessment in
  this subject. I understand that submitting for assessment work developed by
  or in collaboration with other students or non-students constitutes Academic
  Misconduct, and may be penalized by mark deductions, or by other penalties
  determined via the University of Melbourne Academic Honesty Policy, as
  described at https://academicintegrity.unimelb.edu.au.

  (2) I also certify that I have not provided a copy of this work in either
  softcopy or hardcopy or any other form to any other student, and nor will I
  do so until after the marks are released. I understand that providing my work
  to other students, regardless of my intention or any undertakings made to me
  by that other student, is also Academic Misconduct.

  (3) I further understand that providing a copy of the assignment specification
  to any form of code authoring or assignment tutoring service, or drawing the
  attention of others to such services and code that may have been made
  available via such a service, may be regarded as Student General Misconduct
  (interfering with the teaching activities of the University and/or inciting
  others to commit Academic Misconduct). I understand that an allegation of
  Student General Misconduct may arise regardless of whether or not I personally
  make use of such solutions or sought benefit from such actions.

  Signed by: [Name: Edwin Zhu]			[Student ID: 911277]
  Dated:     [10 October 2021]

*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions
#define SEPARATOR 			 "====================================="
#define INITIAL             8       // initial number of nodes out of each node

/* one type definition from my sample solution -------------------------------*/
typedef unsigned char board_t[BOARD_SIZE][BOARD_SIZE];  // board type

typedef struct node node_t;

struct node {
    int depth;            /* depth of node */
    int move_num;         /* move number to be played at the node */       
	int *rational_cost;   /* assigned cost value if rational plays */
    int nboards;          /* number of boards(nodes) attached to the node */  
    char **board;         /* position of board at node */   
    char *prev_move;      /* move played to get to the board position */  
	node_t **node;        /* the nodes attached to this node */    
    int (*cmp)(int, int); /* function pointer */
};

typedef struct {
	node_t *root;         /* root node of the tree */
} tree_t;

/* Stage 0 - Reading, Analyzing and Printing Input Data */
char **create_print_new_board(void);
void print_board(char **board);
int check_move(char *move, int move_num, char **board);
void print_error(int error_type);
void play_move(int error_type, char *move, char **board);
int board_cost(char **board);

/* Stage 1 - Compute and Print Next Action */

/* functions for reaching all board configurations from the current board */
tree_t *make_empty_tree(int func(int, int), char **board, int move_no);
char **duplicate_board(char **board);
void recursive_make_branches(node_t *root);

/* functions for assigning costs to nodes */
void recursive_assign_costs(node_t *root);
int min(int num1, int num2);
int max(int num1, int num2);

/* Stage 1 and 2 -> response to 'A' and 'P' commands */
tree_t *next_action(tree_t *tree);

/* functions that free up allocated memory and setting pointers to NULL */
void free_board(char **board);
void free_tree(tree_t *tree);
void recursive_free_root(node_t* root);


/* -------------- main function ---------------- */
int
main(int argc, char *argv[]) {
    char **board;
    int move_num=0, error_type, actions_left;
    char move[5];
    int (*func)(int, int);
    tree_t *tree;
    
    /* pointer to a new board with initial setup */
    board = create_print_new_board();
    
    while (scanf("%s", move) == 1){
    
        /* actions specified are of length 5 */
        if (strlen(move) != 5){
            break;
        }
        
        /* increment move number for each action found */
        move_num += 1;
        
        error_type = check_move(move, move_num, board);
        
        /* prints error message based on type of error found, and terminates
           program immediately */
        if (error_type > 0){
            print_error(error_type);
            return 0;
        }
        
        /* if valid move found, play the move to the board 
           and print information about the action, the board, and board cost */
        play_move(error_type, move, board);
                
        printf("%s\n", SEPARATOR);

        if (move_num % 2){
            printf("BLACK ");
        }
        else{
            printf("WHITE ");
        }
        
        printf("ACTION #%d: %s\n", move_num, move);
        printf("BOARD COST: %d\n", board_cost(board));
        print_board(board);

    }
    
    /* function at tree root based on white's or black's move */
    move_num += 1;
    if (move_num % 2){
        func = max;
    }
    else{
        func = min;
    }
    
    /* makes tree based on current state of the game and frees memory
       allocated to the old board used */
    tree = make_empty_tree(func, board, move_num);
    free_board(board);
    
    /* extend tree until desired tree depth is reached */
    recursive_make_branches(tree->root);
    
    /* assign costs to all nodes, starting from the deepest levels, until
       the tree's cost is assigned */
    while((tree->root->rational_cost) == NULL){
        recursive_assign_costs(tree->root);
    }
 
    /* response to commands 'A' and 'P' */        
    if (strlen(move) == 1 ){
        /* one move is played if command is 'A' */
        if (move[0] == 'A'){
            tree = next_action(tree);
        }
       
        if (move[0] == 'P'){
            actions_left = COMP_ACTIONS;
            /* moves are played only if either player is 
               able to move, up to a maximum of 10 */
            while (actions_left && ((tree = next_action(tree)) != NULL)){
                /* makes branches and assigns costs to each node, for every
                   new tree that results from an action */
                recursive_make_branches(tree->root);
    
                while ((tree->root->rational_cost) == NULL){
                recursive_assign_costs(tree->root);
                }
                
                actions_left -= 1;
            }
        }       
    }
    
    /* frees up memory allocated to the final tree and its components */
    if (tree != NULL){
    	free_tree(tree);
    }
    
    return EXIT_SUCCESS;            // exit program with the success code
}

/* creates a new board with the initial setup and returns a pointer to it */
char
**create_print_new_board(void){
	char **new_board;
	int row, col;
	
	new_board = (char**)malloc(sizeof(*new_board)*BOARD_SIZE);
	assert(new_board != NULL);
	
    
	for (row=0; row<BOARD_SIZE; row++){
        new_board[row] = (char*)malloc(BOARD_SIZE*sizeof(char));
        assert(new_board[row] != NULL);
        
        /* initialise all cells to empty */
		for (col=0; col<BOARD_SIZE; col++){
			new_board[row][col] = CELL_EMPTY;
		}
	}
	
	for (row=0; row<ROWS_WITH_PIECES; row++){
		for (col=1; col<BOARD_SIZE; col+=2){
            /* by symmetry, can assign both colour pieces to their squares */
			new_board[row][col-row%2] = CELL_WPIECE;
			new_board[BOARD_SIZE-row-1][BOARD_SIZE-col-1+row%2] = CELL_BPIECE;
		}
	}
	
	printf("BOARD SIZE: 8x8\n#BLACK PIECES: 12\n#WHITE PIECES: 12\n");
    
	print_board(new_board);
	return new_board;
}

/* prints the board in required format */
void
print_board(char **board){
	int row, col;
	
	printf("     A   B   C   D   E   F   G   H\n");
    printf("   +---+---+---+---+---+---+---+---+\n");
    
    for (row=0; row<BOARD_SIZE; row++){
    	printf(" %d |", row+1);
    	
    	for (col=0; col<BOARD_SIZE; col++){
    		printf(" %c |", board[row][col]);
    	}
    	
    	printf("\n   +---+---+---+---+---+---+---+---+\n");
    }
    
    return;		
}

/* returns a positive value based on type of error if move is invalid,
   or negative value based on distance moved if move is valid */
int
check_move(char *move, int move_num, char **board){
    int source_col, source_row, dest_col, dest_row, col_diff, row_diff;
    char source_cell, dest_cell, jump_over_cell, turn;
    
    /* decomposing input move into rows/columns of source/destination cells */
    source_col = (int)(move[0]) - (int)('A');
    source_row = (int)(move[1]) - (int)('1');
    dest_col   = (int)(move[3]) - (int)('A');
    dest_row   = (int)(move[4]) - (int)('1');
  
    /* Source cell is outside of the board */
    if ((source_col < 0 || source_col >= BOARD_SIZE) || 
        (source_row < 0 || source_row >= BOARD_SIZE)){
        return 1;
    }
    
    /* Target cell is outside of the board */
    if ((dest_col < 0 || dest_col >= BOARD_SIZE) || 
        (dest_row < 0 || dest_row >= BOARD_SIZE)){
        return 2;
    }
    
    /* Source cell and Target cell exist on the board */
    source_cell = board[source_row][source_col];
    dest_cell = board[dest_row][dest_col];
    
    /* Source cell is empty */
    if (source_cell == CELL_EMPTY){
        return 3;
    }
    
    /* Target cell is empty */
    if (dest_cell != CELL_EMPTY){
        return 4;
    }
    
    /* Turn can be determined from move number */
    if (move_num % 2){
        turn = 'b';
    }
    else{
        turn = 'w';
    }
    
    /* Source cell holds opponent's piece/tower */
    if ((turn == 'b' && (source_cell == CELL_WPIECE || 
                        source_cell == CELL_WTOWER))||
        (turn == 'w' && (source_cell == CELL_BPIECE || 
                        source_cell == CELL_BTOWER))){
        return 5;
    }
    
    col_diff = dest_col - source_col;
    row_diff = dest_row - source_row;
    
    /* piece moves 1 square diagonal */
    if (abs(col_diff) == 1 && abs(row_diff) == 1){
        /* towers can move in any direction north/south */
        if (source_cell == CELL_BTOWER || source_cell == CELL_WTOWER){
            return -1;
        }
        
        /* pieces can only move in a specific direction north/south */
        if ((row_diff == 1 && source_cell == CELL_WPIECE) || 
            (row_diff == -1 && source_cell == CELL_BPIECE)){
            return -1;
        }
    }
    
    /* piece moves 2 squares diagonal */
    if (abs(col_diff) == 2 && abs(row_diff) == 2){
        /* what is on the board in the cell that is 'jumped over' */
        jump_over_cell = board[(source_row + dest_row)/2]
                              [(source_col + dest_col)/2];
        
        /* can only jump over opponent's piece */
        if ((turn == 'b' && (jump_over_cell == CELL_WPIECE || 
                            jump_over_cell == CELL_WTOWER))||
            (turn == 'w' && (jump_over_cell == CELL_BPIECE || 
                            jump_over_cell == CELL_BTOWER))){
            /* pieces can only move in one direction north/south */
            if ((source_cell == CELL_BPIECE && row_diff == -2) ||
                (source_cell == CELL_WPIECE && row_diff == 2)){
                return -2;
            }
            
            /* towers can move in any direction */
            if (source_cell == CELL_BTOWER || source_cell == CELL_WTOWER){
                return -2;
            }
        }
    }
    
    /* valid play cannot be found, so it is an illegal action */
    return 6;
}


/* prints error message based on the type of error */
void
print_error(int error_type){
    if (error_type == 1){
        printf("ERROR: Source cell is outside of the board.\n");
    }
    
    if (error_type == 2){
        printf("ERROR: Target cell is outside of the board.\n");
    }
    
    if (error_type == 3){
        printf("ERROR: Source cell is empty.\n");
    }
    
    if (error_type == 4){
        printf("ERROR: Target cell is not empty.\n");
    }
    
    if (error_type == 5){
        printf("ERROR: Source cell holds opponent's piece/tower.\n");
    }
    
    if (error_type == 6){
        printf("ERROR: Illegal action.\n");
    }
    
    return;
}

/* plays the valid move specified to the board */
void
play_move(int error_type, char *move, char **board){
    int source_col, source_row, dest_col, dest_row;
    char source_cell;
    
    source_col = (int)(move[0]) - (int)('A');
    source_row = (int)(move[1]) - (int)('1');
    dest_col   = (int)(move[3]) - (int)('A');
    dest_row   = (int)(move[4]) - (int)('1');
    
    source_cell = board[source_row][source_col];
    
    /* white piece reaching the final row becomes a white tower */
    if (source_cell == CELL_WPIECE && dest_row == 7){
        board[dest_row][dest_col] = CELL_WTOWER;
    }
    /* black piece reaching the first row becomes a black tower */
    else{if (source_cell == CELL_BPIECE && dest_row == 0){
        board[dest_row][dest_col] = CELL_BTOWER;
    }
    /* piece moves to its target cell without changing states */
    else{
        board[dest_row][dest_col] = source_cell;
    }
    }
    
    /* piece leaves behind an empty cell */
    board[source_row][source_col] = CELL_EMPTY;
    
    /* if piece moves 2 squares diagonal, it also captures the opponent's 
       piece on the cell jumped over */
    if (error_type == -2){
    board[(source_row + dest_row)/2][(source_col + dest_col)/2] = CELL_EMPTY;
    }
    
    return;
}

/* computes the cost of the board */
int
board_cost(char **board){
    int row, col, cost=0;
    char cell;
    
    for (row=0; row<BOARD_SIZE; row++){
        for (col=0; col<BOARD_SIZE; col++){
            cell = board[row][col];
            cost += COST_PIECE * (cell == CELL_BPIECE);
            cost += COST_TOWER * (cell == CELL_BTOWER);
            cost -= COST_PIECE * (cell == CELL_WPIECE);
            cost -= COST_TOWER * (cell == CELL_WTOWER);
        }
    }
    
    return cost;
}


/* makes an empty tree and stores the current position of the game inside
   its root, and initializes other values */
tree_t
*make_empty_tree(int func(int, int), char **board, int move_no) {
	tree_t *tree;
    char **board_copy;
    
	tree = malloc(sizeof(*tree));
    board_copy = duplicate_board(board);
    assert(tree!=NULL && board_copy!=NULL);
    
    tree->root = malloc(sizeof(*(tree->root)));
    assert(tree->root != NULL);
    
    /* saved supplied function pointer, move number, and board position */
    tree->root->cmp = func; 
    tree->root->move_num = move_no;
    tree->root->board = board_copy;
    
    /* tree begins with no depth or nodes attached */
	tree->root->depth = 0;
    tree->root->nboards = 0;
    
    /* previous move does not need to be stored at the start of the tree,
       and cost cannot yet be assigned without the desired depth */
    tree->root->prev_move = NULL;
    tree->root->rational_cost = NULL;
    
    /* no children nodes upon creation */
    tree->root->node = NULL;

	return tree;
}

/* creates a copy of the board supplied and returns a pointer to it */
char **
duplicate_board(char **board){
    char **board_copy;
    int row, col;
    
    board_copy = (char**)malloc(sizeof(*board_copy)*BOARD_SIZE);
    assert(board_copy != NULL);
	
	for (row=0; row<BOARD_SIZE; row++){
        board_copy[row] = (char*)malloc(BOARD_SIZE*sizeof(char));
        assert(board_copy[row] != NULL);
        
		for (col=0; col<BOARD_SIZE; col++){
			board_copy[row][col] = board[row][col];
		}
	}
    
    return board_copy;
}

/* creates child nodes for a tree until desired tree depth is reached */
void
recursive_make_branches(node_t *root){
    int row, col, error_type, current_size=INITIAL, i;
    int find_piece_moves[8][2] = {{-1, 1}, {-2, 2}, {1, 1}, {2, 2}, 
    							   {1, -1}, {2, -2}, {-1, -1}, {-2, -2}};
    char test_move[6];
    char **new_board;
    node_t *child;
    
    /* stop making branches at desired tree depth, and assign the board cost
       of that node's board to its cost */
    if (root->depth == TREE_DEPTH){
        (root->rational_cost) = (int*)malloc(sizeof(int));
        assert((root->rational_cost) != NULL);
        *(root->rational_cost) = board_cost(root->board);
        return;
    }
    
    /* iterate over every cell of the current node's board, 
       in row-major order */   
    for (row=0; row<BOARD_SIZE; row++){
        for (col=0; col<BOARD_SIZE; col++){
            /* if black's turn and cell contains black's piece */
            if (((root->cmp == max) && (((root->board)[row][col] == CELL_BPIECE) 
                                   || ((root->board)[row][col] == CELL_BTOWER)))
            /* or white's turn and cell contains white's piece */ 
             || ((root->cmp == min) && (((root->board)[row][col] == CELL_WPIECE) 
                                  || ((root->board)[row][col] == CELL_WTOWER))))
            {       
                /* filling in the source cell of the candidate move */
                test_move[0] = 'A' + col;
                test_move[1] = '1' + row;
                test_move[2] = '-';
                test_move[5] = '\0';

                /* finding moves the piece can make, starting from the
                   north-east direction and going clockwise */
                for (i=0; i<8; i++){

                    /* filling in destination cell of the candidate move */
                    test_move[3] = 'A' + col + find_piece_moves[i][1];
                    test_move[4] = '1' + row + find_piece_moves[i][0];

                    /* check the candidate move, and add as a child
                       node if move is valid */ 
                    error_type = check_move(test_move, root->move_num, 
                                            root->board);
                    if (error_type < 0){
                        /* move is valid ... */
                        
                        /* parent node has now an extra child */
                        root->nboards += 1;
                        
                        /* allocate initial memory for child nodes  
                           for first child found */
                        if (root->nboards == 1){
                            root->node = malloc(sizeof(*(root->node))*INITIAL);
                            assert(root->node != NULL);
                        }
                        
                        /* if number of child nodes exceed current 
                           memory allocated, reallocate memory */
                        if (root->nboards == current_size){
                            current_size *= 2;
                            root->node = realloc(root->node, 
                                         current_size*sizeof(*root->node));
                            assert(root->node != NULL);
                        }

                        /* allocate pointer and memory to 
                           current child node's contents */
                        (root->node)[(root->nboards)-1] = 
                            malloc(sizeof(*(child))); 
                        assert((root->node)[(root->nboards)-1] != NULL);    
                        child = (root->node)[(root->nboards)-1];
                        

                        /* if move at parent's board was black's, then the move 
                           at child's board is white's, and vice versa */
                        if (root->cmp == max){
                            child->cmp = min;
                        }
                        else{
                            child->cmp = max;
                        }
                        
                        /* increment depth and move number for the child */
                        child->depth = (root->depth) + 1;
                        child->move_num = (root->move_num)+1;
                        
                        /* record the move that leads to the child board 
                           within the child node */
                        child->prev_move = (char*)malloc(sizeof(char)*5+1);
                        assert(child->prev_move != NULL);
                        strcpy(child->prev_move, test_move);
                        
                        /* create a copy of the parent's board and assign
                           a pointer (within the child node) to the new board 
                           with the candidate move played */
                        new_board = duplicate_board(root->board);
                        play_move(error_type, test_move, new_board);
                        child->board = new_board;
                        
                        /* initialise values for cost and number of nodes
                           of current child node */
                        child->rational_cost = NULL;
                        child->nboards = 0;

                        /* recursively make branches out of each child node
                           until tree depth is reached */
                        recursive_make_branches(child);
                    }
                }
            }
        }
    }

    return;
}


void
recursive_assign_costs(node_t *root){
    int value, i, null_count;
    
    
    /* final depth contains cost */
    if (root->depth == TREE_DEPTH){
        return; 
    }
    
    /* no moves available, so set cost to INT_MAX or INT_MIN */
    else{
        if ((root->nboards) == 0){
            (root->rational_cost) = (int*)malloc(sizeof(int));
            assert((root->rational_cost) != NULL);
            
            /* black cannot play a move, set cost to INT_MIN */
            if ((root->cmp) == max){
                *(root->rational_cost) = INT_MIN;
                return;
            }
            /* white cannot play a move, set cost to INT_MAX */
            if ((root->cmp) == min){
                *(root->rational_cost) = INT_MAX;
                return;
            }
        }
    }
    
    null_count = 0;
    for (i=0; i<(root->nboards); i++){
        if (((root->node)[i]->rational_cost) == NULL){
            /* for every child node where the cost is not assigned,
               recursively assign the cost to that child node, and 
               increment 'null_count' */
            recursive_assign_costs((root->node)[i]); 
            null_count += 1;
        }
    }
   
    /* if all child nodes have their costs assigned, compute the
       parent node's cost using the parent's comparison function */
    if (!null_count){
        /* initialise cost to the first child node's cost */
        value = *(((root->node)[0])->rational_cost);
        for (i=1; i<(root->nboards); i++){
            value = (root->cmp)(*((root->node)[i]->rational_cost), value);
        }
        
        root->rational_cost = (int*)malloc(sizeof(int));
        assert(root->rational_cost != NULL);

        *(root->rational_cost) = value;
    }
    
    return;
    
}

/* returns the minimum of two integers */
int
min(int num1, int num2){
    if (num1 < num2){
        return num1;
    }
    else{
        return num2;
    }
}

/* returns the maximum of two integers */
int
max(int num1, int num2){
    if (num1 > num2){
        return num1;
    }
    else{
        return num2; 
    }
}


/* computes the most rational move at the tree root, returns a pointer
   to a fresh tree with the new board after that rational move, as well as
   freeing up the memory allocated to the old tree */  
tree_t
*next_action(tree_t *tree){
    int cost, node_num, move_num;
    char *move;
    char **board;
    int (*cmp)(int, int);
    tree_t *new_tree;
    
    cost = *(tree->root->rational_cost);
    
    /* returns a NULL pointer if either player cannot make any more moves 
       and frees up the last tree */
    if (cost == INT_MIN){
        printf("WHITE WIN!\n");
        free_tree(tree);
        return NULL;
    }
    
    if (cost == INT_MAX){
        printf("BLACK WIN!\n");
        free_tree(tree);
        return NULL;
    }
    
    /* searches for first child node that contains the board with the cost
       associated with the rational move */
    for (node_num=0; node_num<(tree->root->nboards); node_num++){
        if (*(((tree->root->node)[node_num])->rational_cost) == cost){
            /* first child node has been found ... */
            move = (tree->root->node)[node_num]->prev_move;
            move_num = ((tree->root->node)[node_num]->move_num);
            board = (tree->root->node)[node_num]->board;
            cmp = (tree->root->node)[node_num]->cmp;    
            
            /* prints action and resulting board in required format */
            printf("%s\n*** ", SEPARATOR);
            if ((move_num - 1) % 2){
            printf("BLACK ");
            }
            
            else{
            printf("WHITE ");
            }
        
            printf("ACTION #%d: %s\n", move_num - 1, move);
            printf("BOARD COST: %d\n", board_cost(board));
            print_board(board);
            
            /* returns pointer to the new tree, freeing up the old tree */
            new_tree = make_empty_tree(cmp, board, move_num);
            free_tree(tree);
            return new_tree;
        }
    }
    
    /* to prevent memory leak warning message */
    free_tree(tree);
    
    return NULL;
}

/* frees memory allocated to a board */
void
free_board(char **board){
	int row;
	
    for (row=0; row<BOARD_SIZE; row++){
        free(board[row]);
        board[row] = NULL;
    }      
    
	free(board);
    board = NULL;
}

/* frees memory allocated to a tree and its components */
void
free_tree(tree_t *tree){
    assert(tree != NULL);
	recursive_free_root(tree->root);
	free(tree);
	tree = NULL;
}

/* recursively frees memory allocated to all nodes and their components */
void
recursive_free_root(node_t* root){
    int node_num;
    
    /* base cases : 
       (root->node) pointers don't exist as they have not been
       allocated for depths of 3 or nodes with no child nodes */
    if ((root->depth == 3) || (root->nboards == 0)){
        /* free components of base case node, and set its pointers to NULL */  
        free(root->prev_move);
        (root->prev_move) = NULL;
        
        free(root->rational_cost);
        (root->rational_cost) = NULL;
        
        free_board(root->board);
        
        free(root);
        root = NULL;
        
        return;
    }
    
    /* if not base case, recursively free all its children first ... */
    for (node_num=0; node_num<(root->nboards); node_num++){
        recursive_free_root((root->node)[node_num]);
    }
    
    /* ... and then free up its own components, and set its pointers to NULL */
    free(root->node);
    (root->node) = NULL;
    
    if (root->prev_move){
    free(root->prev_move);
    (root->prev_move) = NULL;
    }
    
    free(root->rational_cost);
    (root->rational_cost) = NULL;
    
    free_board(root->board);
    
    free(root);
    root = NULL;
    return;
}
    
        
/* algorithms are fun */
