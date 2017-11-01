#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
//#include <cust_adc.h>
#define MIN_VOLTAGE (100)
#define MAX_VOLTAGE (300)
#define LCM_ID (0x94)

#define FRAME_WIDTH  				(720)
#define FRAME_HEIGHT 				(1280)

#define REGFLAG_DELAY             							0XFFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE			0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) 					(lcm_util.udelay(n))
#define MDELAY(n) 					(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                  lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg                                            lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)               lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)       

#define AUXADC_LCM_VOLTAGE_CHANNEL                          12
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

#ifndef BUILD_LK
extern atomic_t ESDCheck_byCPU;
#endif

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {

	{0XB9, 3 ,{0xFF ,0x83 ,0x94}},
	{0XB1, 10,{0x50 ,0x15 ,0x75 ,0x09 ,0x32 ,0x44 ,0x71 ,0x31 ,0x55 ,0x2F}},
	{0XBA, 6 ,{0x63 ,0x03 ,0x68 ,0x6B ,0xB2 ,0xC0}},
	{0XD2, 1 ,{0x88}},
	{0XB2, 5 ,{0x00 ,0x80 ,0x64 ,0x10 ,0x07}},
	{0XB4, 21,{0x01 ,0x72 ,0x01 ,0x72 ,0x01 ,0x72 ,0x01 ,0x0C ,0x7E ,0x86 ,0x00 ,0x3F ,0x01 ,0x72 ,0x01 ,0x72 ,0x01 ,0x72 ,0x01 ,0x0C ,0x86}},
	{0XD3, 33,{0x00 ,0x00 ,0x07 ,0x07 ,0x40 ,0x1E ,0x08 ,0x00 ,0x32 ,0x10 ,0x08 ,0x00 ,0x08 ,0x54 ,0x15 ,0x10 ,0x05 ,0x04 ,0x02 ,0x12 ,0x10 ,
						0x05 ,0x07 ,0x23 ,0x23 ,0x0C ,0x0C ,0x27 ,0x10 ,0x07 ,0x07 ,0x10 ,0x40}},
	{0XD5, 44,{0x04 ,0x05 ,0x06 ,0x07 ,0x00 ,0x01 ,0x02 ,0x03 ,0x20 ,0x21 ,0x22 ,0x23 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x19 ,
						0x19 ,0x18 ,0x18 ,0x18 ,0x18 ,0x1B ,0x1B ,0x1A ,0x1A ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18}},
	{0XD6, 44,{0x03 ,0x02 ,0x01 ,0x00 ,0x07 ,0x06 ,0x05 ,0x04 ,0x23 ,0x22 ,0x21 ,0x20 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x58 ,0x58 ,0x18 ,
						0x18 ,0x19 ,0x19 ,0x18 ,0x18 ,0x1B ,0x1B ,0x1A ,0x1A ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18 ,0x18}},
	{0XE0, 58,{0x00 ,0x09 ,0x15 ,0x1D ,0x20 ,0x24 ,0x27 ,0x26 ,0x4F ,0x60 ,0x72 ,0x71 ,0x7A ,0x8B ,0x8F ,0x94 ,0xA0 ,0xA3 ,0x9D ,0xAC ,0xBA ,
						0x5C ,0x5C ,0x5F ,0x63 ,0x66 ,0x6B ,0x7F ,0x7F ,0x00 ,0x09 ,0x15 ,0x1C ,0x20 ,0x24 ,0x27 ,0x26 ,0x4F ,0x60 ,0x72 ,0x71 ,0x7A ,
						0x8C ,0x91 ,0x94 ,0xA0 ,0xA3 ,0x9F ,0xAC ,0xBB ,0x5D ,0x5B ,0x60 ,0x65 ,0x68 ,0x73 ,0x7F ,0x7F}},
	{0XCC, 1 ,{0x03}},
	{0XC0, 2 ,{0x1F ,0x31}},
	{0XB6, 2 ,{0x46 ,0x46}},
	{0XD4, 1 ,{0x02}},
	{0XBD, 1 ,{0x01}},
	{0XB1, 1 ,{0x00}},
	{0XBD, 1 ,{0x00}},
	{0XBF, 7 ,{0x40 ,0x81 ,0x50 ,0x00 ,0x1A ,0xFC ,0x01}},
	{0X36, 1 ,{0X02}},
	{0X11, 0 ,{0x00}},
	{REGFLAG_DELAY, 120, {}},
	{0X29, 0 ,{0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_DELAY, 10, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28,1,{0x00}},
	{REGFLAG_DELAY, 50, {}},
	// Sleep Mode On
	{0x10,1,{0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++)
	{
		unsigned cmd;
		cmd = table[i].cmd;
		
		switch (cmd)
		{
			case REGFLAG_DELAY :
				MDELAY(table[i].count);
				break;

				case REGFLAG_END_OF_TABLE :
				break;
				
			default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS * params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode 		=LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity    = LCM_POLARITY_RISING;

	#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
	#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;
	#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM		    = LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size = 230;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//2;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count = 720 * 3;
 
	params->dsi.vertical_sync_active 				= 2;//4;4
	params->dsi.vertical_backporch 					= 16;//16;//16 12
	params->dsi.vertical_frontporch 				= 9;//20;//20 18 10
	params->dsi.vertical_active_line 				= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 80;
	params->dsi.horizontal_backporch 				= 80;
	params->dsi.horizontal_frontporch 				= 20;
	params->dsi.horizontal_active_pixel 				= FRAME_WIDTH;

	// Bit rate calculation
	/*
	params->dsi.pll_div1=1;             // div1=0,1,2,3;div1_real=1,2,4,4
	params->dsi.pll_div2=0;             // div2=0,1,2,3;div2_real=1,2,4,4
	params->dsi.fbk_div =15;   	    //10.11.12/48,13/55,15/62,17/70
	*/

	params->dsi.PLL_CLOCK=215;															

	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.cont_clock=0;// 1
	params->dsi.lcm_esd_check_table[0].cmd = 0x9;//09 ,45,d9
	params->dsi.lcm_esd_check_table[0].count = 3;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80;
	params->dsi.lcm_esd_check_table[0].para_list[1] = 0x73;
	params->dsi.lcm_esd_check_table[0].para_list[2] = 0x6;//0x6 {0x35,1,{0x00}},te

	params->dsi.lcm_esd_check_table[1].cmd = 0xd9;
	params->dsi.lcm_esd_check_table[1].count = 1;
	params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
	//params->dsi.lcm_esd_check_table[1].para_list[1] = 0x00;
	//params->dsi.lcm_esd_check_table[1].para_list[2] = 0x45;

	params->dsi.lcm_esd_check_table[2].cmd = 0x45;
	params->dsi.lcm_esd_check_table[2].count = 2;
	params->dsi.lcm_esd_check_table[2].para_list[0] = 0x5;
	params->dsi.lcm_esd_check_table[2].para_list[1] = 0x19;

	// A: XLLLB-253 daizhiyi 20160627 {
	params->physical_width = 75;
	params->physical_height = 133;
	// A: }
}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10); 
	SET_RESET_PIN(1);
	MDELAY(120);
	
	//mt_set_gpio_out(GPIO_LCM_PWR_EN,GPIO_OUT_ZERO);
	//mt_set_gpio_out(GPIO_LCM_PWR2_EN,GPIO_OUT_ZERO);
}


static void lcm_resume(void)
{
	lcm_init();
}

#if LCM_DSI_CMD_MODE
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;

	dsi_set_cmdq(&data_array, 7, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{

	unsigned int id = 0;

	unsigned char buffer[3];
	unsigned int data_array[16];
	
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50);
	
	data_array[0]=0x00043902;
	data_array[1]=0x9483FFB9;// page enable
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);
	
	data_array[0]=0x00023902;
	data_array[1]=0x000013ba;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);
	
	data_array[0] = 0x00023700;// return byte number
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
	
	read_reg_v2(0xF4, buffer, 2);
	id = buffer[0];
	
#ifdef BUILD_LK
	printf("%s, LK  debug:id = 0x%08x\n", __func__, id);
#else
	printk("%s, kernel debug:id = 0x%08x\n", __func__, id);
#endif

	return (LCM_ID == id) ? 1 : 0;
}


static unsigned int rgk_lcm_compare_id(void)
{
    int data[4] = {0,0,0,0};
    int lcm_vol = 0;

#ifdef AUXADC_LCM_VOLTAGE_CHANNEL
	int res = 0;
    int rawdata = 0;
    res = IMM_GetOneChannelValue(AUXADC_LCM_VOLTAGE_CHANNEL,data,&rawdata);
    if(res < 0)
    { 
	#ifdef BUILD_LK
	printf("[adc_uboot]: get data error\n");
	#endif
	return 0;
		   
    }
#endif

    lcm_vol = data[0]*1000+data[1]*10;

#ifdef BUILD_LK
	printf("[adc_uboot]: lcm_vol= %d\n",lcm_vol);
#else
	printk("[adc_kernel]: lcm_vol= 0x%x\n",lcm_vol);
#endif
    if (lcm_vol>=MIN_VOLTAGE &&lcm_vol <= MAX_VOLTAGE && lcm_compare_id())
    {
		return 1;
    }

    return 0;

}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
	unsigned int id=0;
	unsigned char buf[8];
	unsigned int array[16];  
	
	array[0]=0x00043902;
	array[1]=0x9483FFB9;// page enable
	dsi_set_cmdq(array, 2, 1);
	MDELAY(10);
	
	array[0]=0x00023902;
	array[1]=0x000013ba;
	dsi_set_cmdq(array, 2, 1);
	MDELAY(10);
	
	array[0] = 0x00023700;// return byte number
	dsi_set_cmdq(array, 1, 1);
	MDELAY(10);
	atomic_set(&ESDCheck_byCPU,1);
	read_reg_v2(0xF4, buf, 2);
	atomic_set(&ESDCheck_byCPU,0);
	id = buf[0];
	
#ifdef BUILD_LK
	printf("%s, LK  debug:id = 0x%08x\n", __func__, id);
#else
	printk("%s, kernel debug:id = 0x%08x\n", __func__, id);
#endif

    if(id == LCM_ID)
    	return 1;
    else
        return 0;

#else
	return 0;
#endif
}

LCM_DRIVER hx8394f_hc_hd_6p0_holitech_x6068_blu_lcm_drv = 
{
    .name              = "hx8394f_hc_hd_6p0_holitech_x6068_blu",
	.set_util_funcs    = lcm_set_util_funcs,
	.get_params        = lcm_get_params,
	.init              = lcm_init,
	.suspend           = lcm_suspend,
	.resume            = lcm_resume,
	.compare_id        = rgk_lcm_compare_id,
	.ata_check        = lcm_ata_check,
#if (LCM_DSI_CMD_MODE)
	.update = lcm_update,
#endif
};
