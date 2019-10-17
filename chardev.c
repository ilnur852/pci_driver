#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include "chardev.h"


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
	uint8_t chnum;
	struct my_driver_priv * drv;
};

static struct my_driver_priv* drv_access = NULL;

uint32_t get_data_from_hardware(struct my_driver_priv* drv);
//void copy_from_user(uint32_t data, const char __user *buf, size_t count);

int create_char_dev(struct my_driver_priv* drv)
{
    int err;
    dev_t dev;
    err = alloc_chrdev_region(&dev ,0 ,1, "pci_chdev");
    dev_major = MAJOR(dev);
    mydevclass = class_create(THIS_MODULE, "pci_chdev");

    //инициализируем новое устройство с заданием структуры file_operations
    cdev_init(&mydev_data.cdev, &mydev_fops);

    //указываем владельца устройства - текущий модуль ядра
    mydev_data.cdev.owner = THIS_MODULE;

    // добавляем в ядро новое символьное устройство
    cdev_add(&mydev_data.cdev, MKDEV(dev_major, 0), 1);

    // и наконец создаем файл устройства /dev/mydev-<i>
    // где вместо <i> будет порядковый номер устройства
    mydev_data.mydev = device_create(mydevclass, NULL, MKDEV(dev_major, 0), NULL, "mydev-%d", 0);
    
    return 0;
}

static int mydev_open(struct inode *inode, struct file *file)
{
	struct my_device_private* dev_priv;

	// получаем значение minor из текущего узла файловой системы
	unsigned int minor = iminor(inode);

	// выделяем память под структуру и инициализируем её поля
	dev_priv = kzalloc(sizeof(struct my_device_private), GFP_KERNEL);

	//  drv_access — глобальный указатель на структуру  my_device_private, которая была
	// инициализирована ранее в коде, работающим с PCI
	dev_priv->drv = drv_access;
	dev_priv ->chnum = minor;

	// сохраняем указатель на структуру как приватные данные открытого файла
	// теперь эта структура будет доступна внутри всех операций, выполняемых над этим файлом
	file->private_data = dev_priv;

	// просто возвращаем 0, вызов open() завершается успешно
	return 0;
}

static ssize_t mydev_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	// получаем доступ к приватной структуре, сохраненной в функции open()
	struct my_device_private* drv = file->private_data;
	uint32_t result;

	// выполняем какой-то запрос к оборудованию с помощью нашей функции  	
	// get_data_from_hardware, передав ей данные драйвера
	result = get_data_from_hardware(drv->drv);

	// копируем результат в пользовательское пространство
	if (copy_to_user(buf, &result, count)) {
		return -EFAULT;
	}

	// возвращаем количество отданных байт
	return count;
}

static ssize_t mydev_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	char data[count];

	if (copy_from_user(data, buf, count) != 0) {
		return -EFAULT;
	}

	// в массив data, размером 42 байта получили какие-то данные от пользователя 
	// и теперь можем безопасно с ним работать

	// возвращаем количество принятых байт
	return count;
}

static int mydev_release(struct inode *inode, struct file *file)
{
	struct my_device_private* priv = file->private_data;

	kfree(priv);

	priv = NULL;

	return 0;
}

int destroy_char_dev(void)
{
    device_destroy(mydevclass, MKDEV(dev_major, 0));
	class_unregister(mydevclass);
	class_destroy(mydevclass);
	unregister_chrdev_region(MKDEV(dev_major, 0), MINORMASK);

	return 0;
}

uint32_t get_data_from_hardware(struct my_driver_priv* drv)
{
	uint32_t res;
	void* read_addr = drv->hwmem + 32;

	res = ioread32(read_addr); 

	printk("raw data from reg: 0x%X\n", res);

	return res;

}

