#include "rbTree.h"
#include "list.h"

struct dataRBTree *rbTreeSearch(struct rb_root *root, ElementType value)
{
	struct rb_node *node = root->rb_node;  // Top of the tree.

	while (node)
	{
		struct dataRBTree *data = rb_entry(node, struct dataRBTree, node);
		
		if (data->value > value)
			node = node->rb_left;
		else if (data->value < value)
			node = node->rb_right;
		else			
			return data;
	}
	
	return NULL;
}

void rbTreeInsert(struct rb_root *root, struct dataRBTree *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

  	while (*new) {
  		struct dataRBTree *this = container_of(*new, struct dataRBTree, node);

		parent = *new;
  		if (this->value > data->value)
  			new = &((*new)->rb_left);
  		else
  			new = &((*new)->rb_right);
  	}

  	// Add new node and rebalance tree.
  	rb_link_node(&data->node, parent, new);
  	rb_insert_color(&data->node, root);
}

void printNode(struct dataRBTree *data)
{
    printk("%lld\n", data->value);
}


void printTreePreOrder(struct rb_node* node)
{
    struct dataRBTree *temp;
    
    if (node)
    {
		temp = rb_entry(node, struct dataRBTree, node);
        printk("%lld\n", temp->value);  
        printTreePreOrder(node->rb_left); 
        printTreePreOrder(node->rb_right);
    }
}

void printTreeInOrder(struct rb_node* node)
{
    struct dataRBTree *temp;
    
    if (node)
    {
        printTreeInOrder(node->rb_left);
        temp = rb_entry(node, struct dataRBTree, node);
        printk("%lld\n", temp->value);   
        printTreeInOrder(node->rb_right);
    }
}

void printTreePostOrder(struct rb_node* node)
{
    struct dataRBTree *temp;
    
    if (node)
    {
        printTreePostOrder(node->rb_left);   
        printTreePostOrder(node->rb_right);
        temp = rb_entry(node, struct dataRBTree, node);
        printk("%lld\n", temp->value);
    }
}

void printTreeLevelOrder(struct rb_node* node)
{
	/*
	struct list_head *iter, *iter_safe;
	struct dataList *itemList;
	LIST_HEAD(list);
	
    struct dataRBTree *temp;
    
    itemList = kmalloc(sizeof(*item), GFP_KERNEL);
	itemList->value = (rb_entry(node, struct dataRBTree, node))->value;
	list_add(&(itemList->list), &list); // Add 2^30.
    
    while (list
    {
        printTreePostOrder(node->rb_left);   
        printTreePostOrder(node->rb_right);
        temp = rb_entry(node, struct dataRBTree, node);
        printk("%lld\n", temp->value);
    }*/
}

void printTree(struct rb_root *root, int order)
{
    if (!root->rb_node)
        return;
        
    if (order == -1)
		printTreePreOrder(root->rb_node);
    else if (order == 0)
        printTreeInOrder(root->rb_node);
    else if (order == 1)
		printTreePostOrder(root->rb_node);
	else
		printTreeLevelOrder(root->rb_node);
}
