#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>

// Check that the file is opened correctly.
bool isOpenIncorrect(struct file *file, char pathToFile[], mm_segment_t fs);

// Check that the file is not empty. It is not used now, but may be useful in future.
bool fileIsEmpty(struct file *file, char pathToFile[], long *length);

//vfs_write(fileOutput, "New line: ", 10, &offset);
//vfs_write(fileOutput, strLba, strlen(strLba), &offset); // Lba.
//vfs_write(fileOutput, " ", 1, &offset);
//vfs_write(fileOutput, str, strlen(str), &offset); // Len.
//vfs_write(fileOutput, "\n", 1, &offset);
