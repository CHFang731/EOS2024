#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>

#define DEVICE_NAME "des"
#define CLASS_NAME "des_class"
#define DEVICE_CNT 1
#define MAJOR_num 0
#define MINOR_num 0
#define des_baseaddr 0x43C10000
#define des_highaddr 0x43C1FFFF


struct chardev{
	dev_t devid;
	int major;
	int minor;
	struct cdev cdev;
	struct class *class;
	struct device_node *nd;
	void __iomem *vbase;
	void __iomem *swt;
};
static struct chardev des;

static int chardev_open(struct inode* node, struct file* filp)
{
    return nonseekable_open(node, filp);
}




static ssize_t chardev_read(struct file *filp, char *buf, size_t size, loff_t *offset)
{ 
    int ret, value;
    value = (readl(des.swt)>>24)& 0xff;
    if((ret = copy_to_user(buf, &value, size))) 
	return ret;
    else
        return 0;    
}
static ssize_t chardev_write(struct file * filp, const char __user *buf, size_t size, loff_t * offset)
{

	//Xuint32 baseaddr = XPAR_AES_0_S00_AXI_BASEADDR;
      	unsigned int DES_Encrypt_KEY_1 = 0x00000000; //Xuint32
      	unsigned int DES_Encrypt_KEY_2 = 0x00000000;
      	unsigned int DES_Encrypt_DATA_1 = 0x80000000;
      	unsigned int DES_Encrypt_DATA_2 = 0x00000000;
      	unsigned int DES_Decrypt_KEY_1 = 0x00000000;
      	unsigned int DES_Decrypt_KEY_2 = 0x00000000;

      	unsigned int tmp_data1, tmp_data2;
      	writel(1, des.vbase);// write 1 reg0
      	writel(0, des.vbase);// write 0 reg0
      	writel(1, des.vbase+12);// write 1 reg1
      	writel(DES_Encrypt_KEY_1, des.vbase+28);// write DES_Encrypt_KEY_1 reg7
      	writel(DES_Encrypt_KEY_2, des.vbase+32);// write DES_Encrypt_KEY_2 reg8
      	writel(DES_Encrypt_DATA_1, des.vbase+20);// write DES_Encrypt_DATA_1 reg5
      	writel(DES_Encrypt_DATA_2, des.vbase+24);// write DES_Encrypt_DATA_2 reg6
	printk("Start encryption !\n\r");
     	writel(1, des.vbase+4);// write 1 reg1
	writel(0, des.vbase+4);// write 0 reg1
      	tmp_data1 = readl(des.vbase+36);// read reg9
      	tmp_data2 = readl(des.vbase+40);// read reg10
      	printk("Encrypted data: %08x, %08x\n\r", tmp_data1, tmp_data2);

      	writel(0, des.vbase+12);// write 0 reg3
      	writel(DES_Decrypt_KEY_1, des.vbase+28);// write DES_Decrypt_KEY_1 reg7
      	writel(DES_Decrypt_KEY_2, des.vbase+32);// write DES_Decrypt_KEY_2 reg8
      	writel(tmp_data1, des.vbase+20);// write tmp_data1 reg5
      	writel(tmp_data2, des.vbase+24);// write tmp_data2 reg6

      	printk("Start decryption!\n\r");

      	writel(1, des.vbase+4);// write 1 reg1
      	writel(0, des.vbase+4);// write 0 reg1


      	tmp_data1 = readl(des.vbase+36);// read 9
      	tmp_data2 = readl(des.vbase+40);// read 10
      	printk("Decrypted data: %08x, %08x\n\r", tmp_data1, tmp_data2);
	printk(" ");
	return 0;
}




static int chardev_release(struct inode *inode, struct file *flip){
    return 0;
}

static struct file_operations chardev_fops = {
    .owner  = THIS_MODULE,
    .open   = chardev_open,
    .write  = chardev_write,
    .read   = chardev_read,
    .release= chardev_release,
};




static int chardev_init(void)
{
    unsigned int des_base = des_baseaddr;
    des.major = MAJOR_num;
    des.minor = MINOR_num;
    if(des.major){
    	des.devid = MKDEV(des.major, 0);
	register_chrdev_region(des.devid, DEVICE_CNT, DEVICE_NAME);
    }
    else{
		if((alloc_chrdev_region(&des.devid, des.minor, DEVICE_CNT, DEVICE_NAME))<0)
			printk("allocating chrdev region failed!\n");
		else{
			des.major = MAJOR(des.devid);
			des.minor = MINOR(des.devid);
		}
    }
    printk("major:%d, minor:%d\n", des.major, des.minor);

    cdev_init(&des.cdev, &chardev_fops);
    cdev_add(&des.cdev, des.devid, DEVICE_CNT);

    des.class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(des.class, NULL, des.devid, NULL, DEVICE_NAME);
	
    
    des.vbase = ioremap(des_base, des_highaddr-des_baseaddr+1);
    if(des.vbase)
	printk("phyaddr: 0x%08x => viraddr: 0x%p\n", des_base, des.vbase);
    else
	return -EINVAL;
    des.swt = des.vbase + 1;

    return 0;
}
static void chardev_exit(void){
    printk(KERN_ALERT "Close Module. \n");
    cdev_del(&des.cdev);
    unregister_chrdev_region(des.devid, DEVICE_CNT);
    device_destroy(des.class, des.devid);
    class_destroy(des.class);
};
module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");

