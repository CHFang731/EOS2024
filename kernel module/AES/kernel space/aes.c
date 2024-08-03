#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/of_irq.h>
#include <linux/cdev.h>


#define DEVICE_NAME "aes"
#define CLASS_NAME "aes_class"
#define DEVICE_CNT 1
#define MAJOR_num 0
#define MINOR_num 0
#define aes_baseaddr 0x43C00000
#define aes_highaddr 0x43C0FFFF


struct chardev{
	dev_t devid;
	int major;
	int minor;
	struct cdev cdev;
	struct class *class;
	struct device_node *nd;
	void __iomem *vbase;
	void __iomem *swt;
        unsigned int irq;
};
static struct chardev aes;

static int chardev_open(struct inode* node, struct file* filp)
{
    return nonseekable_open(node, filp);
}




static ssize_t chardev_read(struct file *filp, char *buf, size_t size, loff_t *offset)
{ 
    int ret, value;
    value = (readl(aes.swt)>>24)& 0xff;
    if((ret = copy_to_user(buf, &value, size))) 
	return ret;
    else
        return 0;    
}
static ssize_t chardev_write(struct file * filp, const char __user *buf, size_t size, loff_t * offset)
{

	//Xuint32 baseaddr = XPAR_AES_0_S00_AXI_BASEADDR;
      	unsigned int AES_Encrypt_KEY_1 = 0x00010203;//66051 //Xuint32 
      	unsigned int AES_Encrypt_KEY_2 = 0x04050607;//67438087
      	unsigned int AES_Encrypt_KEY_3 = 0x08090a0b;//134810123
      	unsigned int AES_Encrypt_KEY_4 = 0x0c0d0e0f;

      	unsigned int AES_Encrypt_DATA_1 = 0x00112233;//1122867
      	unsigned int AES_Encrypt_DATA_2 = 0x44556677;//1146447479
      	unsigned int AES_Encrypt_DATA_3 = 0x8899aabb;
      	unsigned int AES_Encrypt_DATA_4 = 0xccddeeff;

      	unsigned int AES_Decrypt_KEY_1 = 0x13111D7F;
      	unsigned int AES_Decrypt_KEY_2 = 0xE3944A17;
      	unsigned int AES_Decrypt_KEY_3 = 0xF307A78B;
      	unsigned int AES_Decrypt_KEY_4 = 0x4D2B30C5;
      	unsigned int tmp_data1, tmp_data2, tmp_data3, tmp_data4;
      	writel(1, aes.vbase+48);// write 1 reg12
      	writel(0, aes.vbase+48);// write 0 reg12
      	writel(1, aes.vbase+4);// write 1 reg1
      	writel(AES_Encrypt_KEY_1, aes.vbase+12);// write AES_Encrypt_KEY_1 reg3
      	writel(AES_Encrypt_KEY_2, aes.vbase+16);// write AES_Encrypt_KEY_2 reg4
      	writel(AES_Encrypt_DATA_1, aes.vbase+20);// write AES_Encrypt_DATA_1 reg5
      	writel(AES_Encrypt_DATA_2, aes.vbase+24);// write AES_Encrypt_DATA_2 reg6

      	writel(1, aes.vbase+8);// write 1 reg2
      	writel(AES_Encrypt_KEY_3, aes.vbase+12);// write AES_Encrypt_KEY_3 reg3
      	writel(AES_Encrypt_KEY_4, aes.vbase+16);// write AES_Encrypt_KEY_4 reg4
      	writel(AES_Encrypt_DATA_3, aes.vbase+20);// write AES_Encrypt_DATA_3 reg5
      	writel(AES_Encrypt_DATA_4, aes.vbase+24);// write AES_Encrypt_DATA_4 reg6

      	writel(0, aes.vbase+8);//write 0 reg2
      	printk("Start encryption!\n\r");
      	writel(1, aes.vbase);// write 1 reg0
      	writel(0, aes.vbase);// write 0 reg0

      	while(readl(aes.vbase+44) == 0);// read reg11

      	tmp_data1 = readl(aes.vbase+28);// read reg7
      	tmp_data2 = readl(aes.vbase+32);// read reg8
      	tmp_data3 = readl(aes.vbase+36);// read reg9
      	tmp_data4 = readl(aes.vbase+40);// read reg10

      	printk("Encrypted data: %08x, %08x, %08x, %08x\n\r", tmp_data1, tmp_data2, tmp_data3, tmp_data4);

      	writel(0, aes.vbase+4);// write 0 reg1
      	writel(AES_Decrypt_KEY_1, aes.vbase+12);// write AES_Decrypt_KEY_1 reg3
      	writel(AES_Decrypt_KEY_2, aes.vbase+16);// write AES_Decrypt_KEY_2 reg4
      	writel(tmp_data1, aes.vbase+20);// write tmp_data1 reg5
      	writel(tmp_data2, aes.vbase+24);// write tmp_data2 reg6

      	writel(1, aes.vbase+8);// write 1 reg2

      	writel(AES_Decrypt_KEY_3, aes.vbase+12);// write AES_Decrypt_KEY_3 reg3
      	writel(AES_Decrypt_KEY_4, aes.vbase+16);// write AES_Decrypt_KEY_4 reg4
      	writel(tmp_data3, aes.vbase+20);// write tmp_data3 reg5
      	writel(tmp_data4, aes.vbase+24);// write tmp_data4 reg6

      	writel(0, aes.vbase+8);// write 0 reg2

      	printk("Start decryption!\n\r");

      	writel(1, aes.vbase);// write 1 reg0
      	writel(0, aes.vbase);// write 0 reg0

      	while(readl(aes.vbase+44) == 0);// read reg11

      	tmp_data1 = readl(aes.vbase+28);// read 7
      	tmp_data2 = readl(aes.vbase+32);// read 8
      	tmp_data3 = readl(aes.vbase+36);// read 9
      	tmp_data4 = readl(aes.vbase+40);// read 10
      	printk("Decrypted data: %08x, %08x, %08x, %08x\n\r", tmp_data1, tmp_data2, tmp_data3, tmp_data4);

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
//interrupt
static irqreturn_t btn_handler(int irq, void *dev_id){
	printk("BTN Interrupt!!!!!\n");
	return 0;
}

static void get_node_irq(void)
{
    int ret;
    aes.nd = of_find_node_by_path("/amba_pl/AES@43c00000");
    if(aes.nd == NULL)
	printk("no node from dts found!\n");
    aes.irq = irq_of_parse_and_map(aes.nd, 0);
    printk("virtual irq: %d\n", aes.irq);
    ret = request_irq(aes.irq, btn_handler, IRQF_TRIGGER_RISING, "aes", NULL); 
    if(ret < 0)
	printk("request_irq %d failed, ret = %d\n", aes.irq, ret);	
}
//interrupt



static int chardev_init(void)
{
    unsigned int aes_base = aes_baseaddr;
    aes.major = MAJOR_num;
    aes.minor = MINOR_num;
    if(aes.major){
    	aes.devid = MKDEV(aes.major, 0);
	register_chrdev_region(aes.devid, DEVICE_CNT, DEVICE_NAME);
    }
    else{
		if((alloc_chrdev_region(&aes.devid, aes.minor, DEVICE_CNT, DEVICE_NAME))<0)
			printk("allocating chrdev region failed!\n");
		else{
			aes.major = MAJOR(aes.devid);
			aes.minor = MINOR(aes.devid);
		}
    }
    printk("major:%d, minor:%d\n", aes.major, aes.minor);

    cdev_init(&aes.cdev, &chardev_fops);
    cdev_add(&aes.cdev, aes.devid, DEVICE_CNT);

    aes.class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(aes.class, NULL, aes.devid, NULL, DEVICE_NAME);
    get_node_irq();
    
    aes.vbase = ioremap(aes_base, aes_highaddr-aes_baseaddr+1);
    if(aes.vbase)
	printk("phyaddr: 0x%08x => viraddr: 0x%p\n", aes_base, aes.vbase);
    else
	return -EINVAL;
    aes.swt = aes.vbase + 1;

    return 0;
}
static void chardev_exit(void){
    printk(KERN_ALERT "Close Module. \n");
    cdev_del(&aes.cdev);
    unregister_chrdev_region(aes.devid, DEVICE_CNT);
    device_destroy(aes.class, aes.devid);
    class_destroy(aes.class);
    iounmap(aes.vbase);
    free_irq(aes.irq, NULL);
};
module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");

