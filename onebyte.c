#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61
#define MAX_SIZE 4194304

/* forward declaration */
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	read: onebyte_read,
	write: onebyte_write,
	open: onebyte_open,
	release: onebyte_release
};

char *data = NULL;
int size_of_data;

int onebyte_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int onebyte_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
	static int finished = 0;
	if (finished) {
		finished = 0;
		return 0;
	}
	finished = 1;

	if (copy_to_user(buf, data, size_of_data)) {
		return -EFAULT;
	}
	return size_of_data;
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	if (count > MAX_SIZE) {
		size_of_data = MAX_SIZE;
	} else {
		size_of_data = count;
	}
	if (copy_from_user(data, buf, size_of_data)) {
		return -EFAULT;
	} 
	printk(KERN_INFO "procfs_write: write %lu bytes\n", size_of_data);
	return size_of_data;
}

static int onebyte_init(void)
{
	int result;
	// register the device
	result = register_chrdev(MAJOR_NUMBER, "onebyte", &onebyte_fops);

	if (result < 0) {
		return result;
	}

	// allocate one byte of memory for storage
	// kmalloc is just like malloc, the second parameter is
	// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.

	data = kmalloc(MAX_SIZE, GFP_KERNEL);

	if (!data) {
		onebyte_exit();
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}

	// initialize the value to be X
	data[0] = '\0';
	printk(KERN_ALERT "This is a onebyte device module\n");
	return 0;

}

static void onebyte_exit(void)
{
	// if the pointer is pointing to something
	if (data) {
		// free the memory and assign the pointer to NULL
		kfree(data);
		data = NULL;
	}

	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "onebyte");
	printk(KERN_ALERT "Onebyte device module is unloaded\n");

}

MODULE_LICENSE("GPL");
module_init(onebyte_init);
module_exit(onebyte_exit);
