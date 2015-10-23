#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 

// Path to input file.
static char* file = NULL; 
module_param(file, charp, 0); 

// Path to output file.
static char* log = NULL; 
module_param(log, charp, 0); 

#define BUFF_LEN 255 
#define DEFNAME "inputFile.txt"; // Default value for input file.
#define DEFLOG "./module.log" // Default value for output file.

struct file *f; 
size_t n;

char buff[BUFF_LEN + 1] = DEFNAME; // Using the default value. 
char buffOutput[BUFF_LEN + 1] = DEFLOG; // Using the default value.

loff_t offset = 0; 
mm_segment_t fs;

static int __init kread_init(void) { 
	if(file != NULL) // If set module parameter "file".
		strcpy(buff, file);

	f = filp_open(buff, O_RDONLY, 0); // Open the file for read.

	if(IS_ERR(f)) { 
		printk("file open failed: %s\n", buff); 
		return -ENOENT; 
	}

	n = kernel_read(f, 0, buff, BUFF_LEN);

	filp_close(f, NULL); // Close the file.

	if(log != NULL) // If set module parameter "log".
		strcpy(buffOutput, log); 

   	f = filp_open(buffOutput, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); // Open the file for write.

	if(IS_ERR(f)) { 
		printk("file open failed: %s\n", buffOutput); 
		return -ENOENT; 
	}
 
	fs = get_fs(); 
	set_fs(get_ds()); // Without it does not work.
	strcpy(buffOutput, buff); // Without it does not work.
	if((n = vfs_write(f, buffOutput, strlen(buffOutput), &offset)) != strlen(buffOutput)) { 
		printk("failed to write: %zu\n", n); 
		return -EIO; 
	} 
	set_fs(fs); // Without it does not work.
	
	filp_close(f, NULL); // Close the file.

	return -EPERM; 
} 

module_init(kread_init);
MODULE_LICENSE("GPL");
