#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

#ifndef BUILD_LK
#include <mt-plat/mt_gpio.h>
#else
#include <platform/mt_gpio.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define MIN_VOLTAGE 					(0)
#define MAX_VOLTAGE 					(100)
#define LCM_ID 							(0x3350)   //(0x13)

#define FRAME_WIDTH  					(720)
#define FRAME_HEIGHT 					(1280)

#define REGFLAG_DELAY      				0XFFE
#define REGFLAG_END_OF_TABLE      		0xFFF   // END OF REGISTERS MARKER


//#define GPIO_LCM_RST 146

#define AUXADC_LCM_VOLTAGE_CHANNEL	12

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

#ifndef BUILD_LK
extern int lcm_rst_gpio_select_state(int s);
#endif

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)   			(lcm_util.set_reset_pin((v)))

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

extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
#define LCM_DSI_CMD_MODE 0


#ifndef BUILD_LK
extern atomic_t ESDCheck_byCPU;
#endif

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	{0xB0,1,{0x04}},
	{0xD6,1,{0x01}},
	{0xB3,6,{0x14,0x00,0x00,0x00,0x00,0x00}},
	{0xB4,2,{0x0C,0x00}},
	{0xB6,3,{0x49,0xBB,0x00}},
	{0xC1,35,{0x04,0x66,0x00,0xFF,0x93,0x2F,0x0A,0xF8,0xFF,0xFF,0x7F,0xD4,0x62,0x6C,0xA9,0xF3,0xFF,0xFF,0x1F,0x94,0x1F,0xFE,0xF8,0x00,0x00,0x00,0x00,0x00,0x62,0x00,0x02,0x22,0x00,0x01,0x11}},
	{0xC2,8,{0x31,0xF5,0x80,0x0E,0x07,0x00,0x00,0x08}},
	//{0xC2,8,{0x30,0xF7,0x83,0x0C,0x08,0x00,0x00,0x08}},
	{0xC4,11,{0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03}},
	{0xC6,22,{0xC8,0x01,0xB4,0x01,0x9C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x2C,0x09,0xC8}},
	{0xC7,30,{0x0A,0x20,0x29,0x33,0x41,0x4F,0x5A,0x69,0x4E,0x55,0x60,0x6B,0x72,0x77,0x7D,0x0A,0x1F,0x28,0x33,0x41,0x4F,0x59,0x69,0x4D,0x55,0x60,0x6B,0x71,0x77,0x7D}},
	{0xCB,15,{0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE0,0x00,0x00}},
	{0xCC,1,{0x08}},
	{0xD0,10,{0x11,0x00,0x00,0x55,0xCC,0x40,0x19,0x19,0x09,0x00}},
	{0xD1,4,{0x00,0x48,0x16,0x0F}},
	{0xD3,26,{0x1B,0x33,0xBB,0xBB,0xB3,0x33,0x33,0x33,0x33,0x00,0x01,0x00,0x00,0xD8,0xA0,0x0B,0x3F,0x3F,0x33,0x33,0x72,0x12,0x8A,0x57,0x3D,0xBC}},
	{0xD5,7,{0x06,0x00,0x00,0x01,0x19,0x01,0x19}},
	{0x29, 0,{0x00}},
	{REGFLAG_DELAY, 30, {}},
	{0x11, 0,{0x00}},
	{REGFLAG_DELAY, 150, {}},

};

#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11,1,{0x00}},
	{REGFLAG_DELAY, 120, {}},
	// Display ON
	{0x29,1,{0x00}},
	{REGFLAG_DELAY, 10, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

#if 0
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28,1,{0x00}},
	{REGFLAG_DELAY, 50, {}},
	// Sleep Mode On
	{0x10,1,{0x00}},
	{REGFLAG_DELAY, 120, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif


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
	params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity    	= LCM_POLARITY_RISING;


	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;


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
	params->dsi.packet_size = 256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//2;

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count = 720 * 3;


	params->dsi.vertical_sync_active				= 2;//1
	params->dsi.vertical_backporch					= 13;//2
	params->dsi.vertical_frontporch					= 8;//2
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 8;//12
	params->dsi.horizontal_backporch				= 60;//72
	params->dsi.horizontal_frontporch				= 140;//120
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;


	params->dsi.PLL_CLOCK=220;															


	params->dsi.cont_clock=1;
	params->dsi.ssc_disable							= 0;
	params->dsi.ssc_range							= 4;

	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd          	= 0x0a;
	params->dsi.lcm_esd_check_table[0].count        	= 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] 	= 0x9c;

    // A: XLLLB-253 daizhiyi 20160627 {
    params->physical_width = 75;
    params->physical_height = 133;
    // A: }
}

static void lcm_init(void)
{
#ifndef BUILD_LK	
    lcm_rst_gpio_select_state(1);
    MDELAY(10);
    lcm_rst_gpio_select_state(0);
    MDELAY(10);
    lcm_rst_gpio_select_state(1);
    MDELAY(120);
#else
    SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
#endif
	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	unsigned int data_array[16];
#ifndef BUILD_LK	
	lcm_rst_gpio_select_state(1);
    MDELAY(10);
    lcm_rst_gpio_select_state(0);
    MDELAY(20);
    lcm_rst_gpio_select_state(1);
    MDELAY(20);
#else
    SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(20);
#endif

	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);

	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
    data_array[0]=0x00022902;
    data_array[1]=0x000004b0;//cmd mode
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(10);
    data_array[0]=0x00022902;
    data_array[1]=0x000001b1;//cmd mode
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(60);
#ifndef BUILD_LK    
    lcm_rst_gpio_select_state(0);
    MDELAY(20);
#else
    SET_RESET_PIN(0);
    MDELAY(20);
#endif
}


static void lcm_resume(void)
{
	lcm_init();
}

#if  LCM_DSI_CMD_MODE
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
	unsigned char buffer[8];
	unsigned int data_array[16];

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50);

	data_array[0] = 0x00053700;// return byte number
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);

	read_reg_v2(0xBF, buffer, 5);
	id=(buffer[2] << 8 )| buffer[3];
#ifdef BUILD_LK
	printf("%s, LK  debug:id = 0x%08x\n", __func__, id);
#else
	printk("%s, kernel debug:id = 0x%08x\n", __func__, id);
#endif

	return (LCM_ID == id) ? 1 : 0;
}

#if 1
static unsigned int rgk_lcm_compare_id(void)
{
    int data[4] = {0,0,0,0};
    int res = 0;
    int rawdata = 0;
    int lcm_vol = 0;

#ifdef AUXADC_LCM_VOLTAGE_CHANNEL
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
#endif

#if 1
static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
	unsigned int id=0;
	unsigned char buf[8];
	unsigned int array[16];  

	array[0] = 0x00053700;// return byte number
	dsi_set_cmdq(array, 1, 1);
	MDELAY(10);

	atomic_set(&ESDCheck_byCPU,1);
	read_reg_v2(0xBF, buf, 5);
	atomic_set(&ESDCheck_byCPU,0);
	//id = buf[2];
	id=(buf[2] << 8 )| buf[3];

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
#endif

LCM_DRIVER r63350_jdi_hd_6p0_eq_x6068_blu_lcm_drv = 
{
	.name              	= "r63350_jdi_hd_6p0_eq_x6068_blu",
	.set_util_funcs    	= lcm_set_util_funcs,
	.get_params        	= lcm_get_params,
	.init  				= lcm_init,
	.suspend           	= lcm_suspend,
	.resume            	= lcm_resume,
	.compare_id        	= rgk_lcm_compare_id,
	.ata_check         	= lcm_ata_check,
#if 0
	.update = lcm_update,
#endif
};
