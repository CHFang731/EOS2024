#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#define DEVICE_NAME "gcd"
#define CLASS_NAME "gcd_class"
#define DEVICE_CNT 1
#define MAJOR_num 0
#define MINOR_num 0
#define gcd_baseaddr 0x43C20000
#define gcd_highaddr 0x43C2FFFF
int flag = 0;
struct chardev {
    dev_t devid;
    int major;
    int minor;
    struct cdev cdev;
    struct class *class;
    void __iomem *vbase;
    void __iomem *swt;
};

static struct chardev gcd;

static int chardev_open(struct inode* node, struct file* filp) {
    return nonseekable_open(node, filp);
}

static ssize_t chardev_read(struct file *filp, char __user *buf, size_t size, loff_t *offset) {
    int ret, value;
    value = (readl(gcd.vbase+16));
    if ((ret = copy_to_user(buf, &value, size))) 
        return ret;
    else
        return 0;    
}

static ssize_t chardev_write(struct file *filp, const char __user *buf, size_t size, loff_t *offset) {
    int value;
    writel(2,gcd.vbase);//reg0;
    if(flag == 0){
	//printk("first");
	copy_from_user(&value, buf, size);
	writel(value,gcd.vbase+4);//reg1
	flag=1;	
    }else{
	//printk("second");
	copy_from_user(&value, buf, size);
        writel(value,gcd.vbase+8);//reg2
	writel(1,gcd.vbase);//reg0
	printk("GCD (%d, %d) = %d \n", readl(gcd.vbase+4), readl(gcd.vbase+8), readl(gcd.vbase+12));
	flag=0;
    }
    //printk("%d",value);

    /*
    writel(2,gcd.vbase);//reg0
    copy_from_user(&value1, buf, size);
    writel(value1,gcd.vbase+4);//reg1
    copy_from_user(&value2, buf, size);
    writel(value2,gcd.vbase+8);//reg2
    writel(1,gcd.vbase);//reg0
    */

    //printk("GCD(%d, %d) = %d\n\r",value1,value2,readl(gcd.vbase+12));
    printk(" ");
    return 0;
}

static int chardev_release(struct inode *inode, struct file *flip) {
    return 0;
}

static struct file_operations chardev_fops = {
    .owner   = THIS_MODULE,
    .open    = chardev_open,
    .write   = chardev_write,
    .read    = chardev_read,
    .release = chardev_release,
};

static int chardev_init(void) {
    unsigned int gcd_base = gcd_baseaddr;
    gcd.major = MAJOR_num;
    gcd.minor = MINOR_num;
    if (gcd.major) {
        gcd.devid = MKDEV(gcd.major, 0);
        register_chrdev_region(gcd.devid, DEVICE_CNT, DEVICE_NAME);
    } else {
        if (alloc_chrdev_region(&gcd.devid, gcd.minor, DEVICE_CNT, DEVICE_NAME) < 0)
            printk("allocating chrdev region failed!\n");
        else {
            gcd.major = MAJOR(gcd.devid);
            gcd.minor = MINOR(gcd.devid);
        }
    }
    printk("major:%d, minor:%d\n", gcd.major, gcd.minor);

    cdev_init(&gcd.cdev, &chardev_fops);
    cdev_add(&gcd.cdev, gcd.devid, DEVICE_CNT);

    gcd.class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(gcd.class, NULL, gcd.devid, NULL, DEVICE_NAME);
    
    gcd.vbase = ioremap(gcd_base, gcd_highaddr - gcd_baseaddr + 1);
    if (gcd.vbase)
        printk("phyaddr: 0x%08x => viraddr: 0x%p\n", gcd_base, gcd.vbase);
    else
        return -EINVAL;

    gcd.swt = gcd.vbase + 1;

    return 0;
}

static void chardev_exit(void) {
    printk(KERN_ALERT "Close Module. \n");
    cdev_del(&gcd.cdev);
    unregister_chrdev_region(gcd.devid, DEVICE_CNT);
    device_destroy(gcd.class, gcd.devid);
    class_destroy(gcd.class);
    iounmap(gcd.vbase);
};

module_init(chardev_init);
module_exit(chardev_exit);
MODULE_LICENSE("GPL");

