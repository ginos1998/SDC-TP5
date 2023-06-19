/***************************************************************************//**
*  \file       driver.c
*
*  \details    GPIO Driver TP5 SC
*
*  \author     Los borobotones
*
*  \Tested with Linux raspberrypi 
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
#include <linux/gpio.h>     
#include <linux/err.h>
#define GPIO_BUTTOM (23)
#define GPIO_ALCOHOLMETER (21)
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev my_gpio_cdev;

static int __init my_gpio_driver_init(void);
static void __exit my_gpio_driver_exit(void);
 
uint8_t selected_pin = GPIO_BUTTOM;

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
    char gpio_state_char[3] = "0";
    int gpio_state = gpio_get_value(selected_pin);
    sprintf(gpio_state_char, "%d", gpio_state);
    int nr_bytes = strlen(gpio_state_char);

    if ((*off) > 0)
      return 0;

    if (copy_to_user(buf, &gpio_state_char, nr_bytes)) {
        pr_err("ERROR: No se pudo copiar todos los bytes al usuario\n");
        return -EFAULT;
    }

    if (selected_pin == GPIO_BUTTOM)
        pr_info("Estado del boton = %d\n", gpio_state);
    else
        pr_info("Estado del alcoholimetro = %d\n", gpio_state);

    (*off) += nr_bytes;

    return nr_bytes;
}


static ssize_t my_gpio_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    uint8_t user_input[10] = {0};
    if(copy_from_user(user_input, buf, len ) > 0) 
        pr_err("ERROR: No se pudo leer la informacion del usuario correctamente\n");

    if(user_input[0]=='0')
        selected_pin = GPIO_BUTTOM;
    else if (user_input[0]=='1')
        selected_pin = GPIO_ALCOHOLMETER;
    else
        pr_err("Error: input de selección no valido: 0 para botón, 1 para alcholimetro");

    return len;
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

    if(gpio_is_valid(GPIO_BUTTOM) == false){
        pr_err("GPIO %d no es valido\n", GPIO_BUTTOM);
        goto r_device;
    }
    if (gpio_is_valid(GPIO_ALCOHOLMETER) == false){
        pr_err("GPIO %d no es valido\n", GPIO_ALCOHOLMETER);
        goto r_device;
    }

    if(gpio_request(GPIO_BUTTOM,"GPIO_23") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_BUTTOM);
        goto r_gpio;
    }
    if(gpio_request(GPIO_ALCOHOLMETER,"GPIO_21") < 0){
        pr_err("ERROR: GPIO %d request\n", GPIO_ALCOHOLMETER);
        goto r_gpio;
    }

    gpio_direction_input(GPIO_BUTTOM);
    gpio_direction_input(GPIO_ALCOHOLMETER);

    gpio_export(GPIO_ALCOHOLMETER, false);
    gpio_export(GPIO_BUTTOM, false);    

    pr_info("Device Driver configurado\n");
    return 0;

    r_gpio:
        gpio_free(GPIO_ALCOHOLMETER);
        gpio_free(GPIO_BUTTOM);
    r_device:
        device_destroy(dev_class,dev);
    r_class:
        class_destroy(dev_class);
    r_del:
        cdev_del(&my_gpio_cdev);
    r_unreg:
        unregister_chrdev_region(dev,1);

    return -1;
}

static void __exit my_gpio_driver_exit(void)
{
    gpio_unexport(GPIO_ALCOHOLMETER);
    gpio_unexport(GPIO_BUTTOM);
    gpio_free(GPIO_ALCOHOLMETER);
    gpio_free(GPIO_BUTTOM);
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