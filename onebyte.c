#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>

#define MAJOR_NUMBER 61
#define MAX_SIZE 4194304
#define SCULL_IOC_MAGIC 'k'
#define SCULL_IOC_MAXNR 14
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_MSG_WRITE _IOW(SCULL_IOC_MAGIC, 2, int)
#define SCULL_MSG_READ _IOR(SCULL_IOC_MAGIC, 3, int)
#define MAX_DEV_MSG 1024

/* forward declaration */
int onebyte_open(struct inode *inode, struct file *filep);
int onebyte_release(struct inode *inode, struct file *filep);
ssize_t onebyte_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
loff_t onebyte_llseek(struct file *filep, loff_t off, int whence);
long onebyte_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);
static void onebyte_exit(void);

/* definition of file_operation structure */
struct file_operations onebyte_fops = {
	read: onebyte_read,
	write: onebyte_write,
	open: onebyte_open,
	release: onebyte_release,
	llseek: onebyte_llseek,
	unlocked_ioctl: onebyte_ioctl 
};

char *data = NULL;
int size_of_data;
char dev_msg[MAX_DEV_MSG] = {'\0'};
int dev_msg_size = 0;

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
	if (*f_pos >= size_of_data) {
	    return 0;
	}
	if (copy_to_user(buf, data, size_of_data)) {
		return -EFAULT;
	}
	(*f_pos) += size_of_data;
	return size_of_data;
}

ssize_t onebyte_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	size_of_data = count > MAX_SIZE ? MAX_SIZE : count;
	if (copy_from_user(data, buf, size_of_data)) {
		return -EFAULT;
	} 
	//printk(KERN_INFO "procfs_write: write %lu bytes\n", size_of_data);
	return size_of_data;
}

loff_t onebyte_llseek(struct file *filep, loff_t f_pos, int whence)
{
	loff_t new_pos;
	switch(whence) {
		case 0: /* SEEK_SET */
			new_pos = f_pos;
			break;
		case 1: /* SEEK_CUR */
			new_pos = filep->f_pos + f_pos;
			break;
		case 2: /* SEEK_END */
			new_pos = size_of_data + f_pos;
			break;
		default: /* Can't happen */
			return -EINVAL;
	}
	if (new_pos < 0) return -EINVAL;
	filep->f_pos = new_pos;
	return new_pos;
}

long onebyte_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;

	/*
	* extract the type and number bitfields, and don't decode
	* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	*/
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	/*
	* the direction is a bitmask, and VERIFY_WRITE catches R/W
	* transfers. `Type' is user‐oriented, while
	* access_ok is kernel‐oriented, so the concept of "read" and
	* "write" is reversed
	*/
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;
	switch(cmd) {
		case SCULL_HELLO:
			printk(KERN_WARNING "hello\n");
			break;
	    	case SCULL_MSG_WRITE:
	      		printk(KERN_WARNING "MSG WRITE: %d\n", arg);
	      		dev_msg_size = strlen((char __user *) arg);
	      		retval = copy_from_user(dev_msg, (char __user *) arg, dev_msg_size);
	      		break;
	    	case SCULL_MSG_READ:
	      		printk(KERN_WARNING "MSG READ: %d\n", arg);
	      		retval = copy_to_user((char __user *) arg, dev_msg, dev_msg_size);
	      		break;
		default:  /* redundant, as cmd was checked against MAXNR */
			return -ENOTTY;
	}
	return retval;
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
