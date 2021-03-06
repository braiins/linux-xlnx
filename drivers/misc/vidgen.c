#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <asm/io.h>

#include "vidgen.h"

#define VIDGEN_CTRL_REG_0		0x43C50000


static dev_t dev = 0;
static struct cdev etx_cdev;
static struct class *dev_class;
static void *mmio_0;
static struct kobject *kobj_ref;


static void set_vidgen_value_0(uint32_t value)
{
	*(unsigned int *)(mmio_0) = value;
}

static unsigned int get_vidgen_value_0(void)
{
	return *(unsigned int *)(mmio_0);
}

static ssize_t vidgen_read(struct file *file, char __user *buffer,
		size_t length, loff_t *offset)
{
	return 0;
}

static ssize_t vidgen_write(struct file *file, const char __user *buffer,
		size_t length, loff_t *offset)
{
	return 0;
}

static long vidgen_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	/*
	 * Switch according to the ioctl called
	 */
	switch (cmd)
	{
        case IOCTL_SET_VALUE_0:
			set_vidgen_value_0(arg);
        break;
	}
	return 0;
}

static int vidgen_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int vidgen_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations fops = {
	.owner			= THIS_MODULE,
	.read			= vidgen_read,
	.write 			= vidgen_write,
	.unlocked_ioctl	= vidgen_ioctl,
	.open			= vidgen_open,
	.release		= vidgen_release,
};
 
static ssize_t vidgen_sysfs_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%d", get_vidgen_value_0());
}

static ssize_t vidgen_sysfs_store(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf, size_t count)
{
	unsigned int value = 0;

	if (sscanf(buf, "%d", &value) != 1)
		return -EINVAL;

	set_vidgen_value_0(value);
	return count;
}

struct kobj_attribute vidgen_attr =
		__ATTR(0.0, 0660, vidgen_sysfs_show, vidgen_sysfs_store);

static int __init vidgen_init(void)
{
	/* Allocating Major number */
	if ((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME_0)) <0) {
		printk(KERN_ERR "Cannot allocate major number\n");
		return -1;
	}

	printk(KERN_INFO "%s (Major = %d Minor = %d)\n", DEVICE_NAME_0, MAJOR(dev), MINOR(dev));

	/* Creating cdev structure */
	cdev_init(&etx_cdev, &fops);
	etx_cdev.owner = THIS_MODULE;
	etx_cdev.ops = &fops;

	/* Adding character device to the system */
	if ((cdev_add(&etx_cdev, dev, 1)) < 0) {
		printk(KERN_ERR "Cannot add the device to the system\n");
		goto r_cdev;
	}

	/* Creating struct class */
	dev_class = class_create(THIS_MODULE, DEVICE_NAME_0);
	if (dev_class == NULL) {
		printk(KERN_ERR "Cannot create the struct class\n");
		goto r_class;
	}

	/* Creating device */
	if (device_create(dev_class, NULL, dev, NULL, DEVICE_NAME_0) == NULL) {
		printk(KERN_ERR "Cannot create the device %s\n", DEVICE_NAME_0);
		goto r_device;
	}

	/* Map vidgen to virtual memory */
	mmio_0 = ioremap(VIDGEN_CTRL_REG_0, 0x100);
	if (!mmio_0) {
		printk(KERN_ERR "Unable to remap IO region for %s\n", DEVICE_NAME_0);
		goto r_ioremap;
	}

	/* Creating a directory in /sys/kernel/ */
	kobj_ref = kobject_create_and_add(SYSDIR_NAME, power_kobj);
	if (!kobj_ref) {
		printk(KERN_ERR "Cannot create the %s in /sys/power\n", SYSDIR_NAME);
		goto r_kobj;
	}

	/* Creating sysfs file for etx_value */
	if (sysfs_create_file(kobj_ref, &vidgen_attr.attr)) {
		printk(KERN_ERR "Cannot create sysfs file in /sys/power/%s\n", SYSDIR_NAME);
		goto r_sysfs;
	}

	return 0;

// 	sysfs_remove_file(power_kobj, &vidgen_attr.attr);
r_sysfs:
	kobject_put(kobj_ref);
r_kobj:
	iounmap(mmio_0);
r_ioremap:
	device_destroy(dev_class, dev);
r_device:
	class_destroy(dev_class);
r_class:
	cdev_del(&etx_cdev);
r_cdev:
	unregister_chrdev_region(dev, 1);
	return -1;
}

static void __exit vidgen_exit(void)
{
	sysfs_remove_file(power_kobj, &vidgen_attr.attr);
	kobject_put(kobj_ref);
	iounmap(mmio_0);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&etx_cdev);
	unregister_chrdev_region(dev, 1);
}

module_init(vidgen_init);
module_exit(vidgen_exit);

/* Standard module information, edit as appropriate */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Xilinx Inc.");
MODULE_DESCRIPTION("vidgen - loadable module template generated by petalinux-create -t modules");
