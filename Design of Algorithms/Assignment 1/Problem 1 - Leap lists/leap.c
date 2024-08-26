/* 
leap.c

Implementations for leap list construction and manipulation.

Skeleton written by Grady Fitzaptrick for COMP20007 Assignment 1 2022
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "leap.h"
#include "utils.h"


// a list node points to the next, previous and downwards nodes in the list, and to some data
struct node {
	Node *next;
    Node *prev;
    Node *down;
	data_t data;
};

// helper function to create a new node and return its address
Node *new_node();

// helper function to clear memory of a node (does not free the node's data)
void free_node(Node *node);

// a list points to its first and last nodes, and the lists above and below them
struct list {
	Node *head;
	Node *tail;
    struct list *aboveList;
    struct list *belowList;
};

struct leapList {
    int maxHeight;
    double prob;
    enum problemPart part;
    List *bottomList; // pointer to bottom list of leap list structure
    List *topList; //pointer to top list of leap list structure
};

struct leapList *newList(int maxHeight, double p, enum problemPart part){
    // leapList must have a height of at least 1
    if (maxHeight < 1) {
        return NULL;
    }

    // allocating memory for the leap list and initialising variables
    struct leapList *newList = malloc(sizeof(* newList));
    assert(newList);

    newList->maxHeight = maxHeight;
    newList->prob = p;
    newList->part = part;

    // allocating memory to the bottom list of the leap list structure
    newList->bottomList = new_list();
    List *currentList = newList->bottomList;

    // allocate memory to consecutive lists until maximum height is reached
    int currentHeight = 1;
    while (currentHeight < maxHeight) {
        currentList->aboveList = new_list();
        currentList->aboveList->belowList = currentList;
        currentList = currentList->aboveList;
        currentHeight += 1;
    }

    // assigning the top list of the leap list structure
    newList->topList = currentList;

    return newList;
}

void printLevel(struct leapList *list, int level){
    // list is undefined
    if(! list){
        printf("\n");
        return;
    }

    // level is outside range
    if (level < 1 || level > list->maxHeight) {
        printf("\n");
        return;
    }

    // finding the right list in the leap list structure
    List *currentList = list->bottomList;

    for (int i=1; i<level; i++) {
        currentList = currentList->aboveList;
    }

    // printing in the correct format
    printList(currentList);
    printf("\n");

    return;
}

void insertKey(int key, struct leapList *list){

    assert(list);

    // creating the node to be inserted
    Node *newNode = new_node();
    assert(newNode);
    newNode->data = key;
    
    // temporary filler pointers to use with the locateKey function
    int *tmp1 = malloc(sizeof(int));
    int *tmp2 = malloc(sizeof(int));
    assert(tmp1 && tmp2);
    
    *tmp1 = 0;
    *tmp2 = -1;

    // locates the position of the node where insertion takes place
    Node *locatedNode = locateKey(list, list->maxHeight, key, tmp1, tmp2);

    // if no node is located, it is the first insertion into the leap list
    if (!locatedNode) {
        list->bottomList->head = newNode;
    } 
    
    // if node has a data value greater than the key, insert it at the head of the bottom list
    else if (locatedNode->data > key) {
        locatedNode->prev = newNode;
        newNode->next = locatedNode;
        list->bottomList->head = newNode;
    } 
    
    // if node has a data value less than the key, insert it after this node
    else {
        if (locatedNode->next) {
            locatedNode->next->prev = newNode;
        }
        newNode->prev = locatedNode;
        newNode->next = locatedNode->next;
        locatedNode->next = newNode; 
    }

    // calls function to insert the key at all higher levels of the leap list
    recursiveInsertKey(list, newNode, list->maxHeight - 1, key, tmp1, tmp2);
    
    free(tmp1);
    free(tmp2);
}

/* Recursively inserts the key into higher lists in the linked list structure. */
void recursiveInsertKey(struct leapList *list, Node *downNode, int levels, int key, int *tmp1, int *tmp2){

    // 'levels' determines how many levels from the top of the leaped list will be explored
    // function returns if no levels left to explore
    if (levels < 1) {
        return;
    }

    // accounting for probability for element to be found on this new level
    if ((double) rand() / RAND_MAX >= list->prob) {
        return;
    }

    // creating the new node to be inserted
    Node *newNode = new_node();
    assert(newNode);
    newNode->data = key;

    // locates the position of the node where insertion takes place
    Node *locatedNode = locateKey(list, levels, key, tmp1, tmp2);

    // if no node is located, it is the first insertion into the deepest list (restricted by levels)
    if (! locatedNode) {

        // finding its head
        List *newBottomList = list->topList;
        for (int i=0; i<levels-1; i++) {
            newBottomList = newBottomList->belowList;
        }

        // and updating it
        newBottomList->head = newNode;
    } 
    
    // node found has a value greater than key, so all values in the list are greater than the key
    else if (locatedNode->data > key) {

        // finding head of the deepest list (restricted by levels)
        List *newBottomList = list->topList;
        for (int i=0; i<levels-1; i++) {
            newBottomList = newBottomList->belowList;
        }

        // and updating the pointers in the list
        locatedNode->prev = newNode;
        newNode->next = locatedNode;
        newBottomList->head = newNode;
    } 
    
    // new node is attached after the located node, no need to update the head of any list
    else {
        // updating the pointers in the list
        if (locatedNode->next) {
            locatedNode->next->prev = newNode;
        }

        newNode->prev = locatedNode;
        newNode->next = locatedNode->next;
        locatedNode->next = newNode;
    }

    // connecting the new node with the node on the level below
    newNode->down = downNode;

    // and repeat, but with one less level to explore from the top of the leaped list
    recursiveInsertKey(list, newNode, levels - 1, key, tmp1, tmp2);    
}


/* Queries the leap list for the given key and places the result in the solution structure. */
int findKey(int key, struct leapList *list, enum problemPart part, struct solution *solution){
    int found = NOTFOUND;
    int element = key;
    int baseAccesses = 0;
    int requiredAccesses = 0;
    assert(solution);

    // finds the node that stores the relevant data required for variable 'baseAccesses'
    // also determines the variables 'requiredAccesses' and 'found'
    Node *currentNode = locateKey(list, list->maxHeight, element, &requiredAccesses, &found);

    // determines the variable 'baseAccesses' using the located node
    if (currentNode) {
        currentNode = list->bottomList->head;
        baseAccesses = 1;

        if (key > currentNode->data) {
            while (currentNode->next) {
                baseAccesses += 1;
                currentNode = currentNode->next;
                if (currentNode->data >= key) {
                    break;
                }
            }
        }
    }

    /* Insert result into solution. */
    (solution->queries)++;
    solution->queryResults = (int *) realloc(solution->queryResults, sizeof(int) * solution->queries);
    assert(solution->queryResults);
    (solution->queryResults)[solution->queries - 1] = found;
    solution->queryElements = (int *) realloc(solution->queryElements, sizeof(int) * solution->queries);
    assert(solution->queryElements);
    solution->queryElements[solution->queries - 1] = element;
    solution->baseAccesses = (int *) realloc(solution->baseAccesses, sizeof(int) * solution->queries);
    assert(solution->baseAccesses);
    solution->baseAccesses[solution->queries - 1] = baseAccesses;
    solution->requiredAccesses = (int *) realloc(solution->requiredAccesses, sizeof(int) * solution->queries);
    assert(solution->requiredAccesses);
    solution->requiredAccesses[solution->queries - 1] = requiredAccesses;

    return found;
}

/* locates the node with data value closest to the key required
 * also determines 'requiredAccesses' and 'found' variables required for findKey function */
Node *locateKey(struct leapList *list, int levels, int key, int *requiredAccesses, int *found) {

    int currentHeight = list->maxHeight;
    List *currentList = list->topList;
    Node *currentNode = currentList->head;

    // propogate down levels of lists as long as the head points to null
    while (! currentNode && currentHeight > list->maxHeight - levels + 1) {
        currentList = currentList->belowList;
        currentNode = currentList->head;
        currentHeight -= 1;
    }
    
    Node *prevNode;
    int prevData;

    // check incase the head on the bottom list also points to a null
    if (currentNode) {

        // first pointer is accessed
        *requiredAccesses += 1;

        // accessing the head value of lists below the current list
        // when the head value remains greater than the key
        while (currentNode->data > key && currentHeight > list->maxHeight - levels + 1) {

            // keeping track of previous head nodes value
            prevNode = currentNode;
            prevData = prevNode->data;

            // updating new (lower) head's value
            currentList = currentList->belowList;
            currentNode = currentList->head;
            currentHeight -= 1;
            
            // new pointer access if previous and new head values are different
            if (currentNode->data != prevData) {
                *requiredAccesses += 1;
            }
        }

        // new key is smaller than head value of the bottom list, so return this head
        if (currentNode->data > key && currentHeight == 1) {
            return currentNode;
        }
    }

    
    // no longer need to access heads of lists as current node is either null,
    // or only moves forwards from this point on (away from heads of lists)
    if (currentNode) {
        while (currentNode) {
            // have not reached bottom of leap list
            if (currentHeight > list->maxHeight - levels + 1) {

                // move down with no new pointer accesses if next node is null
                if (! currentNode->next) {
                    currentNode = currentNode->down;
                    currentHeight -= 1;
                }

                // move down with no new pointer accesses if data matches key
                else if (currentNode->data == key) {
                    currentNode = currentNode->down;
                    currentHeight -= 1;
                }

                // move right with new pointer access if next data is no greater than key
                else if (currentNode->next->data <= key) {
                    currentNode = currentNode->next;
                    *requiredAccesses += 1;
                }

                // move down with new pointer access if next data is greater than key
                else {

                    *requiredAccesses += 1;

                    // keep track of value of node to the right that pushes current node down
                    prevNode = currentNode->next;

                    currentNode = currentNode->down;
                    currentHeight -= 1;

                    // keep moving down while no new pointer accesses
                    // (node to the right remains the same value)
                    while (prevNode->down->data == currentNode->next->data && 
                           currentHeight > list->maxHeight - levels + 1) {
                        prevNode = currentNode->next;
                        currentNode = currentNode->down;
                        currentHeight -= 1;
                    }
                    
                    // reached bottom list, but still no new pointer accesses
                    if (prevNode->down->data == currentNode->next->data) {
                        break;
                    }
                }
            }
            
            // lowest list of leap list
            else {

                // found the element in the list
                if (currentNode->data == key) {
                    *found = 1;
                    break;
                }
                
                // did not find the element in the list
                else if (! currentNode->next) {
                    break;
                }

                // did not find element in the list, but with new pointer access
                else if (currentNode->next->data > key) {
                    *requiredAccesses += 1;
                    break;
                }

                // next number in the list is smaller, so keep searching for the element
                else {
                    currentNode = currentNode->next;
                    *requiredAccesses += 1;
                }
            }
        }
    }
 
    return currentNode;
}

void deleteKey(int key, struct leapList *list, enum problemPart part){

    assert(list);

    int *tmp1 = malloc(sizeof(int));
    int *tmp2 = malloc(sizeof(int));

    assert(tmp1 && tmp2);
    
    *tmp1 = 0;
    *tmp2 = -1;

    recursiveDeleteKey(list, list->maxHeight, key, tmp1, tmp2);

    free(tmp1);
    free(tmp2);
}

// recursively deletes the key from all levels in the leap list
void recursiveDeleteKey(struct leapList *list, int levels, int key, int *tmp1, int *tmp2) {

    // levels determines which list in the leap list to delete the key from
    if (levels < 1) {
        return;
    }

    // locates node to delete
    Node *locatedNode = locateKey(list, levels, key, tmp1, tmp2);

    // no node found to delete, or the data of the node does not match the key
    if (! locatedNode || locatedNode->data != key) {
        return;
    } 
    
    // deletes, frees the node and updates relevant pointers in the list
    else {
        if (locatedNode->next) {
            locatedNode->next->prev = locatedNode->prev;
        }

        if (locatedNode->prev) {
            locatedNode->prev->next = locatedNode->next;
        } 

        else {
            List *newBottomList = list->topList;
                for (int i = 0; i < levels - 1; i++) {
                newBottomList = newBottomList->belowList;
                }
            newBottomList->head = locatedNode->next;
        }

        locatedNode->prev = locatedNode->next = locatedNode->down = NULL;
        free(locatedNode);
    }
    
    // repeat, but deleting from the next list up
    recursiveDeleteKey(list, levels - 1, key, tmp1, tmp2);

}

void freeList(struct leapList *list){
    assert(list);

    List *currentList = list->topList;
    List *nextList;

    while (currentList) {
        nextList = currentList->belowList;
        free_list(currentList);
        currentList = nextList;
    }

    list->bottomList = list->topList = NULL;

    free(list);
}

void freeSolution(struct solution *solution){
    if(! solution){
        return;
    }
    freeList(solution->list);
    if(solution->queries > 0){
        free(solution->queryResults);
        free(solution->queryElements);
        free(solution->baseAccesses);
        free(solution->requiredAccesses);
    }
    free(solution);
}


// create a new list and return a pointer to it
List *new_list() {
	List *list = malloc(sizeof *list);
	assert(list);

	list->head = NULL;
	list->tail = NULL;
	list->aboveList = NULL;
    list->belowList = NULL;

	return list;
}

// prints out the elements in a list
void printList(List *list) {
    assert(list);

    Node *node = list->head;

    while (node) {
        if (node->next) {
            printf("%d ", node->data);
        } 
        else {
            printf("%d", node->data);
        }
        node = node->next;
    }
}

// destroy a list and free its memory
void free_list(List *list) {
	assert(list != NULL);
	// free each node
	Node *node = list->head;
	Node *next;
	while (node) {
		next = node->next;
		free_node(node);
		node = next;
	}

    list->aboveList = NULL;
    list->belowList = NULL;

	// free the list struct itself
	free(list);
}

// helper function to create a new node and return its address
Node *new_node() {
	Node *node = malloc(sizeof *node);
	assert(node);

	return node;
}

// helper function to clear memory of a node
void free_node(Node *node) {
    node->next = node->prev = node->down = NULL;
	free(node);
}
