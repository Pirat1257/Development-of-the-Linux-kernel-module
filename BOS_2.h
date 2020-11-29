#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#define SETUP_LEN 8

typedef uint16_t u16;
typedef int32_t s32;
typedef uint64_t u64;
typedef int64_t s64;


const char *PIPE[] = {"ISO", "INTR", "CTRL","BULK"};

/*----Открытые файла, аналог open----*/
struct file *file_open(const char *path, int flags, int rights) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

/*----Закрытие файла, аналог close----*/
void file_close(struct file *file) 
{
    filp_close(file, NULL);
}

/*----Чтение файла, аналог pread----*/
int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

/*----Запись в файл, аналог pwrite----*/
int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

/*----Синхронизация файла, аналог fsync----*/
int file_sync(struct file *file) 
{
    vfs_fsync(file, 0);
    return 0;
}

// usbmon_packet ( as described in kernel/Documentation/usb/usbmon.txt)
struct usbmon_packet {
    u64 id;         /*  0: URB ID - from submission to callback */
    unsigned char type; /*  8: Same as text; extensible. */
    unsigned char xfer_type; /*    ISO (0), Intr, Control, Bulk (3) */
    unsigned char epnum;    /*     Endpoint number and transfer direction */
    unsigned char devnum;   /*     Device address */
    u16 busnum;     /* 12: Bus number */
    char flag_setup;    /* 14: Same as text */
    char flag_data;     /* 15: Same as text; Binary zero is OK. */
    s64 ts_sec;     /* 16: gettimeofday */
    s32 ts_usec;        /* 24: gettimeofday */
    int status;     /* 28: */
    unsigned int length;    /* 32: Length of data (submitted or actual) */
    unsigned int len_cap;   /* 36: Delivered length */
    union {         /* 40: */
        unsigned char setup[SETUP_LEN]; /* Only for Control S-type */
        struct iso_rec {        /* Only for ISO */
            int error_count;
            int numdesc;
        } iso;
    } s;
    int interval;       /* 48: Only for Interrupt and ISO */
    int start_frame;    /* 52: For ISO */
    unsigned int xfer_flags; /* 56: copy of URB's transfer_flags */
    unsigned int ndesc; /* 60: Actual number of ISO descriptors */
};              /* 64 total length */

struct mon_mfetch_arg {
    uint32_t *offvec;   /* Vector of events fetched */
    uint32_t nfetch;    /* Number of events to fetch (out: fetched) */
    uint32_t nflush;    /* Number of events to flush */
};

struct mon_bin_get {
        struct usbmon_packet  *hdr; /* Can be 48 bytes or 64. */
        void  *data;
        size_t alloc;           /* Length of data (can be zero) */
};
