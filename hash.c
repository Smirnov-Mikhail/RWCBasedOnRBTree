#include "hash.h"

const int numberBuckets = 200000;
const int MAX_RAND = 100;
int errorCounter = 0;

void hashTableInsertSimple(struct list_head *hashList, struct dataListHash *new)
{
	// Calculate hash.
	const int numberOfBucket = (new->lbaMain / MAX_RAND) % numberBuckets;

	list_add(&new->list, &hashList[numberOfBucket]);
}

void hashTableCorrect(struct list_head *hashList, int numberOfBucket, ElementType lbaMain, ElementType lbaAux, ElementType length)
{
	struct list_head *iter = NULL, *iter_safe;
	struct dataListHash *this = NULL;
	struct dataListHash *itemHashTable = NULL;
	int choiceNewInOld = 0;
	int choiceOldInNew = 0;
	
	/*errorCounter++;
	if (errorCounter >= 50) // Если зацикливается.
	{
		printk("111 %lld\n", lbaMain);
		return;
	}*/
	
	list_for_each_safe(iter, iter_safe, &hashList[numberOfBucket])
	{
		this = list_entry(iter, struct dataListHash, list);
		if (lbaMain > this->lbaMain && lbaMain + length < this->lbaMain + this->length) // New contained in old.
		{
			choiceNewInOld = 1;			
			itemHashTable = kmalloc(sizeof(*itemHashTable), GFP_KERNEL);
			itemHashTable->lbaMain = lbaMain + length;
			itemHashTable->lbaAux = lbaAux + length;
			itemHashTable->length = this->lbaMain + this->length - lbaMain - length;
			this->length = lbaMain - this->lbaMain;
		}
		else if (lbaMain <= this->lbaMain && lbaMain + length >= this->lbaMain + this->length) // Old contained in new.
		{			
			choiceOldInNew = 1;
			itemHashTable = kmalloc(sizeof(*itemHashTable), GFP_KERNEL);
			itemHashTable->lbaMain = this->lbaMain;
			itemHashTable->lbaAux = this->lbaAux;
			itemHashTable->length = this->length;
			list_del(iter);
			kfree(this);
			break;
		}
		else if (lbaMain < this->lbaMain + this->length && lbaMain + length >= this->lbaMain + this->length) // New to the right.
		{
			this->length = lbaMain - this->lbaMain;
		}
		else if (lbaMain + length > this->lbaMain && this->lbaMain >= lbaMain) // New to the left.
		{
			this->length -= lbaMain + length - this->lbaMain;
			this->lbaMain = lbaMain + length;
			this->lbaAux = this->lbaAux + length;
		}
	}
	
	if (choiceOldInNew)
	{
		hashTableCorrect(hashList, numberOfBucket, lbaMain, lbaAux, itemHashTable->lbaMain - lbaMain);
		hashTableCorrect(hashList, numberOfBucket, itemHashTable->lbaMain + itemHashTable->length, lbaAux, lbaMain + length - itemHashTable->lbaMain - itemHashTable->length);
	}
	else if (choiceNewInOld)
		hashTableInsertSimple(hashList, itemHashTable);
}

void hashTableInsert(struct list_head *hashList, struct dataListHash *new, int onlyCorrect)
{
	// Calculate hash.
	const int numberOfBucket = (new->lbaMain / MAX_RAND) % numberBuckets;
	
	// Correct prev bucket.
	if (numberOfBucket == 0)
		hashTableCorrect(hashList, numberBuckets - 1, new->lbaMain, new->lbaAux, new->length);
	else
		hashTableCorrect(hashList, numberOfBucket - 1, new->lbaMain, new->lbaAux, new->length);
	
	// Correct current bucket.
	hashTableCorrect(hashList, numberOfBucket, new->lbaMain, new->lbaAux, new->length);
	
	// Correct next bucket.
	if (numberOfBucket == numberBuckets - 1)
		hashTableCorrect(hashList, 0, new->lbaMain, new->lbaAux, new->length);
	else
		hashTableCorrect(hashList, numberOfBucket + 1, new->lbaMain, new->lbaAux, new->length);
	
	if (!onlyCorrect)
		list_add(&new->list, &hashList[numberOfBucket]);
}

void hashTablePrint(struct list_head *hashList)
{
	int i = 0;
	struct list_head *iter = NULL;
	struct dataListHash *item = NULL;

	for (i = 0; i < numberBuckets; ++i)
	{		
		list_for_each(iter, &hashList[i]) 
		{
			item = list_entry(iter, struct dataListHash, list);
			printk("%d: %lld %lld\n", i, item->lbaMain, item->length);
		}
	}
}
