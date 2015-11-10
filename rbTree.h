#include <linux/rbtree.h>

typedef uint64_t ElementType;

struct dataRBTree
{
	ElementType value;
	struct rb_node node;
};

// Searching an existing node for a value.
struct dataRBTree *rbTreeSearch(struct rb_root *root, ElementType value);

// Insert value in a tree.
void rbTreeInsert(struct rb_root *root, struct dataRBTree *new);

// Print tree. (Pre-order, In-order, Post-order or Level-order(not completed))
void printTree(struct rb_root *root, int order);
