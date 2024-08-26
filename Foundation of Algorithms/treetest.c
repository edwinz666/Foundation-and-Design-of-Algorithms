#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct tree Tree; 

struct tree {
    Tree *left;
    Tree *right;
    int data;
}

Tree *insertIntoTree(Tree *tree, int value);
int sumTree(Tree *tree);

int main(int argc, char **argv) {
    Tree *tree = NULL;
    tree = insertIntoTree(tree, 6);
    tree = insertIntoTree(tree, 4);
    tree = insertIntoTree(tree, 0);
    tree = insertIntoTree(tree, 3);
    tree = insertIntoTree(tree, 8);
    tree = insertIntoTree(tree, 7);
    tree = insertIntoTree(tree, 9);

    printf("%d", sumTree(tree));
}



Tree *insertIntoTree(Tree *tree, int value) {
    if (tree == NULL) {
        tree = (Tree *)malloc(sizeof(Tree));
        assert(tree);

        tree->data = value;
        tree->left = NULL;
        tree->right = NULL;
    } else{
        if (value < tree->data) {
            tree = insertIntoTree(tree->left, value);
        } else {
            tree = insertIntoTree(tree->right, value);
        }
    }

    return tree;
}

int sumTree(Tree *tree) {
    if (tree == NULL) {
        return 0;
    }

    return tree->data + sumTree(tree->left) + sumTree(tree->right);
}