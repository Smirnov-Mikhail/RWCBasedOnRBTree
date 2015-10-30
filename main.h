#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h>
#include <linux/list.h>
#include <linux/slab.h>
#include "workWithFile.h"

MODULE_LICENSE("GPL");

static int __init kread_init(void);
static void __exit kread_exit(void);

module_init(kread_init);
module_exit(kread_exit);
