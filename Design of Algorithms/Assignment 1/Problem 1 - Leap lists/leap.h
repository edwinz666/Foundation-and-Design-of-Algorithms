/* 
leap.h

Visible structs and functions for leap list construction and manipulation.

Skeleton written by Grady Fitzaptrick for COMP20007 Assignment 1 2022
*/

typedef int data_t;

typedef struct node Node;

typedef struct list List;

struct leapList;

enum problemPart;

/* A particular solution to an leapList problem. */
#ifndef SOLUTION_STRUCT
#define SOLUTION_STRUCT
struct solution {
    int queries;
    int *queryResults;
    int *queryElements;
    int *baseAccesses;
    int *requiredAccesses;
    /* Required for part B */
    struct leapList *list;
};
#endif

/* Which part the program should find a solution for. */
#ifndef PART_ENUM
#define PART_ENUM
enum problemPart {
    PART_A=0,
    PART_B=1
};
#endif

/* Value indicating the item is not found. */
#define NOTFOUND (-1)

/* Creates an empty leap list with the given max height and probability. */
struct leapList *newList(int maxHeight, double p, enum problemPart part);

/* Prints the given level of the list. */
void printLevel(struct leapList *list, int level);

/* Adds a new key to the leap list. */
void insertKey(int key, struct leapList *list);

/* Recursively inserts the key into higher lists in the linked list structure. */
void recursiveInsertKey(struct leapList *list, Node *downNode, int levels, int key, int *tmp1, int *tmp2);

/* Queries the leap list for the given key and places the result in the solution structure. */
int findKey(int key, struct leapList *list, enum problemPart part, struct solution *solution);

/* locates the node with data close to the key in the leap list structure */
Node *locateKey(struct leapList *list, int levels, int key, int *requireAccesses, int *found);

/* Deletes a value from the leap list. */
void deleteKey(int key, struct leapList *list, enum problemPart part);

/* Recursively deletes keys from higher lists in the linked list structure. */
void recursiveDeleteKey(struct leapList *list, int levels, int key, int *tmp1, int *tmp2);

/* Frees all memory associated with the leap list. */
void freeList(struct leapList *list);

/* Frees all memory associated with the solution. */
void freeSolution(struct solution *solution);

// create a new list and return its pointer
List *new_list();

// prints out the elements in a list
void printList(List *list);

// destroy a list and free its memory
void free_list(List *list);