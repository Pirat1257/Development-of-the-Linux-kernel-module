#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/hcd.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/mutex.h>
#include <linux/fcntl.h>
#include <linux/stat.h>


#include <linux/ioctl.h>      
#include <linux/unistd.h>

#define MON_IOC_MAGIC 0x92

#include "BOS_2.h"

#define MON_IOCX_MFETCH  _IOWR(MON_IOC_MAGIC, 7, struct mon_mfetch_arg)
#define MON_IOCX_GET     _IOW(MON_IOC_MAGIC, 6, struct mon_bin_get)
#define MON_IOCQ_URB_LEN _IO(MON_IOC_MAGIC, 1)

#define BUFF_LEN 1024 // 1k default buffer

DEFINE_MUTEX(mon_lock);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivanov M.S.");
MODULE_DESCRIPTION("BOS_2: USB Sniffer");
MODULE_VERSION("0.01");

static LIST_HEAD(mon_buses); // Все известные bus

/*----Вывод траффика----*/
void start_sniff(struct file* fd)
{
    int len = 0;
    while(1) {
        char buff[1024] = {0};
        file_read(fd, 0, buff, 1024); // Считывваем строку
        if (buff[0] != 0) {
            struct file* output_file = file_open("/home/misha/Desktop/BOS_2/output.txt", O_WRONLY, 0664);
            if (!IS_ERR(output_file)) {
                len += file_write(output_file, len, buff, strlen(buff));
                file_close(output_file);
            }
        }
        else break;
    }
}

/*----Обработчик событий----*/
static int usb_notify(struct notifier_block *self, unsigned long action, void *dev)
{
    mutex_lock(&mon_lock);
    printk(KERN_INFO "BOS_2: USB notify.\n");

    struct file* fd = file_open("/sys/kernel/debug/usb/usbmon/0u", O_RDONLY, 0);
    if(fd == NULL) {
        printk(KERN_INFO "BOS_2: Note: usbmon driver must be loaded before you try BOS_2.\n");
        mutex_unlock(&mon_lock);
        return NOTIFY_OK;
    }

    start_sniff(fd);
    file_close(fd);

    mutex_unlock(&mon_lock);
    return NOTIFY_OK;
}

/*----Привязываем обработчик событий----*/
static struct notifier_block usb_nb = {
    .notifier_call = usb_notify,
};

/*----Инициализация модуля----*/
static int __init BOS_module_init(void)
{
    printk(KERN_INFO "BOS_2: USB sniffer start.\n");
    // Регистрируем драйвер для получения уведомлений от usbmon
    usb_register_notify(&usb_nb);
    struct file* output_file = file_open("/home/misha/Desktop/BOS_2/output.txt", O_CREAT, 0664);
    file_close(output_file);
}

/*----Удаление модуля----*/
static void __exit BOS_module_exit(void)
{
    printk(KERN_INFO "BOS_2: USB sniffer stop.\n");
    // Выгружаем драйвер
    usb_unregister_notify(&usb_nb);
}

/*----Регистрация функций модуля----*/
module_init(BOS_module_init);
module_exit(BOS_module_exit);