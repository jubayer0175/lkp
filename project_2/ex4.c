#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>

static char *int_str;

/* [X1: point 1]
 * General information that is displayed when modinfo is used. The GPL license entry in the information macro defines the usuage and support criteria for this free license. Other two macros state the author name and a short descriotion of the module.
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("[MD JUBAYER AL MAHMOD]");
MODULE_DESCRIPTION("LKP Exercise 4");

/* [X2: point 1]
 * prtotype the parameters. 
 * int_str is the string that is going to hold the string supplied from the command terrminal.
 * - charp defines its type as  character pointer.
 * - S_IRGRP: Read permission to group user who own the file
 * - S_IRUSR: Read permissiion for the owner of the file
 * - S_IROTH: Gives read permission bit for outher user is set
 */
module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);

/* [X3: point 1]
 * This is a simple descritption of the kernel module
 */
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/* [X4: point 1]
 * Create and initialize a list head struct for 'mylist'. This list points to itself for now. 
 */
static LIST_HEAD(mylist);

/* [X5: point 1]
 * Create a node and make it part of a link list. The 'val' holds the incoming integer. list_head basically links this node to previous and next nodes with *prev and *next pointers defines in /list.h.
 *
 */
struct entry {
	int val;
	struct list_head list;
};

static int store_value(int val)
{
	/* [X6: point 10]
	 * Allocate a struct entry of which val is val
	 * and add it to the tail of mylist.
	 * Return 0 if everything is successful.
	 * Otherwise (e.g., memory allocation failure),
	 * return corresponding error code in error.h (e.g., -ENOMEM).
	 */

    struct entry *value;
    value=(struct entry*)kmalloc(sizeof(*value), GFP_KERNEL);
    if (!value)
        return -ENOMEM; /*Failed to allocate memory*/
    else {
    value->val=val; /* assign the int to the node */
    list_add_tail(&value->list, &mylist); /* add the link at the end of the list*/
    }
 return 0;

}





static void test_linked_list(void)
{ 
	struct entry *ptr;
    struct list_head  *head;

    printk("Test running");
	/* [X7: point 10]
     * Print out value of all entries in mylist
     * */

   /* list_for_each_entry(ptr,&mylist,list) */ /* why segfault!*/
     list_for_each(head, &mylist){
         ptr=list_entry(head,struct entry, list); /*get the pointer to the next struct*/
        printk(KERN_INFO "%d", ptr->val); 
}

}




static void destroy_linked_list_and_free(void)
{
	/* [X8: point 10]
	 * Free all entries in mylist.
	 */
    struct list_head *head, *temp;
    struct entry *ptr;

    list_for_each_safe(head,temp, &mylist){ /*store the value in temp just in case*/
        ptr=list_entry(head, struct entry, list);
        list_del(&(ptr->list));
        kfree(ptr); 

    }
    printk("All entries deleted");
}


static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;


	/* [X9: point 1]
	 * kstrdup allocates memory and uplicates the string pointed by int_str. !param checks if memory allocation is successful. orig stores the same pointer as stored by the params.
	 */
	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;

	/* [X10: point 1]
	 * This is the ultimate block that seperates comma-seperated integer to individual integers. strsep() looks for ',' and replaces it with \0. Now we have individul string each with an integer in it. Note, NULL check is done to make sure that there is a  valid string.
	 */
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		/* [X11: point 1]
		 * convert string (pointed by p) to int and store it in a designated address (&val). The seconf argument indiactes the unsigned integer base. 0 Means autodetct - default is decimal. Successful conversion returns 0 else break the loop (err != 0)
		 */
		err = kstrtoint(p, 0, &val);
		if (err)
			break;

		/* [X12: point 1]
		 * store_value() stores the value in the entry struct and attatch is to the linked list. Failure to allocate memory for the new enrty whould return error (ENOMEM).
		 */
		err = store_value(val);
		if (err)
			break;
	}

	/* [X13: point 1]
	 * Free the memory allocated for params. Once done, we return the error return of parse_param.
	 */
	kfree(orig);
	return err;
}



static void run_tests(void)
{
	/* [X14: point 1]
	 * call the function test_linked_list() to print all the integers. 
	 */
test_linked_list();
}


static void cleanup(void) {
	/* [X15: point 1]
	 * deallocate all the memory by callying the destry_linked_list_and_free() function
	 */
	printk(KERN_INFO "\nCleaning up...\n");

	destroy_linked_list_and_free();
}



static int __init ex4_init(void)
{
	int err = 0;

	/* [X16: point 1]
	 * if inst_str is null then no need to do anything other than printing it as  error.
	 */
	if (!int_str) {
		printk(KERN_INFO "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	/* [X17: point 1]
	 * call parse_param() to parse int_str and bulid a linked list. Unsuccessful attempt would generate and error and it exists. 
	 */
	err = parse_params();
	if (err)
		goto out;

	/* [X18: point 1]
	 * Print all the values by calling run_test()
	 */
	run_tests();
out:
	/* [X19: point 1]
	 * either you had error or linked list buliding and printing was successfull clean up all the memory and return. 
	 */
	cleanup();
	return err;
}

static void __exit ex4_exit(void)
{
	/* [X20: point 1]
	 * Just a dummy function that would be called at when module is unloaded from the kernel.
	 */
	return;
}

/* [X21: point 1]
 * execute ex4_init while module is loaded
 */
module_init(ex4_init);

/* [X22: point 1]
 * Execute ex4_exit when unloading the module.
 */
module_exit(ex4_exit);
