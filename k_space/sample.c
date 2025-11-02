#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
 
/*
** Module Init function
*/
static int __init sample_init(void)
{
    printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;
}
/*
** Module Exit function
*/
static void __exit sample_exit(void)
{
    printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}
 
module_init(sample_init);
module_exit(sample_exit);
 
MODULE_AUTHOR("sri charan");
MODULE_DESCRIPTION("Sample");
MODULE_LICENSE("PUBLIC");
