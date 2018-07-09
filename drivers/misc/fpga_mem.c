#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/io.h>				//virt_to_phys
#include <linux/ioport.h>		//request_mem_region
#include <linux/slab.h>			//kmalloc and kfree
#include <linux/mm.h>			//remap_pfn_range
#include <linux/device.h>		//class_create
#include "fpga_mem.h"

static unsigned int *base_vir_addr=NULL;
static struct resource *base_vir_mem_addr=NULL;
static dev_t fpga_mem_num;
static struct cdev *p_fpga_mem=NULL;
static int fpga_mem_major = 0;
static struct class *fpga_mem_class = NULL;

static int fpga_mem_open(struct inode *inode, struct file *filp)
{
	//printk(KERN_EMERG "fpga mem is open!\n");
	return 0;
}

static ssize_t fpga_mem_mmap(struct file *filp, struct vm_area_struct *vma)  
{  
	int ret;  
	//printk(KERN_EMERG "In axi_fpga_dev_mmap!\n");
	
	vma->vm_flags |= VM_IO;
	vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);	//禁止了相关页的cache和写缓冲(write buffer)
	
	ret = remap_pfn_range(vma,  
				vma->vm_start,  
				(FPGA_MEM_START_ADDR + FPGA_MEM_OFFSET_ADDR) >> PAGE_SHIFT,
				//vma->vm_pgoff,
				vma->vm_end-vma->vm_start,  
				vma->vm_page_prot);  
    
	if(ret != 0)
	{   
		printk("fpga_mem_mmap error!\n");
		return -9;  
	}  
    
	return 0;  
}

static int fpga_mem_release(struct inode *node, struct file* filp)
{
	//printk(KERN_EMERG "fpga mem is released!\n");
	return 0;
}

static const struct file_operations fpga_mem_fops =
{
	.owner = THIS_MODULE,
	.open = fpga_mem_open,
	.mmap = fpga_mem_mmap,
	.release = fpga_mem_release,
};

static int __init fpga_mem_init(void)
{
	int ret;
	char *p = "fpga_vir_mem";
	
	printk("In fpga mem driver!\n");
	
	if(fpga_mem_major)
	{
		fpga_mem_num = MKDEV(fpga_mem_major, 0);
		ret = register_chrdev_region(fpga_mem_num, 1, "fpga_mem");
	}
	else
	{
		ret = alloc_chrdev_region(&fpga_mem_num, 0, 1, "fpga_mem");
	}
	if(ret < 0)
	{
		printk("alloc fpga_mem fail!\n");
		return ret;
	}
	
	p_fpga_mem = kmalloc(sizeof(struct cdev), GFP_KERNEL);
	if(!p_fpga_mem)
	{
		printk("kmalloc cdev fail!\n");
		return -10;
	}
	
	cdev_init(p_fpga_mem, &fpga_mem_fops);
	p_fpga_mem->owner = THIS_MODULE;
	
	ret = cdev_add(p_fpga_mem, fpga_mem_num, 1);
	if(ret)
	{
		printk("add fpga_mem fail!\n");
		return -11;
	}
	
    base_vir_mem_addr = request_mem_region(FPGA_MEM_START_ADDR + FPGA_MEM_OFFSET_ADDR, FPGA_MEM_LEN, p);
	if(!base_vir_mem_addr)
	{
		printk("request_mem_region failed!\n");
		return -12;
	}
	printk("request_mem_region OK!\n");
	
	base_vir_addr = ioremap(FPGA_MEM_START_ADDR + FPGA_MEM_OFFSET_ADDR, FPGA_MEM_LEN);
	if(!base_vir_addr)
	{
		printk("ioremap fail!\n");
		printk("!!!*base_vir_addr = 0x%x\n", (unsigned int)base_vir_addr);
		return -13;
	}
	printk("fpga mem virtual address is 0x%x\n", (unsigned int)base_vir_addr);
	
	fpga_mem_class = class_create(THIS_MODULE, "fpga_mem");
	if (IS_ERR(fpga_mem_class))
	{
        	printk("Err:failed in creating fpga mem class.\n");
        	return -14;
	}
	device_create(fpga_mem_class, NULL, fpga_mem_num, NULL, "fpga_mem");
	
	return 0;
}

static void __exit fpga_mem_exit(void)
{
    unregister_chrdev_region(fpga_mem_num, 1);
	cdev_del(p_fpga_mem);
    kfree(p_fpga_mem);
    iounmap(base_vir_addr);
	release_mem_region(FPGA_MEM_START_ADDR + FPGA_MEM_OFFSET_ADDR, FPGA_MEM_LEN);
    device_destroy(fpga_mem_class, fpga_mem_num);
    class_destroy(fpga_mem_class);
	printk("Bye Bye fpga mem driver!\n");
}

module_init(fpga_mem_init);
module_exit(fpga_mem_exit);

MODULE_LICENSE("Dual BSD/GPL");
