#include <linux/list.h>

typedef uint64_t ElementType;

struct dataList
{
	ElementType value;
	struct list_head list;
};

void printList(struct list_head *iter, struct dataList *itemList, struct list_head *list);

// Initialization of list.
//struct list_head *iter = NULL, *iter_safe;
//struct dataList *itemList;
//LIST_HEAD(list);
	
// Work with list. 
	/*
	// Add "15" before "30" and add "2^30" after "30".
	list_for_each_safe(iter, iter_safe, &list) 
	{
		item = list_entry(iter, struct data, list);
		if (item->value == 30) 
		{	
			item = kmalloc(sizeof(*item), GFP_KERNEL);
			item->value = 0x0000000040000000;
			list_add(&(item->list), iter); // Add 2^30.
			
			item = kmalloc(sizeof(*item), GFP_KERNEL);
			item->value = 15;
			list_add(&(item->list), iter->prev); // Add 15.
		}
	}
	
	// Print list.
	list_for_each(iter, &list) 
	{
		item = list_entry(iter, struct data, list);
		printk("%lld\n", item->value);
	}

	// Delete list.
	list_for_each_safe(iter, iter_safe, &list) 
	{
		item = list_entry(iter, struct data, list);
		list_del(iter);
		kfree(item);
	}
	*/
