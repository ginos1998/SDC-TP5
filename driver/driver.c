/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple GPIO driver explanation
*
*  \author     EmbeTronicX
*
*  \Tested with Linux raspberrypi 5.4.51-v7l+
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  
#include <linux/err.h>
//LED is connected to this GPIO
#define GPIO_23 (23)
#define GPIO_21 (21)

 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_gpio_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
 
/*************** Driver functions **********************/
static int my_gpio_open(struct inode *inode, struct file *file);
static int my_gpio_release(struct inode *inode, struct file *file);
static ssize_t my_gpio_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t my_gpio_write(struct file *filp, const char *buf, size_t len, loff_t * off);
/******************************************************/
 
static struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = my_gpio_read,
    .write          = my_gpio_write,
    .open           = my_gpio_open,
    .release        = my_gpio_release,
};

static int my_gpio_open(struct inode *inode, struct file *file)
{
    pr_info("Abriste el CDF de GPIO!!\n");
    return 0;
}

static int my_gpio_release(struct inode *inode, struct file *file)
{
    pr_info("Gracias vuelva prontos!!!\n");
    return 0;
}


static ssize_t my_gpio_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{

}


static ssize_t my_gpio_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{

}

static int __init my_gpio_driver_init(void){

    if((alloc_chrdev_region(&dev, 0, 1, "my_gpio_Dev")) <0){
        pr_err("Error alocando major number\n");
        goto r_unreg;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    cdev_init(&my_gpio_cdev,&fops);

    if((cdev_add(&my_gpio_cdev,dev,1)) < 0){
        pr_err("No se puede agregar el dispositivo\n");
        goto r_del;
    }

    if(IS_ERR(dev_class = class_create(THIS_MODULE,"my_gpio_class"))){
        pr_err("No se pudo crear la estuctura\n");
        goto r_class;
    }

    if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"my_gpio_device"))){
        pr_err( "No se pudo crear el dispositivo \n");
        goto r_device;
    }

    if(gpio_is_valid(GPIO_23) == false){
        pr_err("GPIO %d no es valido\n", GPIO_23);
        goto r_device;
    }
    if (gpio_is_valid(GPIO_21) == false){
        pr_err("GPIO %d no es valido\n", GPIO_21);
        goto r_device;
    }

    if(gpio_request(GPIO_23,"GPIO_23") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_23);
        goto r_gpio;
    }
    if(gpio_request(GPIO_21,"GPIO_21") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_21);
        goto r_gpio;
    }

    gpio_direction_input(GPIO_23);
    gpio_direction_input(GPIO_21);

    gpio_export(GPIO_21, false);
    gpio_export(GPIO_23, false);    

    pr_info("Device Driver configurado\n");
    return 0;

    rgpio:
    gpio_free(GPIO_21);
    gpio_free(GPIO_23);
    rdevice:
    device_destroy(dev_class,dev);
    rclass:
    class_destroy(dev_class);
    rdel:
    cdev_del(&my_gpio_cdev);
    runreg:
    unregister_chrdev_region(dev,1);

    return -1;
}

static void __exit my_gpio_driver_exit(void)
{
    gpio_unexport(GPIO_21);
    gpio_unexport(GPIO_23);
    gpio_free(GPIO_21);
    gpio_free(GPIO_23);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&my_gpio_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Derive removido\n");
}
 
module_init(my_gpio_driver_init);
module_exit(my_gpio_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Los borbotones - SC");
MODULE_DESCRIPTION("GPIO driver");
MODULE_VERSION("1.0");