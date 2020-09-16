#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* KERN_INFO */
#include <linux/init.h>		/* Init and exit macros */

static int answer = 42;

static int __init lkp_init(void)
{
    printk(KERN_INFO "Module loaded ...\n");
    printk(KERN_INFO "The answer is %d ...\n", answer);

    /* Return 0 on success, something else on error */
    return 0;
}

static void __exit lkp_exit(void)
{
	printk(KERN_INFO "Module exiting ...\n");
}

module_init(lkp_init);
module_exit(lkp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Changwoo Min <changwoo@vt.edu");
MODULE_DESCRIPTION("Sample kernel module");
