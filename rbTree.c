#include "rbTree.h"
#include "list.h"

struct dataRBTree *rbTreeSearch(struct rb_root *root, ElementType lbaMain)
{
	struct rb_node *node = root->rb_node;  // Top of the tree.

	while (node)
	{
		struct dataRBTree *data = rb_entry(node, struct dataRBTree, node);
		
		if (data->lbaMain > lbaMain)
			node = node->rb_left;
		else if (data->lbaMain < lbaMain)
			node = node->rb_right;
		else			
			return data;
	}
	
	return NULL;
}

// Insert element in RBTree without check intersections.
void rbTreeInsertSimple(struct rb_root *root, struct dataRBTree *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

  	while (*new) 
  	{
  		struct dataRBTree *this = container_of(*new, struct dataRBTree, node);

		parent = *new;
  		if (this->lbaMain > data->lbaMain)
  			new = &((*new)->rb_left);
  		else
  			new = &((*new)->rb_right);
  	}

  	// Add new node and rebalance tree.
  	rb_link_node(&data->node, parent, new);
  	rb_insert_color(&data->node, root);
}

// Removes excess pieces of memory.
void rbTreeCut(struct rb_root *root, ElementType lbaMain, ElementType lbaAux, ElementType length)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	struct dataRBTree *itemRBTree = NULL;
	int choice = 0;

  	while (*new) 
  	{
  		struct dataRBTree *this = container_of(*new, struct dataRBTree, node);

		parent = *new;
  		if (lbaMain > this->lbaMain && lbaMain + length < this->lbaMain + this->length) // New conteined in old.
		{
			choice = 1;			
			itemRBTree = kmalloc(sizeof(*itemRBTree), GFP_KERNEL);
			itemRBTree->lbaMain = lbaMain + length;
			itemRBTree->lbaAux = 0;
			itemRBTree->length = this->lbaMain + this->length - lbaMain - length ;
			
			this->length = lbaMain - lbaMain;
			new = &((*new)->rb_right);
		}
		else if (lbaMain <= this->lbaMain && lbaMain + length >= this->lbaMain + this->length) // Old conteined in new.
		{			
			rbTreeCut(root, lbaMain, lbaAux, this->lbaMain - lbaMain);
			rbTreeCut(root, this->lbaMain + this->length, lbaAux, lbaMain + length - this->lbaMain - this->length);
			rb_erase(&this->node, root);
			return;
		}
		else if (lbaMain < this->lbaMain + this->length && lbaMain + length >= this->lbaMain + this->length) // New to the right.
		{
			this->length = lbaMain - this->lbaMain;
			new = &((*new)->rb_right);
		}
		else if (lbaMain + length > this->lbaMain && this->lbaMain >= lbaMain) // New to the left.
		{
			this->length -= lbaMain + length - this->lbaMain;
			this->lbaMain = lbaMain + length; 
			new = &((*new)->rb_left);
		}			
  		else if (this->lbaMain > lbaMain)
  			new = &((*new)->rb_left);
  		else
  			new = &((*new)->rb_right);
  	}
  	
  	if (choice)
		rbTreeInsertSimple(root, itemRBTree);
}

void rbTreeInsert(struct rb_root *root, struct dataRBTree *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	struct dataRBTree *itemRBTree = NULL;
	int choice = 0;
	
  	while (*new) 
  	{
  		struct dataRBTree *this = container_of(*new, struct dataRBTree, node);

		parent = *new;	
		if (data->lbaMain > this->lbaMain && data->lbaMain + data->length < this->lbaMain + this->length) // New conteined in old.
		{
			choice = 1;			
			itemRBTree = kmalloc(sizeof(*itemRBTree), GFP_KERNEL);
			itemRBTree->lbaMain = data->lbaMain + data->length;
			itemRBTree->lbaAux = data->lbaAux + data->length;
			itemRBTree->length = this->lbaMain + this->length - data->lbaMain - data->length;
			
			this->length = data->lbaMain - this->lbaMain;
			new = &((*new)->rb_right);
		}
		else if (data->lbaMain <= this->lbaMain && data->lbaMain + data->length >= this->lbaMain + this->length) // Old conteined in new.
		{			
			rbTreeCut(root, data->lbaMain, data->lbaAux, this->lbaMain - data->lbaMain);
			rbTreeCut(root, this->lbaMain + this->length, data->lbaAux, data->lbaMain + data->length - this->lbaMain - this->length);
			this->lbaMain = data->lbaMain;
			this->lbaAux = data->lbaAux;
			this->length = data->length;
			return;
		}
		else if (data->lbaMain < this->lbaMain + this->length && data->lbaMain + data->length >= this->lbaMain + this->length) // New to the right.
		{
			this->length = data->lbaMain - this->lbaMain;
			new = &((*new)->rb_right);
		}
		else if (data->lbaMain + data->length > this->lbaMain && this->lbaMain >= data->lbaMain) // New to the left.
		{
			this->length -= data->lbaMain + data->length - this->lbaMain;
			this->lbaMain = data->lbaMain + data->length;
			this->lbaAux = this->lbaAux + data->length;
			new = &((*new)->rb_left);
		}			
  		else if (this->lbaMain > data->lbaMain)
  			new = &((*new)->rb_left);
  		else
  			new = &((*new)->rb_right);
  	}
	
  	// Add new node and rebalance tree.
  	rb_link_node(&data->node, parent, new);
  	rb_insert_color(&data->node, root);
  	
  	
	if (choice)
		rbTreeInsertSimple(root, itemRBTree);
}

void printNode(struct dataRBTree *data)
{
    printk("%lld\n", data->lbaMain);
}

void printTreePreOrder(struct rb_node* node)
{
    struct dataRBTree *temp;
    
    if (node)
    {
		temp = rb_entry(node, struct dataRBTree, node);
        printk("%lld ", temp->lbaMain);
        printk("%lld ", temp->lbaAux);
        printk("%lld\n", temp->length);  
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
        printk("%lld ", temp->lbaMain);
        printk("%lld ", temp->lbaAux);
        printk("%lld\n", temp->length);
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
        printk("%lld ", temp->lbaMain);
        printk("%lld ", temp->lbaAux);
        printk("%lld\n", temp->length);
    }
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
}
