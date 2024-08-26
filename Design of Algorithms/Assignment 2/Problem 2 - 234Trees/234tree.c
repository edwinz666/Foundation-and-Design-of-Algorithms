/* 
234tree.c

Implementation for a 2-3-4 tree.

Original solution written by Grady Fitzpatrick for COMP20007 Week 9 Workshop
    with modifications for skeleton.
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "234tree.h"

/* These constants are not responsive, changing them from their value will
likely cause issues in the operation of the tree without other changes. */
#define MAXCHILDREN 4
#define MAXVALUES 3

struct tree {
    struct tree *child0;
    struct tree *child1;
    struct tree *child2;
    struct tree *child3;
    int value1;
    int value2;
    int value3;
    int numVals;
    int numDescendants;
};

/* Inserts a value into the given node. */
void insertIntoNode(struct tree *tree, int value);
/* Split the middle value from the child into the tree node and insert into 
the tree node recursively. */
struct tree *splitAndInsert(struct tree *tree, struct tree *child, int value);
/* Insert the item into the tree using pre-emptive splitting. */
struct tree *insertTreeRecursive(struct tree *tree, int value);

int countNodes(struct tree *tree);

int nth_element(struct tree *tree, int n);

void updateDescendants(struct tree *tree);

void recursiveFindMedian(struct tree *tree, int low, int index, int level);

void insertIntoNode(struct tree *tree, int value){
    if(tree->numVals == 1){
        tree->value2 = value;
    } else if(tree->numVals == 2){
        tree->value3 = value;
    }
    tree->numVals = tree->numVals + 1;

    /* insertionSort(tree->value[1..3]), all values before final are sorted. */
    if(tree->numVals > 2){
      if(tree->value3 < tree->value2){
        tree->value3 = tree->value2;
        tree->value2 = value;
      }
    }
    if(tree->numVals > 1){
      if(tree->value2 < tree->value1){
        tree->value2 = tree->value1;
        tree->value1 = value;
      }
    }
}

struct tree *splitAndInsert(struct tree *tree, struct tree *child, int value){
    /* Take median value, place in the tree. */

    /* Find which child the value being split up comes from. */
    int insertLocation = 0;
    if(tree->child0 == child){
        insertLocation = 0;
    } else if(tree->child1 == child){
        insertLocation = 1;
    } else if(tree->child2 == child){
        insertLocation = 2;
    } else if(tree->child3 == child){
        insertLocation = 3;
    }

    /* Insert into empty trees to lazily do allocation. */
    struct tree *left = insertTree(NULL, child->value1);
    struct tree *right = insertTree(NULL, child->value3);
    left->child0 = child->child0;
    left->child1 = child->child1;
    right->child0 = child->child2;
    right->child1 = child->child3;

    switch(insertLocation){
        case 0:
            /* Inserting in first value, shuffle all along. */
            tree->child3 = tree->child2;
            tree->value3 = tree->value2;
            tree->child2 = tree->child1;
            tree->value2 = tree->value1;
            /* Place median child value in 1st value. */
            tree->value1 = child->value2;
            tree->child0 = left;
            tree->child1 = right;
            break;
        case 1:
            /* Inserting in second value, shuffle all after first along. */
            tree->child3 = tree->child2;
            tree->value3 = tree->value2;
            /* Place median child value in 2nd value. */
            tree->value2 = child->value2;
            tree->child1 = left;
            tree->child2 = right;
            break;
        case 2:
            /* Inserting in third value, no shuffling needed. */
            /* Place median child value in 3rd value. */
            tree->value3 = child->value2;
            tree->child2 = left;
            tree->child3 = right;
            break;
        case 3:
            /* Child being pushed up being the third child implies the node is
              a 4-node. This should never happen as the tree node should have
              been split before its child was checked (pre-emptive split).
            */
            assert(insertLocation < MAXVALUES);
            break;
        default:
            /* Should be 2-3-4. */
            assert(insertLocation < MAXCHILDREN && insertLocation >= 0);
    }
    /* Value inserted now, so increase number of items by one. */
    tree->numVals = tree->numVals + 1;

    /* Clean up replaced child node. */
    free(child);
    return insertTreeRecursive(tree, value);
}

/* Handles the first node, where either:
  1. The tree contains 3-elements at the root and must be pre-emptively split.
  2. The tree contains no nodes and the root must be created.
  3. The tree does not contain 3-elements and the usual insertion begins.
*/
struct tree *insertTree(struct tree *tree, int value){
    if(tree == NULL){
        tree = (struct tree *) malloc(sizeof(struct tree));
        assert(tree);
        tree->child0 = NULL;
        tree->child1 = NULL;
        tree->child2 = NULL;
        tree->child3 = NULL;
        tree->value1 = value;
        tree->value2 = -1;
        tree->value3 = -1;
        tree->numVals = 1;
        tree->numDescendants = 0; // ?????
    } else {
        if(tree->numVals == 3){
            /* Insert into empty trees to lazily do allocation. */
            struct tree *left = insertTree(NULL, tree->value1);
            struct tree *root = insertTree(NULL, tree->value2);
            struct tree *right = insertTree(NULL, tree->value3);
            /* Distribute children. */
            left->child0 = tree->child0;
            left->child1 = tree->child1;
            right->child0 = tree->child2;
            right->child1 = tree->child3;
            root->child0 = left;
            root->child1 = right;
            /* Clean up old node */
            free(tree);
            tree = root;
        }
        tree = insertTreeRecursive(tree, value);
        
        // update numDescendants in all nodes after insertion
        updateDescendants(tree);
    }

    return tree;
}

// added some comments for personal understanding
struct tree *insertTreeRecursive(struct tree *tree, int value){

    // no children nodes, put in the same tree
    if(tree->child0 == NULL){
        insertIntoNode(tree, value);
    } 
    
    // child0 exists, child1 also must exist, and value is smaller than value1
    else if(value < tree->value1) {
        // child0 node has 3 values, split and insert
        if(tree->child0->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child0, value);
        } 
        // child0 node has less than 3 values
        else {
            tree->child0 = insertTreeRecursive(tree->child0, value);
        }
    } 
    
    // child0 exists, child1 also must exist, value is greater than value1
    // tree has only 1 value (and value is greater than this value1) 
    // OR value to compare is less than value2 (AND numVal >= 2 from failing first condition)
    else if(tree->numVals < 2 || value < tree->value2){
        // child1 has 3 values...
        if(tree->child1->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child1, value);
        } 
        // child1 has less than 3 values
        else {
            tree->child1 = insertTreeRecursive(tree->child1, value);
        }
    } 
    
    // child0, child1 exists, child2 exists
    // tree has only 2 values (and value greater than this value2)
    // OR value to compare is less than value3, (AND numVal == 3 from failing first condition)
    else if(tree->numVals < 3 || value < tree->value3){
        if(tree->child2->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child2, value);
        } else {
            tree->child2 = insertTreeRecursive(tree->child2, value);
        }
    } 
    
    // tree has 3 values and value greater than value3
    else {
        if(tree->child3->numVals > (MAXVALUES - 1)){
            tree = splitAndInsert(tree, tree->child3, value);
        } else {
            tree->child3 = insertTreeRecursive(tree->child3, value);
        }
    }

    return tree;
}

void printTree(struct tree *tree, int level){
    if(tree == NULL){
        return;
    }
    /* Pre-order traversal. */
    // printf("[ %d, %d, %d ] (level %d)\n", tree->value1, tree->value2, tree->value3, level);

    /* In-order traversal. */
    printTree(tree->child0, level + 1);
    if(tree->value1 != (-1)){
        printf("%d (level %d)\n", tree->value1, level);
    }
    printTree(tree->child1, level + 1);
    if(tree->value2 != (-1)){
        printf("%d (level %d)\n", tree->value2, level);
    }
    printTree(tree->child2, level + 1);
    if(tree->value3 != (-1)){
        printf("%d (level %d)\n", tree->value3, level);
    }
    printTree(tree->child3, level + 1);

}

void findMedian(struct tree *tree){
    
    /*int numNodes = tree->numVals + tree->numDescendants;
    int index = (numNodes + 1)/2;
    recursiveFindMedian(tree, 1, index, 0);*/

    int length = countNodes(tree);
    int m_index = (length + 1)/2;
    int median = nth_element(tree, m_index);

    // printf("median = %d\n", m_index);
    
    printf("%d ", median);

}


int countNodes(struct tree *tree){
    if(tree == NULL){
        return 0;
    }
    
    return tree->numVals + countNodes(tree->child0) + countNodes(tree->child1) +
            countNodes(tree->child2) + countNodes(tree->child3);
}

int nth_element(struct tree *tree, int n) {
    static int count = 0;
    static int element = -1;
    
    if (tree == NULL) {
        return element;
    }

    if (count <= n) {
        nth_element(tree->child0, n);

        if (tree->value1 != (-1)) {
            count++;
        }
        if (count == n) {
            element = tree->value1;
            return element;
        }
        nth_element(tree->child1, n);

        if (tree->value2 != (-1)) {
            count++;
        }
        if (count == n) {
            element = tree->value2;
            return element;
        }
        nth_element(tree->child2, n);
        if (tree->value3 != (-1)) {
            count++;
        }
        if (count == n) {
            element = tree->value3;
            return element;
        }
        nth_element(tree->child3, n);
    }
    
    return element;
}

// recursively updates numDescendants in all nodes by updating children first
void updateDescendants(struct tree *tree) {
    // base case : leaf node does not need updating
    if (tree->child0 == NULL) {
        assert(tree->numDescendants == 0);
        return;
    }

    // assert there exists child0 and child1
    assert(tree->child0 && tree->child1);

    // update numDescendants of child0 and then add its contribution to the tree
    updateDescendants(tree->child0);
    tree->numDescendants = (tree->child0)->numDescendants + (tree->child0)->numVals;

    // update numDescendants of child1 and then add its contribution to the tree
    updateDescendants(tree->child1);
    tree->numDescendants += (tree->child1)->numDescendants + (tree->child1)->numVals;

    // update numDescendants of child2 and then add its contribution to the tree
    if (tree->child2) {
        updateDescendants(tree->child2);
        tree->numDescendants += (tree->child2)->numDescendants + (tree->child2)->numVals;
    }
    
    // update numDescendants of child3 and then add its contribution to the tree
    if (tree->child3) {
        updateDescendants(tree->child3);
        tree->numDescendants += (tree->child3)->numDescendants + (tree->child3)->numVals;
    }

}

/* Recursively finds, and then prints the number in the tree 
   with specified index, and its level
   low : represents the lowest index that can be found in the current tree
   index: represents the index to be found (in sorted order)
   level: level of the tree, starting from 0 at the root
*/
void recursiveFindMedian(struct tree *tree, int low, int index, int level) {
    if (tree == NULL) {
        return;
    }

    // median is found in leaf node
    if (tree->child0 == NULL) {
        if (low == index) {
            printf("%d %d\n", tree->value1, level);
        } else if (low + 1 == index) {
            printf("%d %d\n", tree->value2, level);
        } else if (low + 2 == index) {
            printf("%d %d\n", tree->value3, level);
        }
        
        return;
    }

    /* checking child0 ...
       high : currently represents the index of value1 
    */
    int high = low + (tree->child0)->numDescendants + (tree->child0)->numVals;

    // median index is found in child0 of current tree
    if (index < high) {
        recursiveFindMedian(tree->child0, low, index, level + 1);
        return;
    }

    // index of value1 is index of median, so value1 is the median
    if (high == index) {
        printf("%d %d\n", tree->value1, level);
        return;
    }

    /* now checking child1 ...
       update low and high bounds for possible indexes in child1 
    */
    low = high + 1;
    high = low + (tree->child1)->numDescendants + (tree->child1)->numVals; 

    // median index is found in child1
    if (index < high) {
        recursiveFindMedian(tree->child1, low, index, level + 1);
        return; // ????
    }

    // index of value2 is index of median, so value2 is the median
    if (high == index) {
        printf("%d %d\n", tree->value2, level);
        return;
    }

    /* now checking child2 ...
       update low and high bounds for possible indexes in child2 
    */
    low = high + 1;
    high = low + (tree->child2)->numDescendants + (tree->child2)->numVals; 

    // median is found in child2
    if (index < high) {
        recursiveFindMedian(tree->child2, low, index, level + 1);
        return; // ????
    }

    // index of value 3 is index of median, so value3 is the median
    if (high == index) {
        printf("%d %d\n", tree->value3, level);
        return;
    }

    /* median is found in child3 if all else fails
       lowest possible index becomes 1 above the index of value3
    */
    low = high + 1;
    recursiveFindMedian(tree->child3, low, index, level + 1);

}
