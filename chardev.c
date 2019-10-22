#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include "chardev.h"
#include "mydev_main.h"


// определение функций файловых операций
static int mydev_open(struct inode *inode, struct file *file);
static int mydev_release(struct inode *inode, struct file *file);
static ssize_t mydev_read(struct file *file, char __user *buf, size_t count, loff_t *offset);
static ssize_t mydev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset);


// настраиваем структуру file_operations
static const struct file_operations mydev_fops = {
	.owner      = THIS_MODULE,
	.open       = mydev_open,
	.release    = mydev_release,
	.read       = mydev_read,
	.write       = mydev_write
};


// структура устройства, необходимая для инициализации
struct my_device_data {
	struct device* mydev;
	struct cdev cdev;
};

// в этой переменной хранится уникальный номер нашего символьного устройства
// номер создается с помощью макроса ядра и используется в операциях инициализации
// и уничтожения устройства, поэтому его необходимо объявлять глобально и статически 
static int dev_major = 0;

// структура класса устройства, необходимая для появления устройства в /sys
// это дает возможность взаимодействовать с udev
static struct class *mydevclass = NULL;

//Создаем экземпляр структуры my_device data
static struct my_device_data mydev_data;

struct my_device_private {
	struct my_driver_priv * drv;
};

static struct my_driver_priv* drv_access = NULL;

uint32_t get_data_from_hardware(struct my_driver_priv* drv);
void set_data(struct my_driver_priv* drv, uint32_t data);
//void copy_from_user(uint32_t data, const char __user *buf, size_t count);

int create_char_dev(struct my_driver_priv* drv)
{
    int err;
    dev_t dev;
    err = alloc_chrdev_region(&dev ,0 ,1, "my_pci_driver");
    dev_major = MAJOR(dev);
    mydevclass = class_create(THIS_MODULE, "my_pci_driver");

    //инициализируем новое устройство с заданием структуры file_operations
    cdev_init(&mydev_data.cdev, &mydev_fops);

    //указываем владельца устройства - текущий модуль ядра
    mydev_data.cdev.owner = THIS_MODULE;

    // добавляем в ядро новое символьное устройство
    cdev_add(&mydev_data.cdev, MKDEV(dev_major, 0), 1);

    // и наконец создаем файл устройства /dev/mydev-<i>
    // где вместо <i> будет порядковый номер устройства
    mydev_data.mydev = device_create(mydevclass, NULL, MKDEV(dev_major, 0), NULL, "mypci");
    drv_access  =drv;
    return 0;
}

static int mydev_open(struct inode *inode, struct file *file)
{
	struct my_device_private* dev_priv;

	// получаем значение minor из текущего узла файловой системы
	
	// выделяем память под структуру и инициализируем её поля
	dev_priv = kzalloc(sizeof(struct my_device_private), GFP_KERNEL);

	//  drv_access — глобальный указатель на структуру  my_device_private, которая была
	// инициализирована ранее в коде, работающим с PCI
	dev_priv -> drv = drv_access;
	

	// сохраняем указатель на структуру как приватные данные открытого файла
	// теперь эта структура будет доступна внутри всех операций, выполняемых над этим файлом
	file->private_data = dev_priv;
	printk("Device opened");
	// просто возвращаем 0, вызов open() завершается успешно
	return 0;
}

static ssize_t mydev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	
	// получаем доступ к приватной структуре, сохраненной в функции open()
	struct my_device_private* drv = file->private_data;
	uint32_t result;
	printk("attempted read: buffer size %d, count %d \n", sizeof(buf), count);
	// выполняем какой-то запрос к оборудованию с помощью нашей функции  	
	// get_data_from_hardware, передав ей данные драйвера
	
	result = get_data_from_hardware(drv->drv);
	printk("size of res %d" ,sizeof(result));
	// копируем результат в пользовательское пространство
	if (copy_to_user(buf, &result, count)) {
		return -EFAULT;
	}

	// возвращаем количество отданных байт
	return count;
}

static ssize_t mydev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	struct my_device_private* drv = file->private_data;
	u32 data =0;

	if (copy_from_user(&data, buf, count) != 0) {
		return -EFAULT;
	}
	else{
		set_data(drv->drv, data);
		return count;

	}
	// в массив data, размером 42 байта получили какие-то данные от пользователя 
	// и теперь можем безопасно с ним работать

	// возвращаем количество принятых байт
}

static int mydev_release(struct inode *inode, struct file *file)
{
	struct my_device_private* priv = file->private_data;

	kfree(priv);

	priv = NULL;
	printk("dev released");
	return 0;
}

int destroy_char_dev(void)
{
    device_destroy(mydevclass, MKDEV(dev_major, 0));
	class_unregister(mydevclass);
	class_destroy(mydevclass);
	unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);
	printk("dev destroyed");
	return 0;
}

uint32_t get_data_from_hardware(struct my_driver_priv* drv)
{
	uint32_t res;
	void* read_addr = drv -> hwmem;
	res = ioread32(read_addr); 

	printk("raw data from reg 0x%p is %X \n", drv-> hwmem, res);

	return res;

}

void set_data(struct my_driver_priv* drv, u32 data)
{
	void* wr_addr = drv -> hwmem;
	iowrite32(data, wr_addr); 

	printk("write address to reg 0x%p %d \n", drv-> hwmem, data);


}