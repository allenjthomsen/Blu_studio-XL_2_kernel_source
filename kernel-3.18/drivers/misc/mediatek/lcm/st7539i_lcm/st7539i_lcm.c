#include <linux/time.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/moduleparam.h>

#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/i2c.h>

#include <linux/input.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <linux/miscdevice.h>

//for dym create device
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/device.h>
#include <asm/uaccess.h>

//kthread
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/platform_device.h>

#include <linux/semaphore.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <linux/leds.h>
#include "leds_sw.h"
#include "st7539i_lcm.h"

#include <mt-plat/upmu_common.h>

#define st7539i_DEBUG
#ifdef st7539i_DEBUG
#define st7539i_LOG(fmt, args...)			printk("[st7539i] " fmt, ##args)
#else
#define st7539i_LOG(fmt, args...)
#endif
#define st7539i_ERR(fmt, args...)			printk("[st7539i] " fmt, ##args)

#define st7539i_NAME     	"st7539i"

struct i2c_client *st7539i_lcd_client;
static const struct i2c_device_id lcd_id[] = {{st7539i_NAME,0},{}};
static struct pinctrl *pinctrl;
static struct pinctrl_state *en_low;
static struct pinctrl_state *en_high;
static struct pinctrl_state *rst_high;
static struct pinctrl_state *rst_low;
// i2c write and read
//////////////////////////////////////////////////////
unsigned char I2C_write_com(unsigned char reg_data)
{
	char ret = 0;
	u8 wdbuf[512];
	struct i2c_msg msg[2];
	wdbuf[0] = reg_data;
	
	msg[0].addr = st7539i_lcd_client->addr;
	msg[0].flags = 0;
	msg[0].timing = 200;
	msg[0].len = 1;
	msg[0].buf = wdbuf;
	msg[0].ext_flag = st7539i_lcd_client->ext_flag;
	
	ret = i2c_transfer(st7539i_lcd_client->adapter, msg, 1);
	if (ret < 0)
		st7539i_ERR("msg %s i2c read error: %d\n", __func__, ret);
	
	return ret;
}

unsigned char I2C_write_data(unsigned char reg_data)
{
	char ret = 0;
	u8 wdbuf[512];
	struct i2c_msg msg[2];
	wdbuf[0] = reg_data;
	
	msg[0].addr = 0x39;
	msg[0].flags = 0;
	msg[0].timing = 200;
	msg[0].len = 1;
	msg[0].buf = wdbuf;
	msg[0].ext_flag = st7539i_lcd_client->ext_flag;
	
	ret = i2c_transfer(st7539i_lcd_client->adapter, msg, 1);
	if (ret < 0)
		st7539i_ERR("msg %s i2c read error: %d\n", __func__, ret);
	
	return ret;
}

unsigned char I2C_write_reg(unsigned char addr, unsigned char reg_data)
{
	char ret = 0;
	u8 wdbuf[512];
	struct i2c_msg msg[2];
	wdbuf[0] = addr;
	wdbuf[1] = reg_data;
	
	msg[0].addr = st7539i_lcd_client->addr;
	msg[0].flags = 0;
	msg[0].timing = 200;
	msg[0].len = 2;
	msg[0].buf = wdbuf;
	msg[0].ext_flag = st7539i_lcd_client->ext_flag;
	
	ret = i2c_transfer(st7539i_lcd_client->adapter, msg, 1);
	if (ret < 0)
		st7539i_ERR("msg %s i2c read error: %d\n", __func__, ret);
	
	return ret;
}

unsigned char I2C_read_reg(unsigned char addr)
{
	unsigned char ret = 0;
	u8 rdbuf[512];
	struct i2c_msg msg[2];
	
	rdbuf[0] = addr;
	
	msg[0].addr = st7539i_lcd_client->addr;//(st7539i_I2C_ADDR) ;
	msg[0].flags = 0;
	msg[0].timing = 200;
	msg[0].len = 1;
	msg[0].buf = rdbuf;
	msg[0].ext_flag = st7539i_lcd_client->ext_flag;
	
	// read addr data
	msg[1].addr = st7539i_lcd_client->addr;//(st7539i_I2C_ADDR) ;
	msg[1].flags = I2C_M_RD;
	msg[1].timing = 200;
	msg[1].len = 1;
	msg[1].buf = rdbuf;
	msg[1].ext_flag = st7539i_lcd_client->ext_flag;
	
	ret = i2c_transfer(st7539i_lcd_client->adapter, msg, 2);
	if (ret < 0)
		st7539i_ERR("msg %s i2c read error: %d\n", __func__, ret);
	
	return rdbuf[0];
}

unsigned char bmp[]={/*--  调入了一幅图像：E:\wzq\编程图片\96X16.bmp  --*/
/*--  宽度x高度=96x16  --*/
//0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x09,
//0xF9,0x89,0x89,0xC9,0x19,0x01,0x09,0xF9,0x09,0x01,0x09,0xF9,0x09,0x09,0xF9,0x09,
//0x01,0x01,0x01,0x01,0x09,0xF9,0x09,0x01,0x01,0x01,0x01,0x09,0xF9,0x89,0x89,0xC9,
//0x19,0x01,0x09,0xF9,0x39,0xC1,0x09,0xF9,0x09,0x01,0x09,0x09,0xF9,0x09,0x09,0x01,
//0xE1,0x11,0x09,0x09,0x11,0xE1,0x01,0x09,0x19,0x69,0x81,0x69,0x19,0x09,0x01,0x01,
//0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,
//0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x88,
//0x8F,0x88,0x80,0x81,0x80,0x80,0x80,0x87,0x88,0x88,0x88,0x87,0x80,0x88,0x8F,0x88,
//0x88,0x88,0x8C,0x80,0x88,0x8F,0x88,0x88,0x88,0x8C,0x80,0x88,0x8F,0x88,0x88,0x89,
//0x8C,0x80,0x88,0x8F,0x88,0x81,0x86,0x8F,0x80,0x98,0x90,0x90,0x8F,0x80,0x80,0x80,
//0x83,0x84,0x88,0x88,0x84,0x83,0x80,0x80,0x80,0x88,0x8F,0x88,0x80,0x80,0x80,0x80,
//0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF

0xFF,0x01,0x0D,0x0D,0xBD,0xB9,0xB9,0xB9,0xB9,0x01,0x39,0x39,0x39,0x39,0x39,0x39,
0xF9,0xF9,0xF1,0xE1,0x01,0x01,0xE1,0xF1,0xF9,0xF9,0x39,0x39,0x39,0x39,0x39,0x39,
0x39,0x19,0xE1,0xF1,0xF1,0xF9,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0xF9,0xF1,
0xE1,0x01,0x01,0xE1,0xF1,0xF9,0x39,0x39,0x39,0x39,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,
0xB9,0xB9,0x01,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,
0x01,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0xB9,0x01,0xFF,
0xFF,0x80,0x80,0x80,0xBF,0xBF,0xBF,0xBF,0xBF,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,
0xBF,0xBF,0x9F,0x8F,0x80,0x80,0x8F,0x9F,0xBF,0xBF,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,
0xB8,0xB8,0xBF,0x9F,0x9F,0xAF,0xB0,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,0xBF,0x9F,
0x8F,0x80,0x80,0x8F,0x9F,0xBF,0xB8,0xB8,0xB8,0xB8,0xB9,0xB9,0xB9,0xB9,0xBF,0xBF,
0xBF,0xBF,0x80,0xBF,0xBF,0xBF,0xBF,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,
0x80,0xBF,0xBF,0xBF,0xBF,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0x80,0xFF

};

//显示图片
int showpic(char *p)			
{
	int i,j,ret =0;
	
	for(i=0;i<2;i++)
	{
		//ret = I2C_write_com(0x40);
		//if(ret < 0)
		//{
			//return ret;
		//}
		ret = I2C_write_com(i+0xb0);
		if(ret < 0)
		{
			return ret;
		}
		ret = I2C_write_com(0x04);
		if(ret < 0)
		{
			return ret;
		}
		ret = I2C_write_data(0x24);
		if(ret < 0)
		{
			return ret;
		}
		ret = I2C_write_com(0x01);
		if(ret < 0)
		{
			return ret;
		}		
		for(j=0;j<96;j++)
		{
			ret = I2C_write_data(p[i*96+j]);
			if(ret < 0)
			{
				return ret;
			}
		}
	}
	return ret;
}
//清屏
void clealddram(void)
{
	int i,j;
	for(i=0Xb0;i<0Xb2;i++)
	{
		I2C_write_com(i);
		I2C_write_com(0x04);
		I2C_write_data(0x00);
		I2C_write_com(0x01);
		for(j=0;j<71;j++)
		{
			I2C_write_data(0x00);
		}
	}
}

int set_lcd_backlight(int level)
{
	//mutex_lock(&leds_pmic_mutex);
	
	if (level)
	{
		pmic_set_register_value(PMIC_RG_DRV_32K_CK_PDN, 0x0);	/* Disable power down */
		pmic_set_register_value(PMIC_RG_DRV_ISINK0_CK_PDN, 0);
		pmic_set_register_value(PMIC_RG_DRV_ISINK0_CK_CKSEL, 0);
		pmic_set_register_value(PMIC_ISINK_CH0_MODE, ISINK_PWM_MODE);
		pmic_set_register_value(PMIC_ISINK_CH0_STEP, ISINK_4);	/* 20mA */
		pmic_set_register_value(PMIC_ISINK_DIM0_DUTY, 31); //10
		pmic_set_register_value(PMIC_ISINK_DIM0_FSEL, ISINK_1KHZ);	/* 1KHz */
		pmic_set_register_value(PMIC_ISINK_CH0_EN, NLED_ON);
	}
	else
		pmic_set_register_value(PMIC_ISINK_CH0_EN, NLED_OFF);
	//mutex_unlock(&leds_pmic_mutex);
	st7539i_LOG("%s \n", __func__);
	return 0;
}
/*******************************misc device start************************************/
static int st7539i_lcd_open(struct inode *inode, struct file *file)
{
	st7539i_LOG("%s \n", __func__);
    return 0; 
}

static int st7539i_lcd_release(struct inode *inode, struct file *file)
{
	st7539i_LOG("%s \n", __func__);
    return 0;
}

ssize_t st7539i_lcd_read(struct file *filp, char __user *buff, size_t count, loff_t *offset)
{
	st7539i_LOG("%s \n", __func__);
	return count;
}

ssize_t st7539i_lcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *offset)
{
    int ret;
	st7539i_LOG("%s count =%zu\n", __func__,count);
    if(count > sizeof(bmp))
	{
		return 0;
	}
	if(copy_from_user(bmp,buff,count))
	{
		return 0;
	}
	ret = showpic(bmp);
	if(ret < 0)
	{
		return ret;
	}
	return count;
}

/*******************ata test*************************/
int lcd_ata_test(void)
{
	int i = 0, err =  0;
	char databuf[2];
	pinctrl_select_state(pinctrl, en_high);
	pinctrl_select_state(pinctrl, rst_high);
	mdelay(20);
	pinctrl_select_state(pinctrl, rst_low);
	mdelay(20);
	pinctrl_select_state(pinctrl, rst_high);
	mdelay(20);
	for(i = 0; i<3; i++)
	{
		databuf[0] = 0x40;   
		//err = i2c_master_send(st7539i_lcd_client, databuf, 0x1);
		err = i2c_master_recv(st7539i_lcd_client, databuf, 0x01);
		if(err == 1)
		{
			printk("ok %s\n",__func__);
			break;
		}
	}
	if(i == 3)
	{
		return 0;
	}
	st7539i_lcm_init();
	mdelay(120);
	pinctrl_select_state(pinctrl, en_low);
	set_lcd_backlight(0);
	return 1;
}

static long st7539i_lcd_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	char *data = NULL;
	int err = 0;
	switch (cmd) 
	{
		case LCD_IOCTL_OPEN:
			st7539i_lcm_init();
			break;
		case LCD_IOCTL_CLOSE:
			pinctrl_select_state(pinctrl, en_low);
			set_lcd_backlight(0);
			break;
		case LCD_IOCTL_WRITE_BUFFER:
			data = (void __user *)arg;
			if (data == NULL)
			{
				err = -EINVAL;
				break;
			}
			if(copy_from_user(bmp,data,sizeof(bmp)))
			{
				return 0;
			}
			err = showpic(bmp);
			break;
		case LCD_IOCTL_READ_BUFFER:
			break;
		case LCD_IOCTL_CLEAR:
			clealddram();
			break;
		case LCD_IOCTL_BACKLIGHT_LEVEL:
			data = (void __user *)arg;
			set_lcd_backlight(*data);
			break;
		case LCD_IOCTL_ATA_TEST:
			data = (void __user *)arg;
			*data =lcd_ata_test();
			break;
		default:
			break;
	}
	return err;
}

static struct file_operations st7539i_lcd_fops = {
	.owner           = THIS_MODULE,
	.open            = st7539i_lcd_open,
	.release         = st7539i_lcd_release,
	.unlocked_ioctl  = st7539i_lcd_ioctl,
	.read            = st7539i_lcd_read,
	.write           = st7539i_lcd_write,
};

static struct miscdevice st7539i_lcd_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "st7539i_lcd",
	.fops = &st7539i_lcd_fops,
};
/*******************************misc device end************************************/

void st7539i_lcm_init(void)
{
	pinctrl_select_state(pinctrl, en_high);
	mdelay(2);
	//pinctrl_select_state(pinctrl, rst_high);
	//mdelay(20);
	pinctrl_select_state(pinctrl, rst_low);
	mdelay(20);
	pinctrl_select_state(pinctrl, rst_high);
	mdelay(120);

	//I2C_write_com(0x40);     //SCOLL LINE=0     ST7539    set duty
	//I2C_write_com(0x85);     //SET PARTIAL SCREEN MODE
	//I2C_write_reg(0xf1,0x0F);  //SET COM END=16
	//I2C_write_reg(0xf2,0x00);  //SET PARTIAL START ADDRESS
	//I2C_write_reg(0xf3,0x0F);  //SET PARTIAL END ADDRESS

	//I2C_write_com(0xa6);     //NORMAL DISPLAY
	//I2C_write_com(0x89);     //SET RAM ADDRESS CONTROL
	//I2C_write_com(0xa3);     //FRAME RATE=76Hz  a3
	//I2C_write_com(0xe8);     //SET BIAS=1/6  e8
	//I2C_write_reg(0x81,0x78);//SET VOP   BA=8.3V
	//I2C_write_com(0xc2);     //SET MX/MY NORMAL
	//I2C_write_com(0xaf);     //DISPLAY ON

	I2C_write_com(0xa4);		//SET MX/MY NORMAL
	I2C_write_com(0xe8);		//SET BIAS=1/4
	I2C_write_com(0x10);		//FRAME RATE=92Hz
	I2C_write_com(0x26);		//Temperature Compensation
	I2C_write_com(0x85);		//SET PARTIAL SCREEN MODE
	
	I2C_write_com(0xf1);  		//SET COM END=16
	I2C_write_data(0x0f);
	
	I2C_write_com(0xf2);  		//SET PARTIAL START ADDRESS
	I2C_write_data(0x00);
	
	I2C_write_com(0xf3);  		//SET PARTIAL END ADDRESS	
	I2C_write_data(0x0f);
	
	I2C_write_com(0x81);		//SET VOP   BA=8.3V
	I2C_write_data(0x45);
	
	I2C_write_com(0x2c);		//Power Control
	I2C_write_com(0xc9);
	I2C_write_data(0xaf);		//DISPLAY ON
	
	
	set_lcd_backlight(255);
	showpic(bmp);
	st7539i_LOG("%s \n", __func__);
}

static int st7539i_i2c_remove(struct i2c_client *client)
{
	return 0;
}

static int st7539i_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;
	int i = 0;
	char databuf[2];
	st7539i_LOG("i2c_Probe\n");

	st7539i_lcd_client = client;
	st7539i_lcd_client->addr = 0x38;
	
	
	pinctrl_select_state(pinctrl, en_high);
	pinctrl_select_state(pinctrl, rst_high);
	mdelay(20);
	pinctrl_select_state(pinctrl, rst_low);
	mdelay(20);
	pinctrl_select_state(pinctrl, rst_high);
	mdelay(20);
	for(i = 0; i<3; i++)
	{
		databuf[0] = 0x40;   
		//err = i2c_master_send(st7539i_lcd_client, databuf, 0x1);
		err = i2c_master_recv(st7539i_lcd_client, databuf, 0x01);
		if(err == 1)
		{
			printk("ok %s\n",__func__);
			break;
		}
	}
	if(i == 3)
	{
		pinctrl_select_state(pinctrl, en_low);
		return err;
	}
	
	st7539i_lcm_init();

	st7539i_LOG("i2c probe end\n");

	return err;
}

static int st7539i_i2c_resume(struct i2c_client *client)
{
	//st7539i_lcm_init();
	return 0;
}

static int st7539i_i2c_suspend(struct i2c_client *client, pm_message_t msg)
{
	//pinctrl_select_state(pinctrl, en_low);
	//set_lcd_backlight(0);
	return 0;
}

struct of_device_id st7539i_of_match[] = {
	{ .compatible = "mediatek,LCM", },
	{},
};

static struct i2c_driver st7539i_i2c_driver = 
{  
	.probe 	 	= st7539i_i2c_probe,
	.remove	 	= st7539i_i2c_remove,
	.suspend	= st7539i_i2c_suspend,
	.resume	 	= st7539i_i2c_resume,
	.id_table	= lcd_id,
	.driver 	= 
	{
		.owner             = THIS_MODULE,
		.name              = st7539i_NAME,
		#ifdef CONFIG_OF
		.of_match_table    = st7539i_of_match,
		#endif
	},
};


static int st7539i_lcd_remove(struct platform_device *pdev)
{
	i2c_del_driver(&st7539i_i2c_driver);
	return 0;
}

static void st7539i_lcd_shutdown(struct platform_device *dev)
{

	return ;
}

static int st7539i_lcd_probe(struct platform_device *pdev) 
{
	int ret = 0;
	pinctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(pinctrl)) 
	{
		ret = PTR_ERR(pinctrl);
		st7539i_LOG("Cannot find st7539i pinctrl!\n");
		return ret;
	}
	
	en_high = pinctrl_lookup_state(pinctrl, "en_high");
	if (IS_ERR(en_high)) {
		ret = PTR_ERR(en_high);
		st7539i_LOG("Cannot find st7539i en_high!\n");
		return ret;
	}
	en_low = pinctrl_lookup_state(pinctrl, "en_low");
	if (IS_ERR(en_low)) {
		ret = PTR_ERR(en_low);
		st7539i_LOG("Cannot find st7539i en_low!\n");
		return ret;
	}
	
	rst_high = pinctrl_lookup_state(pinctrl, "rst_high");
	if (IS_ERR(rst_high))
	{
		ret = PTR_ERR(rst_high);
		st7539i_LOG("Cannot find st7539i rst_high!\n");
		return ret;
	}
	rst_low = pinctrl_lookup_state(pinctrl, "rst_low");
	if (IS_ERR(rst_low))
	{
		ret = PTR_ERR(rst_low);
		st7539i_LOG("Cannot find st7539i rst_low!\n");
		return ret;
	}
	
	if(i2c_add_driver(&st7539i_i2c_driver))
	{
		printk("[st7539i_lcd_probe] add driver error\n");
		return -1;
	}
	
	//create misc deivce
	ret = misc_register(&st7539i_lcd_misc_device);
	if(ret)
	{
		st7539i_LOG("misc device register failed\n");
		return ret;
	}
	return 0;
}

#ifdef CONFIG_OF
static const struct of_device_id lcd_of_match[] = {
	{ .compatible = "mediatek,i2c_lcm", },
	{},
};
#endif

static struct platform_driver st7539i_lcd_driver =
{
	.driver	 = 
	{
		.name	 = st7539i_NAME,
		.owner	 = THIS_MODULE,
	#ifdef CONFIG_OF
		.of_match_table = lcd_of_match,
	#endif
	},
	.probe 	 = st7539i_lcd_probe,
	.remove	 = st7539i_lcd_remove,
	.shutdown = st7539i_lcd_shutdown
};

/* called when loaded into kernel */
static int __init st7539i_lcd_init(void)
{
	
	st7539i_LOG("driver init start\n");
	
	if(platform_driver_register(&st7539i_lcd_driver) < 0)
	{
		printk("platform register st7539i driver failed\n");
		return -1;
	}
	
	st7539i_LOG("st7539i driver init end.\n");
	
	return 0;
}
 
/* should never be called */
static void __exit st7539i_lcd_exit(void)
{
	platform_driver_unregister(&st7539i_lcd_driver);
}

module_init(st7539i_lcd_init);
module_exit(st7539i_lcd_exit);

/*----------------------------------------------------------------------------*/

MODULE_AUTHOR("juwei.yang@wheatek.com");
MODULE_DESCRIPTION("motek st7539i respiration lamp ic driver");
MODULE_LICENSE("GPL");
