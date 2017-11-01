#ifndef SENSORS_IO_H
#define SENSORS_IO_H

#include <linux/ioctl.h>

#define LCD                             0X91
#define LCD_IOCTL_OPEN                  _IO(LCD, 0x01)
#define LCD_IOCTL_CLOSE                 _IO(LCD, 0x02)
#define LCD_IOCTL_WRITE_BUFFER          _IOW(LCD, 0x03, char[256])
#define LCD_IOCTL_READ_BUFFER           _IOR(LCD, 0x04, char[256])
#define LCD_IOCTL_CLEAR                 _IO(LCD, 0x05)
#define LCD_IOCTL_BACKLIGHT_LEVEL       _IOW(LCD, 0x06, int)
#define LCD_IOCTL_ATA_TEST              _IOR(LCD, 0x07, int)

void st7539i_lcm_init(void);
#endif