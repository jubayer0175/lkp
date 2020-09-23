#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/xarray.h> 
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

/* Methods for linked list*/

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



/**********HASH*****************************/
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
/****************RED BLACK TREE**********************/

static struct rb_root rbtree = RB_ROOT;
struct my_rb_entry {
    int data; 
    struct rb_node mynode;
};

/*rb tree store function*/

static int store_rbTree(int val)
{
     struct my_rb_entry *ptr;
     struct my_rb_entry *temp;
     struct rb_node *root = NULL; 
     struct rb_node **link = &rbtree.rb_node; 

    ptr = kmalloc(sizeof(*ptr), GFP_KERNEL); 
     if(!ptr)
         return -ENOMEM;
    else 
         ptr->data=val;
    while (*link) {
        root = *link;
        temp = rb_entry(root, struct my_rb_entry, mynode); /*root node*/
/*find location*/

        if(ptr->data < temp->data) { 
             link = &root->rb_left;
         } else{
             link = &root->rb_right;
         }
    }
/*insert and balance*/
    rb_link_node(&ptr->mynode, root, link);
    rb_insert_color(&ptr->mynode, &rbtree);
 return 0;  
}


/*Test Red b;ack tree*/

static void test_rbtree(struct seq_file *m){
    struct my_rb_entry *ptr;
    struct rb_node *node;

    seq_printf(m,"Rb tree: ");
    printk(KERN_CONT "Rb tree: ");

    for(node = rb_first(&rbtree); node; node= rb_next(node)) {
        ptr = rb_entry(node, struct my_rb_entry, mynode);
        seq_printf(m, "%d, ", ptr->data);
        printk(KERN_CONT "%d, ", ptr->data);
    }
seq_printf(m,"\n");
printk("\n");

}

/* free rb */

static void destroy_rbtree(void){
    struct my_rb_entry *ptr;
    struct rb_node *node;
   // for(node = rb_first(&rbtree); node; node= rb_next(node)) { /*potential seg fault*/
        node = rb_first(&rbtree);
        while (node) {
        ptr = rb_entry(node, struct my_rb_entry, mynode);
        rb_erase(&ptr->mynode, &rbtree);
        kfree(ptr);
        node = rb_first(&rbtree);
        if (!node)
            break;
    }
}

    /*later to check the empty root*/


/****************************************Radix tree**********************/
static RADIX_TREE(myRDtree, GFP_KERNEL);
static unsigned long  ind = 0;


static int radix_tree_add_(int data){
    int *ptr;
    int err;
    ptr = kmalloc(sizeof(*ptr), GFP_KERNEL);
    if(!ptr){
        return -ENOMEM;
    }else {
        *ptr = data;
    } 
 
    err = radix_tree_insert(&myRDtree, ind, (void* )ptr);
    if (err !=0) 
        return err;
    else
        ind++; /*insertion success*/

return 0;

}

/*test radix tree*/
static void test_radix_tree(struct seq_file *m){
    int *ptr;
    unsigned long i;
    seq_printf(m,"Radix Tree: ");
    printk(KERN_CONT "Radix Tree: ");
  
    for(i=0; i< ind; i++) {
        ptr= (int*) radix_tree_lookup(&myRDtree, i);
        seq_printf(m,"%d, ", *ptr);
        printk(KERN_CONT "%d,  ", *ptr );
    }

    seq_printf(m, "\n");
    printk(KERN_CONT "\n");
}

/*RD destroy*/

static void destroy_radix_tree_and_free(void){



    int *ptr;
    unsigned long i;
    for(i=0; i< ind; i++) {
        ptr= (int*) radix_tree_delete(&myRDtree, i);
        kfree(ptr);
    }
 /*TODO: check for emptiness*/
}

/*Xarray*/

DEFINE_XARRAY(array);

// struct xarray array;
// xa_init(&array);
 unsigned long xarray_ind = 0;

static void store_xarray(int val)
{ 
int *ptr;
ptr  = kmalloc(sizeof(*ptr), GFP_KERNEL);
*ptr = val;

 xa_store(&array, xarray_ind, (void*) ptr, GFP_KERNEL);

 xarray_ind++;
}

/* Test xarray*/

static void test_xarray(struct seq_file *m){
    unsigned long i;
    int *ptr;
    printk(KERN_CONT "Xarray: ");
    seq_printf(m, "Xarray: ");
    for(i=0; i < xarray_ind; i++ ){
        ptr = (int*)xa_load(&array, i);
        printk(KERN_CONT "%d, ", *ptr);
        seq_printf(m, "%d, ", *ptr);
    }
    printk(KERN_CONT "\n");
    seq_printf(m,"\n");

}
/*Destroy the Xarray*/

static void destroy_xarray_and_free(void)
{
    unsigned long i;
    int *ptr;
    for(i=0; i<xarray_ind; i++){
        ptr = (int*) xa_erase(&array, i);
        kfree(ptr);
    }
}



static int store_value(int val)
{   int rb = 0;
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

    rb = store_rbTree(val);

    /* red black tree */
    if (rb != 0)  /*TODO: shorter statement maybe */
        return -ENOMEM;
    
    /*RD tree*/
    rb = radix_tree_add_(val);
   if(rb != 0 )
        return rb;
    store_xarray(val);
 return 0;
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


/********************test and prc functions*******************/

static void run_tests(struct seq_file *m)
{
test_linked_list(m);
test_hash(m);
test_rbtree(m);
test_radix_tree(m);
test_xarray(m);

// More tests

}


static void cleanup(void) {
	printk(KERN_CONT "\nCleaning up...\n");

	destroy_linked_list_and_free();
    destroy_hash_and_free();
    destroy_rbtree();
    destroy_radix_tree_and_free();
    destroy_xarray_and_free();
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
