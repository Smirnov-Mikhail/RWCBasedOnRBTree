#pragma once

#include <linux/rbtree.h>
#include <linux/slab.h>

typedef uint64_t ElementType;

extern unsigned long long int countOfNodes;

struct dataRBTree
{
	ElementType lbaMain;
	ElementType lbaAux;
	ElementType length;
	struct rb_node node;
};

// Searching an existing node for a value.
struct dataRBTree *rbTreeSearch(struct rb_root *root, ElementType lbaMain);

// Insert value in a tree.
void rbTreeInsert(struct rb_root *root, struct dataRBTree *new);

// Print tree. (Pre-order, In-order, Post-order or Level-order(not completed))
void rbTreePrint(struct rb_root *root, int order);

// Correct tree (seq write or old conteined in new).
void rbTreeCorrect(struct rb_root *root, struct rb_node **new, ElementType lbaMain, ElementType lbaAux, ElementType length);

// Remove data from RBTree.
void removeDataFromRBTree(struct rb_root *root, long long int size);
