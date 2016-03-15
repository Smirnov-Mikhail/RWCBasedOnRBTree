#include "main.h"
#include <linux/rbtree.h>	

// Number of bytes for reading.
int numberOfBytes = 10;

// Path to input file.
static char* file = NULL; 
module_param(file, charp, 0); 

// Path to output file.
static char* log = NULL; 
module_param(log, charp, 0); 

#define BUFF_LEN 255
#define DEFNAME "32k.log"; // Default value for input file.
char pathToInputFile[BUFF_LEN + 1] = DEFNAME;
#define DEFLOG "lol.txt";//./module.log" // Default value for output file.
char pathToOutputFile[BUFF_LEN + 1] = DEFLOG;

struct file *fileInput;
struct file *fileOutput;
	
char buff[BUFF_LEN + 1]; // Buffer for reading the memory unit from file.
char strLba[BUFF_LEN + 1]; // Buffer for forming one element of rbtree.
char str[BUFF_LEN + 1]; // Buffer for forming one element of rbtree.

void tostring(char [], long long int);

static int __init kread_init(void) 
{ 
	unsigned int j = 0;
	unsigned int i = 0;
	unsigned int state = 0; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	long long int timeStart = 0;
	long long int timeEnd = 0;
	//long long int result = 0;
	size_t n;

	loff_t offset = 0; // For write.
	loff_t file_offset = 0; // For read.
	mm_segment_t fs;
	long move; // Step at reading.
	
	int endRWC = 0;
	// Initialization of rbTree.
	struct dataRBTree *itemRBTree;
	struct rb_root rbTree = RB_ROOT;
	//struct rb_node *node;

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
	
	move = vfs_llseek(fileInput, numberOfBytes, 0); // 0 means SEEK_SET. (set in begin of file)
	timeStart = ktime_to_ms(ktime_get());
	while (1) 
	{
		n = vfs_read(fileInput, buff, move, &file_offset);
		if (n == 0) // If the file is ended.
		{
			if (strlen(str) > 0) 
			{
				vfs_write(fileOutput, "New line: ", 10, &offset);
				vfs_write(fileOutput, str, strlen(str), &offset);
			}
			break;
		}

		for (j = 0; j < n; j++) 
		{
			if (buff[j] == ' ' || buff[j] == ':')
				continue;
			else if (buff[j] == 'l')
				state = 1;
			else if ((buff[j] == 'b' || buff[j] == 'e') && state == 1)
				state = 2;
			else if ((buff[j] == 'a' || buff[j] == 'n') && state == 2)
				state = 3;
			else if ((state == 3 || state == 4) && buff[j] >= '0' && buff[j] <= '9') // Reads the number.
			{
				state = 4;
				str[i] = buff[j];
				str[i + 1] = '\0';
				i++;
			}
			else if (state == 4 && buff[j] == ',') // Read lba.
			{
				strcpy(strLba, str);
				i = 0;
				strcpy(str, ""); // Clean the string.
			}
			else if (state == 4 && !(buff[j] >= '0' && buff[j] <= '9')) // Read lba and len.
			{
				//vfs_write(fileOutput, "New line: ", 10, &offset);
				//vfs_write(fileOutput, strLba, strlen(strLba), &offset); // Lba.
				//vfs_write(fileOutput, " ", 1, &offset);
				//vfs_write(fileOutput, str, strlen(str), &offset); // Len.
				//vfs_write(fileOutput, "\n", 1, &offset);

				itemRBTree = kmalloc(sizeof(*itemRBTree), GFP_KERNEL);
				kstrtoll(strLba, 10, &itemRBTree->lbaMain);
				itemRBTree->lbaAux = endRWC;
				kstrtoll(str, 10, &itemRBTree->length);
				endRWC += itemRBTree->length;
				
				timeStart = ktime_to_ns(ktime_get());
				rbTreeInsert(&rbTree, itemRBTree);
				timeEnd = ktime_to_ns(ktime_get());
				tostring(str, (timeEnd - timeStart));
				vfs_write(fileOutput, str, strlen(str), &offset);
				vfs_write(fileOutput, "\n", 1, &offset);
				printk("!!!Time: %lld\n", (timeEnd - timeStart));
				
				i = 0;
				strcpy(str, ""); // Clean the string.
				state = 0; // Go to search 'l'.
			}
		}
	}
	set_fs(fs);

	filp_close(fileInput, NULL); // Close the input file.
	filp_close(fileOutput, NULL); // Close the output file.

	/*printk("Root\n");
	for (node = rb_first(&rbTree); node; node = rb_next(node))
		printk("lbaMain=%lld lbaAux=%lld length=%lld\n", 
			rb_entry(node, struct dataRBTree, node)->lbaMain, 
			rb_entry(node, struct dataRBTree, node)->lbaAux, 
			rb_entry(node, struct dataRBTree, node)->length);*/
	
	return 0; 
}

void tostring(char str[], long long int num)
{
    int i, len = 0;
	long long int rem, n;
	
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

static void __exit kread_exit(void)
{
	printk("module is off\n");
}
