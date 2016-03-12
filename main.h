#include <linux/module.h>
#include <linux/kernel.h> // For kstrtoll.
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/uaccess.h> 
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/timekeeping.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include "workWithFile.h"
#include <linux/ktime.h>
#include "rbTree.h"
#include "list.h"

MODULE_LICENSE("GPL");

static int __init kread_init(void);
static void __exit kread_exit(void);

module_init(kread_init);
module_exit(kread_exit);
