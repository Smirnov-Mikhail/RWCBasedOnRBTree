#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h> 

// Number of bits for reading.
int numberOfBits = 10;

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

static int __init kread_init(void) {
	int j = 0;
	int i = 0;
	size_t n;

	loff_t offset = 0; // For write.
	loff_t file_offset = 0; // For read.
	mm_segment_t fs;
	long move; // Step at reading.

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
	
	move = vfs_llseek(fileInput, numberOfBits, 0); // 0 means SEEK_SET. (set in begin of file
	while (1) {
		n = vfs_read(fileInput, buff, move, &file_offset);
		if (n == 0) { // If the file is ended.
			if (strlen(str) > 0) {
				printk("ALO %s\n", str);
				vfs_write(fileOutput, "New line: ", 10, &offset);
				vfs_write(fileOutput, str, strlen(str), &offset);
			}
			break;
		}

		for (j = 0; j < n; j++) {
			if (buff[j] == '\n') {
				printk("buff %s\n", buff);
				printk("lol %s\n", str);
				vfs_write(fileOutput, "New line: ", 10, &offset);
				vfs_write(fileOutput, str, strlen(str), &offset); // Main string.
				vfs_write(fileOutput, "\n", 1, &offset);
				i = 0;
				strcpy(str, ""); // Clean the string.
			}
			else {		
				str[i] = buff[j];
				str[i + 1] = '\0';
				i++;
			}
		}
		printk("APY %s\n", str);
	}
	set_fs(fs);

	filp_close(fileInput, NULL); // Close the input file.
	filp_close(fileOutput, NULL); // Close the output file.

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
