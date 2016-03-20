#pragma once

#include <linux/slab.h>
#include <linux/list.h>
#include <linux/kernel.h>
#include <linux/delay.h>

typedef uint64_t ElementType;
extern const int numberBuckets;

struct dataListHash
{
	ElementType lbaMain;
	ElementType lbaAux;
	ElementType length;
	struct list_head list;
};

// Correct hash table (seq write or old conteined in new).
void hashTableCorrect(struct list_head *hashList, int numberOfBucket, ElementType lbaMain, ElementType lbaAux, ElementType length);

// Insert value in a hash table.
void hashTableInsert(struct list_head *hashList, struct dataListHash *new);

// Print hash table.
void hashTablePrint(struct list_head *hashList);

	/*struct dataListHash *v = NULL;
	struct dataListHash *q = NULL;
	//struct list_head *iter = NULL;
	struct list_head hashList[numberBuckets];
	int i;
	
	for (i = 0; i < numberBuckets; ++i)
		INIT_LIST_HEAD(&hashList[i]); //or LIST_HEAD(mylist); 
	
	v = kmalloc(sizeof(*v), GFP_KERNEL);
	v->lbaMain = 11;

	list_add(&v->list, &hashList[2]);

	q = list_first_entry(&hashList[2], struct dataListHash, list);
	hashTablePrint(hashList);*/
