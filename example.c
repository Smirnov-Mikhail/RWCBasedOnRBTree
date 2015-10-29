#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>

// Number of bytes for reading.
int numberOfBytes = 10;

// Path to input file.
static char* file = NULL; 
module_param(file, charp, 0); 

// Path to output file.
static char* log = NULL; 
module_param(log, charp, 0); 

#define BUFF_LEN 255 
#define DEFNAME "inputFile.txt"; // Default value for input file.
char pathToInputFile[BUFF_LEN + 1] = DEFNAME;
#define DEFLOG "./module.log" // Default value for output file.
char pathToOutputFile[BUFF_LEN + 1] = DEFLOG;

struct file *fileInput;
struct file *fileOutput; 

char buff[BUFF_LEN + 1]; // Buffer for reading the memory unit from file.
char str[BUFF_LEN + 1]; // Buffer for forming one line.

// Check that the file is opened incorrectly.
bool isOpenIncorrect(struct file *file, char pathToFile[], mm_segment_t fs);

// Check that the file is empty.
bool fileIsEmpty(struct file *file, char pathToFile[], long *length);

struct data {
	char value[BUFF_LEN + 1];
	struct list_head list;
};

static int __init kread_init(void) {
	unsigned int j = 0;
	unsigned int i = 0;
	size_t n;

	loff_t offset = 0; // For write.
	loff_t file_offset = 0; // For read.
	mm_segment_t fs;
	long move; // Step at reading.
	
	struct list_head *iter, *iter_safe;
	struct data *item;
	LIST_HEAD(list);

	fs = get_fs(); 
	set_fs(get_ds()); 

	if (file != NULL) // If set module parameter "file".
		strcpy(pathToInputFile, file);

	fileInput = filp_open(pathToInputFile, O_RDONLY, 0); // Open the file for read.
	
	if (isOpenIncorrect(fileInput, pathToInputFile, fs))
		return -ENOENT;

	if (log != NULL) // If set module parameter "log".
		strcpy(pathToOutputFile, log); 

   	fileOutput = filp_open(pathToOutputFile, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); // Open the file for write.
	
	if (isOpenIncorrect(fileOutput, pathToOutputFile, fs))
		return -ENOENT;
	
	move = vfs_llseek(fileInput, numberOfBytes, 0); // 0 means SEEK_SET. (set in begin of file
	while (1) {
		n = vfs_read(fileInput, buff, move, &file_offset);
		if (n == 0) { // If the file is ended.
			if (strlen(str) > 0) {
				vfs_write(fileOutput, "New line: ", 10, &offset);
				vfs_write(fileOutput, str, strlen(str), &offset);
			}
			break;
		}

		for (j = 0; j < n; j++) {
			if (buff[j] == '\n') {
				vfs_write(fileOutput, "New line: ", 10, &offset);
				vfs_write(fileOutput, str, strlen(str), &offset); // Main string.
				vfs_write(fileOutput, "\n", 1, &offset);

				item = kmalloc(sizeof(*item), GFP_KERNEL);
				strcpy(item->value, str);
				list_add(&(item->list), &list); // Add to list.
				
				i = 0;
				strcpy(str, ""); // Clean the string.
			}
			else {		
				str[i] = buff[j];
				str[i + 1] = '\0';
				i++;
			}
		}
	}
	set_fs(fs);

	filp_close(fileInput, NULL); // Close the input file.
	filp_close(fileOutput, NULL); // Close the output file.

	// Delete "20".
	list_for_each_safe(iter, iter_safe, &list) {
		item = list_entry(iter, struct data, list);
		if (strcmp(item->value, "20") == 0) {
			list_del(iter);
			kfree(item);
		}
	}
	
	// Print list.
	list_for_each(iter, &list) {
		item = list_entry(iter, struct data, list);
		printk("%s ", item->value);
	}

	// Delete list.
	list_for_each_safe(iter, iter_safe, &list) {
		item = list_entry(iter, struct data, list);
		list_del(iter);
		kfree(item);
	}

	return -EPERM; 
}

bool isOpenIncorrect(struct file *file, char pathToFile[], mm_segment_t fs) {
	if (IS_ERR(file)) { // Check that the input file is opened correctly.
		printk("file open failed: %s\n", pathToFile); 
		set_fs(fs);
		return true; 
	}
	
	return false;
}

bool fileIsEmpty(struct file *file, char pathToFile[], long *length) {
	*length = vfs_llseek(fileInput, 0L, 2); // 2 means SEEK_END. (set in end of file)
	if (*length <= 0) { // Check that the input file is not empty.
		printk( "failed to lseek %s\n", pathToInputFile); 
		*length = -EINVAL; 
		return true; 
	}

	return false;
}

module_init(kread_init);
MODULE_LICENSE("GPL");
