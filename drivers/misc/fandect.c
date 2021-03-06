/*  blink.c - The simplest kernel module.
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

#include <linux/fs.h>

#include <linux/slab.h>
#include <linux/io.h>
#include <linux/interrupt.h>

#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include <asm/uaccess.h> /* for get_user and put_user */
#include <asm/io.h>
#include "fandect.h"
#define SUCCESS 0
#define DEVICE_NAME_0 "fandect0.0"
#define DEVICE_NAME_1 "fandect1.0"
#define DEVICE_NAME_2 "fandect2.0"
#define DEVICE_NAME_3 "fandect3.0"
#define DEVICE_NAME_4 "fandect4.0"
#define DEVICE_NAME_5 "fandect5.0"

#define FANDECT_CTRL_REG_0 0x43C00000
#define FANDECT_CTRL_REG_1 0x43C10000
#define FANDECT_CTRL_REG_2 0x43C70000
#define FANDECT_CTRL_REG_3 0x43C80000
#define FANDECT_CTRL_REG_4 0x43C90000
#define FANDECT_CTRL_REG_5 0x43Ca0000

static void *mmio_0;
static void *mmio_1;
static void *mmio_2;
static void *mmio_3;
static void *mmio_4;
static void *mmio_5;
static int major_num_0;
static int major_num_1;
static int major_num_2;
static int major_num_3;
static int major_num_4;
static int major_num_5;
static struct class *dev_class_0;
static struct class *dev_class_1;
static struct class *dev_class_2;
static struct class *dev_class_3;
static struct class *dev_class_4;
static struct class *dev_class_5;
static struct device *dev_device_0;
static struct device *dev_device_1;
static struct device *dev_device_2;
static struct device *dev_device_3;
static struct device *dev_device_4;
static struct device *dev_device_5;

/*
* Is the device open right now? Used to prevent
* concurent access into the same device
*/
static int Device_Open = 0;


//void get_pwmgen_freq_0(uint32_t *param)
//{
//        printk("KERNEL PRINT : get_pwmgen_freq at:%x \n\r", ((unsigned int *)mmio_0));
//	*param = *(unsigned int *)(mmio_0);
//
//}
//
//void get_pwmgen_freq_1(uint32_t *param)
//{
//        printk("KERNEL PRINT : get_pwmgen_freq at:%x \n\r", ((unsigned int *)mmio_1));
//	*param = *(unsigned int *)(mmio_1);
//}

/*
* This is called whenever a process attempts to open the device file
*/
static int device_open(struct inode *inode, struct file *file)
{
	#ifdef DEBUG
		//printk(KERN_INFO "device_open(%p)\n", file);
	#endif
	/*
	* We don't want to talk to two processes at the same time
	*/
	if (Device_Open)
		return -EBUSY;
	Device_Open++;
	/*
	* Initialize the message
	*/
//	Message_Ptr = Message;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}
static int device_release(struct inode *inode, struct file *file)
{
	#ifdef DEBUG
		//printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
	#endif
	/*
	* We're now ready for our next caller
	*/
	Device_Open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}
/*
* This function is called whenever a process which has already opened the
* device file attempts to read from it.
*/
static ssize_t device_read(	struct file *file, /* see include/linux/fs.h */
							char __user * buffer, /* buffer to be filled with data */
							size_t length, /* length of the buffer */
							loff_t * offset)
{

	char freq[24];

    if (length == 0 || *offset != 0)
        return 0;

	freq[0]  = *(unsigned char *)(mmio_0 + 0);
	freq[1]  = *(unsigned char *)(mmio_0 + 1);
	freq[2]  = *(unsigned char *)(mmio_0 + 2);
	freq[3]  = *(unsigned char *)(mmio_0 + 3);
	freq[4]  = *(unsigned char *)(mmio_1 + 0);
	freq[5]  = *(unsigned char *)(mmio_1 + 1);
	freq[6]  = *(unsigned char *)(mmio_1 + 2);
	freq[7]  = *(unsigned char *)(mmio_1 + 3);
	freq[8]  = *(unsigned char *)(mmio_2 + 0);
	freq[9]  = *(unsigned char *)(mmio_2 + 1);
	freq[10] = *(unsigned char *)(mmio_2 + 2);
	freq[11] = *(unsigned char *)(mmio_2 + 3);
	freq[12] = *(unsigned char *)(mmio_3 + 0);
	freq[13] = *(unsigned char *)(mmio_3 + 1);
	freq[14] = *(unsigned char *)(mmio_3 + 2);
	freq[15] = *(unsigned char *)(mmio_3 + 3);
	freq[16] = *(unsigned char *)(mmio_4 + 0);
	freq[17] = *(unsigned char *)(mmio_4 + 1);
	freq[18] = *(unsigned char *)(mmio_4 + 2);
	freq[19] = *(unsigned char *)(mmio_4 + 3);
	freq[20] = *(unsigned char *)(mmio_5 + 0);
	freq[21] = *(unsigned char *)(mmio_5 + 1);
	freq[22] = *(unsigned char *)(mmio_5 + 2);
	freq[23] = *(unsigned char *)(mmio_5 + 3);

    if (copy_to_user(buffer, freq, length))
        return -EFAULT;

    *offset += length;

    return length;
}
/*
* This function is called when somebody tries to
* write into our device file.
*/
static ssize_t device_write(struct file *file,
							const char __user * buffer, 
							size_t length, 
							loff_t * offset)
{
	return SUCCESS;
}
/*
* This function is called whenever a process tries to do an ioctl on our
* device file. We get two extra parameters (additional to the inode and file
* structures, which all device functions get): the number of the ioctl called
* and the parameter given to the ioctl function.
*
* If the ioctl is write or read/write (meaning output is returned to the
* calling process), the ioctl call returns the output of this function.
*
*/
int fandect_ioctl(		struct file *file, /* ditto */
					unsigned int ioctl_num, /* number and param for ioctl */
					unsigned long ioctl_param)
{
//	uint32_t i;
//	char *temp;
//	char ch;
	/*
	* Switch according to the ioctl called
	*/
	switch (ioctl_num) 
	{
//        	case IOCTL_GET_FREQ_0:
//        	        get_pwmgen_freq_0((uint32_t *)ioctl_param);
//		break;
//        	case IOCTL_GET_FREQ_1:
//        	        get_pwmgen_freq_1((uint32_t *)ioctl_param);
//        	break;	
	}
	return SUCCESS;
}
/* Module Declarations */
/*
* This structure will hold the functions to be called
* when a process does something to the device we
* created. Since a pointer to this structure is kept in
* the devices table, it can't be local to
* init_module. NULL is for unimplemented functions.
*/
struct file_operations Fops_fandect = {
	.owner = THIS_MODULE,       
	.read = device_read,
	.write = device_write,
	.unlocked_ioctl = fandect_ioctl,
	.open = device_open,
	.release = device_release, /*close */								
};



/* Standard module information, edit as appropriate */
MODULE_LICENSE("GPL");
MODULE_AUTHOR
    ("Xilinx Inc.");
MODULE_DESCRIPTION
    ("fandect - loadable module template generated by petalinux-create -t modules");

#define DRIVER_NAME "fandect"
#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0, char *)
#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, char *)


/* Simple example of how to receive command line parameters to your module.
   Delete if you don't need them */
static unsigned myint = 0xdeadbeef;
static char *mystr = "default";

module_param(myint, int, S_IRUGO);
module_param(mystr, charp, S_IRUGO);

struct fandect_local {
	int irq;
	unsigned long mem_start;
	unsigned long mem_end;
	void __iomem *base_addr;
};

static irqreturn_t fandect_irq(int irq, void *lp)
{
	printk("fandect interrupt\n");
	return IRQ_HANDLED;
}

static int fandect_probe(struct platform_device *pdev)
{
	struct resource *r_irq; /* Interrupt resources */
	struct resource *r_mem; /* IO mem resources */
	struct device *dev = &pdev->dev;
	struct fandect_local *lp = NULL;

	int rc = 0;
	
	dev_info(dev, "Device Tree Probing\n");

	/* Get iospace for the device */
	r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r_mem) {
		dev_err(dev, "invalid address\n");
		return -ENODEV;
	}
	
	lp = (struct fandect_local *) kmalloc(sizeof(struct fandect_local), GFP_KERNEL);
	if (!lp) {
		dev_err(dev, "Cound not allocate fandect device\n");
		return -ENOMEM;
	}
	
	dev_set_drvdata(dev, lp);
	
	lp->mem_start = r_mem->start;
	lp->mem_end = r_mem->end;

	if (!request_mem_region(lp->mem_start,
				lp->mem_end - lp->mem_start + 1,
				DRIVER_NAME)) {
		dev_err(dev, "Couldn't lock memory region at %p\n",
			(void *)lp->mem_start);
		rc = -EBUSY;
		goto error1;
	}

	lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	if (!lp->base_addr) {
		dev_err(dev, "fandect: Could not allocate iomem\n");
		rc = -EIO;
		goto error2;
	}

	/* Get IRQ for the device */
	r_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!r_irq) {
		dev_info(dev, "no IRQ found\n");
		dev_info(dev, "fandect at 0x%08x mapped to 0x%08x\n",
			(unsigned int __force)lp->mem_start,
			(unsigned int __force)lp->base_addr);
		return 0;
	}
	lp->irq = r_irq->start;
	
	rc = request_irq(lp->irq, &fandect_irq, 0, DRIVER_NAME, lp);
	if (rc) {
		dev_err(dev, "testmodule: Could not allocate interrupt %d.\n",
			lp->irq);
		goto error3;
	}

	dev_info(dev,"fandect at 0x%08x mapped to 0x%08x, irq=%d\n",
		(unsigned int __force)lp->mem_start,
		(unsigned int __force)lp->base_addr,
		lp->irq);
	return 0;
error3:
	free_irq(lp->irq, lp);
error2:
	release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
error1:
	kfree(lp);
	dev_set_drvdata(dev, NULL);
	return rc;
}

static int fandect_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct fandect_local *lp = dev_get_drvdata(dev);
	free_irq(lp->irq, lp);
	release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
	kfree(lp);
	dev_set_drvdata(dev, NULL);
	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id fandect_of_match[] = {
	{ .compatible = "vendor,fandect", },
	{ /* end of list */ },
};
MODULE_DEVICE_TABLE(of, fandect_of_match);
#else
# define fandect_of_match
#endif


static struct platform_driver fandect_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
		.of_match_table	= fandect_of_match,
	},
	.probe		= fandect_probe,
	.remove		= fandect_remove,
};

static int __init fandect_init(void)
{
	//printk("<1>Hello module world.\n");
	//printk("<1>Module parameters were (0x%08x) and \"%s\"\n", myint,
	//       mystr);

	/*
	* Register the character device (atleast try)
	*/
	major_num_0 = register_chrdev(0,DEVICE_NAME_0, &Fops_fandect);
	if (major_num_0 < 0) 
	{
		printk(KERN_ALERT "%s failed with \n","Sorry, registering the character device ");
	}
	dev_class_0 = class_create(THIS_MODULE, DEVICE_NAME_0);

	dev_device_0 = device_create(dev_class_0, NULL, MKDEV(major_num_0, 0), NULL, DEVICE_NAME_0);
	
	mmio_0 = ioremap(FANDECT_CTRL_REG_0,0x100);
        
    //printk(KERN_INFO "%s the major device number is %d.\n","Registeration is a success", major_num_0);
	//printk(KERN_INFO "If you want to talk to the device driver,\n");
	//printk(KERN_INFO "create a device file by following command. \n \n");
	//printk(KERN_INFO "mknod %s c %d 0\n\n", DEVICE_NAME_0, major_num_0);
	//printk(KERN_INFO "The device file name is important, because\n");
	//printk(KERN_INFO "the ioctl program assumes that's the file you'll use\n");

	/*
	* Register the character device (atleast try)
	*/
	major_num_1 = register_chrdev(0,DEVICE_NAME_1, &Fops_fandect);

	/*
	* Negative values signify an error
	*/
	if (major_num_1 < 0) 
	{
		printk(KERN_ALERT "%s failed with \n","Sorry, registering the character device ");
	}
	dev_class_1 = class_create(THIS_MODULE, DEVICE_NAME_1);

	dev_device_1 = device_create(dev_class_1, NULL, MKDEV(major_num_1, 0), NULL, DEVICE_NAME_1);
	
	mmio_1 = ioremap(FANDECT_CTRL_REG_1,0x100);

	/*
	* Register the character device (atleast try)
	*/
	major_num_2 = register_chrdev(0,DEVICE_NAME_2, &Fops_fandect);

	/*
	* Negative values signify an error
	*/
	if (major_num_2 < 0) 
	{
		printk(KERN_ALERT "%s failed with \n","Sorry, registering the character device ");
	}
	dev_class_2 = class_create(THIS_MODULE, DEVICE_NAME_2);

	dev_device_2 = device_create(dev_class_2, NULL, MKDEV(major_num_2, 0), NULL, DEVICE_NAME_2);
	
	mmio_2 = ioremap(FANDECT_CTRL_REG_2,0x100);

	/*
	* Register the character device (atleast try)
	*/
	major_num_3 = register_chrdev(0,DEVICE_NAME_3, &Fops_fandect);

	/*
	* Negative values signify an error
	*/
	if (major_num_3 < 0) 
	{
		printk(KERN_ALERT "%s failed with \n","Sorry, registering the character device ");
	}
	dev_class_3 = class_create(THIS_MODULE, DEVICE_NAME_3);

	dev_device_3 = device_create(dev_class_3, NULL, MKDEV(major_num_3, 0), NULL, DEVICE_NAME_3);
	
	mmio_3 = ioremap(FANDECT_CTRL_REG_3,0x100);

	/*
	* Register the character device (atleast try)
	*/
	major_num_4 = register_chrdev(0,DEVICE_NAME_4, &Fops_fandect);

	/*
	* Negative values signify an error
	*/
	if (major_num_4 < 0) 
	{
		printk(KERN_ALERT "%s failed with \n","Sorry, registering the character device ");
	}
	dev_class_4 = class_create(THIS_MODULE, DEVICE_NAME_4);

	dev_device_4 = device_create(dev_class_4, NULL, MKDEV(major_num_4, 0), NULL, DEVICE_NAME_4);
	
	mmio_4 = ioremap(FANDECT_CTRL_REG_4,0x100);

	/*
	* Register the character device (atleast try)
	*/
	major_num_5 = register_chrdev(0,DEVICE_NAME_5, &Fops_fandect);

	/*
	* Negative values signify an error
	*/
	if (major_num_5 < 0) 
	{
		printk(KERN_ALERT "%s failed with \n","Sorry, registering the character device ");
	}
	dev_class_5 = class_create(THIS_MODULE, DEVICE_NAME_5);

	dev_device_5 = device_create(dev_class_5, NULL, MKDEV(major_num_5, 0), NULL, DEVICE_NAME_5);
	
	mmio_5 = ioremap(FANDECT_CTRL_REG_5,0x100);

	return platform_driver_register(&fandect_driver);
}


static void __exit fandect_exit(void)
{
	//unregister_chrdev(major_num_0, DEVICE_NAME_0);
	//unregister_chrdev(major_num_1, DEVICE_NAME_1);
	//device_unregister(dev_device_0);
	//device_unregister(dev_device_1);
	//class_destroy(dev_class_0);
	//class_destroy(dev_class_1);
	platform_driver_unregister(&fandect_driver);
	printk(KERN_ALERT "Goodbye module world.\n");
}

module_init(fandect_init);
module_exit(fandect_exit);
