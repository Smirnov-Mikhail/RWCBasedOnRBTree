#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h> 

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
size_t n;

char buff[BUFF_LEN + 1]; // Using the default value.
char str[BUFF_LEN + 1];
char buffOutput[BUFF_LEN + 1]; // Using the default value.
char strNumber[11] = "New line: ";

loff_t offset = 0; // For write.
loff_t file_offset = 0; // For read.
mm_segment_t fs;
size_t n; 
long move;
long length;

int i = 0;
int cur = 0;
int numberOfStr = 0;

// Check that the file is opened incorrectly.
bool isOpenIncorrect(struct file *file, char pathToFile[]);

// Check that the file is empty.
bool fileIsEmpty(struct file *file, char pathToFile[]);

static int __init kread_init(void) { 
	fs = get_fs(); 
	set_fs(get_ds()); 

	if (file != NULL) // If set module parameter "file".
		strcpy(pathToInputFile, file);

	fileInput = filp_open(pathToInputFile, O_RDONLY, 0); // Open the file for read.
	
	if (isOpenIncorrect(fileInput, pathToInputFile))
		return -ENOENT;

	if (fileIsEmpty(fileInput, pathToInputFile))
		return (int)length; 

	printk("file size = %d bytes\n", (int)length);

	if (log != NULL) // If set module parameter "log".
		strcpy(pathToOutputFile, log); 

   	fileOutput = filp_open(pathToOutputFile, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR); // Open the file for write.
	
	if (isOpenIncorrect(fileOutput, pathToOutputFile))
		return -ENOENT;

	move = vfs_llseek(fileInput, 1L, 0); // 0 means SEEK_SET. (set in begin of file)

	for (cur = 0; cur < length; cur++) {
		vfs_read(fileInput, buff, move, &file_offset);
		if (strcmp(buff, "\n") == 0) {
			vfs_write(fileOutput, strNumber, strlen(strNumber), &offset);
			vfs_write(fileOutput, str, strlen(str), &offset); // Main string.
			vfs_write(fileOutput, "\n", 1, &offset);
			i = 0;
			strcpy(str, ""); // Clean the string.
			numberOfStr++;
		}
		else {
			printk("%s", buff);	
			i++;
			strcat(str, buff);
		}
	}
	set_fs(fs);

	filp_close(fileInput, NULL); // Close the input file.
	filp_close(fileOutput, NULL); // Close the output file.

	return -EPERM; 
}

bool isOpenIncorrect(struct file *file, char pathToFile[]) {
	if (IS_ERR(file)) { // Check that the input file is opened correctly.
		printk("file open failed: %s\n", pathToFile); 
		set_fs(fs);
		return true; 
	}
	
	return false;
}

bool fileIsEmpty(struct file *file, char pathToFile[]) {
	length = vfs_llseek(fileInput, 0L, 2); // 2 means SEEK_END. (set in end of file)
	if (length <= 0) { // Check that the input file is not empty.
		printk( "failed to lseek %s\n", pathToInputFile); 
		length = -EINVAL; 
		return true; 
	}

	return false;
}

module_init(kread_init);
MODULE_LICENSE("GPL");
