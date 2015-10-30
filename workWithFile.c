#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "workWithFile.h"

bool isOpenIncorrect(struct file *file, char pathToFile[], mm_segment_t fs) 
{
	if (IS_ERR(file)) 
	{ 
		printk("file open failed: %s\n", pathToFile); 
		set_fs(fs);
		return true; 
	}
	
	return false;
}

bool fileIsEmpty(struct file *file, char pathToFile[], long *length) 
{
	*length = vfs_llseek(file, 0L, 2); // 2 means SEEK_END. (set in end of file)
	if (*length <= 0) 
	{ 
		printk( "failed to lseek %s\n", pathToFile); 
		*length = -EINVAL; 
		return true; 
	}

	return false;
}
