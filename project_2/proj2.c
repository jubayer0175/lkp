#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#include <linux/hashtable.h>


static char *int_str;


MODULE_LICENSE("GPL");
MODULE_AUTHOR("[MD JUBAYER AL MAHMOD]");
MODULE_DESCRIPTION("Project 2");
module_param(int_str, charp, S_IRUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(int_str, "A comma-separated list of integers");

/*---------Linked list init----------*/
static LIST_HEAD(mylist);
struct entry {
	int val;
	struct list_head list;
};

/*----------Hash table--------------*/
struct hash_entry {
    int hash_data;
    struct hlist_node hashlist;

};

DEFINE_HASHTABLE(mytable,3);

/*--------------------*/


static int store_value(int val)
{

    struct entry *value;
    struct hash_entry *hentry;

    value=(struct entry*)kmalloc(sizeof(*value), GFP_KERNEL);
    if (!value)
        return -ENOMEM; /*Failed to allocate memory*/
    else {
    value->val=val; /* assign the int to the node */
    list_add_tail(&value->list, &mylist); /* add the link at the end of the list*/
    }

    /*hash store*/
    hentry= (struct hash_entry*)kmalloc(sizeof(*hentry),GFP_KERNEL);
    if (!hentry)
             return -ENOMEM; /*Failed to allocate memory*/

    hentry->hash_data = val;
    hash_add(mytable, &(hentry->hashlist), hentry->hash_data);

 // Storre more DS

 return 0;





}
static void test_linked_list(struct seq_file *m)
{ 
	struct entry *ptr;
    struct list_head  *head;

    printk("Linked List: ");
    seq_printf(m,"LInked List: ");
     list_for_each(head, &mylist){
         ptr=list_entry(head,struct entry, list); /*get the pointer to the next struct*/
        printk(KERN_CONT "%d, ", ptr->val);
        seq_printf(m,"%d, ",ptr->val);
}

    printk("\n");
    seq_printf(m,"\n");
}






static void destroy_linked_list_and_free(void)
{
    struct list_head *head, *temp;
    struct entry *ptr;

    list_for_each_safe(head,temp, &mylist){ /*store the value in temp just in case*/
        ptr=list_entry(head, struct entry, list);
        list_del(&(ptr->list));
        kfree(ptr); 

    }
    printk(KERN_CONT "\n linked list emptied\n ");
}

static int parse_params(void)
{
	int val, err = 0;
	char *p, *orig, *params;

	params = kstrdup(int_str, GFP_KERNEL);
	if (!params)
		return -ENOMEM;
	orig = params;
	while ((p = strsep(&params, ",")) != NULL) {
		if (!*p)
			continue;
		err = kstrtoint(p, 0, &val);
		if (err)
			break;
		err = store_value(val);
		if (err)
			break;
	}
	kfree(orig);
	return err;
}


/*test hash table*/
static void test_hash(struct seq_file *m) {

struct hash_entry *ptr;
int grp;
printk(KERN_CONT "Hash table: ");
seq_printf(m, "Hash table: ");

    hash_for_each(mytable, grp, ptr, hashlist){
        printk(KERN_CONT "%d, ",ptr->hash_data);
        seq_printf(m,"%d, ", ptr->hash_data);
    }
 printk(KERN_CONT "\n");
 seq_printf(m, "\n");
}

/* Destroy hash*/
static void destroy_hash_and_free(void) {
    int grp;
    struct hash_entry *ptr;
    struct hlist_node *temp;
    hash_for_each_safe(mytable,grp, temp, ptr, hashlist) {
        hash_del(&ptr->hashlist);
        kfree(ptr);
    }
    
    if(hash_empty(mytable) == 1) 
        printk(KERN_CONT "\n Hash emptied\n");
}

static void run_tests(struct seq_file *m)
{
test_linked_list(m);
test_hash(m);
// More tests

}





static void cleanup(void) {
	printk(KERN_CONT "\nCleaning up...\n");

	destroy_linked_list_and_free();
    destroy_hash_and_free();
}


static int proj2_show(struct seq_file *m, void *v) {
    run_tests(m);
        return 0;
}
static int proj2_open(struct inode *inode, struct  file *file) {
      return single_open(file, proj2_show, NULL);
      
}
static struct proc_ops proj2_fops={
        .proc_open = proj2_open,
        .proc_release = single_release,
        .proc_read = seq_read,
        .proc_lseek = seq_lseek,
    
                            
};







static int __init proj2_init(void)
{
	int err = 0;

	if (!int_str) {
		printk(KERN_CONT "Missing \'int_str\' parameter, exiting\n");
		return -1;
	}

	err = parse_params();
	if (err)
		goto out;
    	
    /* call the proc creation that will call run test and print dmesg and proc*/
    proc_create("proj2", 0, NULL, &proj2_fops);

out:
	
	return err;
}
static void __exit proj2_exit(void)
{
    cleanup();
    remove_proc_entry("proj2", NULL);
    

	return;
}
/*The proc file creation*/


module_init(proj2_init);
module_exit(proj2_exit);
