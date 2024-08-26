typedef struct node node_t;

struct node {
	void *data;              /* ptr to stored structure */
	node_t *left;            /* left subtree of node */
	node_t *rght;            /* right subtree of node */
};

typedef struct {
	node_t *root;            /* root node of the tree */
	int (*cmp)(void*,void*); /* function pointer */
} tree_t;

/* prototypes for the functions in this library */
tree_t *make_empty_tree(int func(void*,void*));
int is_empty_tree(tree_t *tree);
void *search_tree(tree_t *tree, void *key);
tree_t *insert_in_order(tree_t *tree, void *value);
void traverse_tree(tree_t *tree, void action(void*));
void free_tree(tree_t *tree);        