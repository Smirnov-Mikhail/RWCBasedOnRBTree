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
			printk("1) %lld _ %lld\n", data->lbaMain, data->length);
			choice = 1;			
			itemRBTree = kmalloc(sizeof(*itemRBTree), GFP_KERNEL);
			itemRBTree->lbaMain = data->lbaMain + data->length;
			itemRBTree->lbaAux = 0;
			itemRBTree->length = this->lbaMain + this->length - data->lbaMain - data->length ;
			
			this->length = data->lbaMain - this->lbaMain;
			new = &((*new)->rb_right);
		}
		else if (data->lbaMain > this->lbaMain && data->lbaMain + data->length < this->lbaMain + this->length) // Old conteined in new.
		{			
			printk("2) %lld _ %lld\n", data->lbaMain, data->length);
		}
		else if (this->lbaMain + this->length > data->lbaMain && this->lbaMain < data->lbaMain) // New to the right.
		{
			printk("3) %lld _ %lld\n", data->lbaMain, data->length);
			this->length = data->lbaMain - this->lbaMain;
			new = &((*new)->rb_right);
		}
		else if (data->lbaMain + data->length > this->lbaMain && this->lbaMain > data->lbaMain) // New to the left.
		{
			printk("4) %lld _ %lld\n", data->lbaMain, data->length);
			this->length = data->lbaMain + data->length - this->lbaMain;
			this->lbaMain = data->lbaMain + data->length; 
			new = &((*new)->rb_left);
		}			
  		else if (this->lbaMain > data->lbaMain)
  		{
			printk("5) %lld _ %lld\n", data->lbaMain, data->length);
  			new = &((*new)->rb_left);
		}
  		else
  		{
			printk("6) %lld _ %lld\n", data->lbaMain, data->length);
  			new = &((*new)->rb_right);
		}
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
