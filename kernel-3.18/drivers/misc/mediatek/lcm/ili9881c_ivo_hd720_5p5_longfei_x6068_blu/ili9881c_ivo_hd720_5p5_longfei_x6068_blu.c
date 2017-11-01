#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
//#include <cust_adc.h>
#define MIN_VOLTAGE (300)
#define MAX_VOLTAGE (500)
#define LCM_ID (0x9881)

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0XFFE
#define REGFLAG_END_OF_TABLE      							0xFFF   // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


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
#define AUXADC_LCM_VOLTAGE_CHANNEL 12

#ifndef BUILD_LK
extern atomic_t ESDCheck_byCPU;
#endif

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {

	//Page 3	
	{0xFF,3,{0x98,0x81,0x03}},
	
	
	{0x01,1,{0x00}},
	{0x02,1,{0x00}},
	{0x03,1,{0x73}},
	{0x04,1,{0x73}},
	{0x05,1,{0x00}},//00 for 0D
	{0x06,1,{0x06}},
	{0x07,1,{0x02}},
	{0x08,1,{0x00}},
	{0x09,1,{0x01}},
	{0x0a,1,{0x01}},
	{0x0b,1,{0x01}},
	{0x0c,1,{0x01}},
	{0x0d,1,{0x01}},
	{0x0e,1,{0x01}},
	{0x0f,1,{0x32}},//0x32
	{0x10,1,{0x32}},//0x32
	{0x11,1,{0x00}},
	{0x12,1,{0x00}},
	{0x13,1,{0x01}},
	{0x14,1,{0x00}},
	{0x15,1,{0x00}},
	{0x16,1,{0x00}}, 
	{0x17,1,{0x00}},
	{0x18,1,{0x00}},
	{0x19,1,{0x00}},
	{0x1a,1,{0x00}},
	{0x1b,1,{0x00}},
	{0x1c,1,{0x00}},
	{0x1d,1,{0x00}},
	{0x1e,1,{0xC0}},
	{0x1f,1,{0x80}},
	{0x20,1,{0x04}},//0x03
	{0x21,1,{0x03}},//0x04
	{0x22,1,{0x00}},
	{0x23,1,{0x00}},
	{0x24,1,{0x00}},
	{0x25,1,{0x00}},
	{0x26,1,{0x00}},
	{0x27,1,{0x00}},
	{0x28,1,{0x33}},
	{0x29,1,{0x02}},//0x02
	{0x2a,1,{0x00}},
	{0x2b,1,{0x00}},
	{0x2c,1,{0x00}},
	{0x2d,1,{0x00}},
	{0x2e,1,{0x00}},
	{0x2f,1,{0x00}},
	{0x30,1,{0x00}},
	{0x31,1,{0x00}},
	{0x32,1,{0x00}},
	{0x33,1,{0x00}},  
	{0x34,1,{0x03}},
	{0x35,1,{0x00}},
	{0x36,1,{0x03}},
	{0x37,1,{0x00}},
	{0x38,1,{0x00}},
	{0x39,1,{0x00}},
	{0x3a,1,{0x00}},//00 
	{0x3b,1,{0x00}},//00 
	{0x3c,1,{0x00}},
	{0x3d,1,{0x00}},
	{0x3e,1,{0x00}},
	{0x3f,1,{0x00}},
	{0x40,1,{0x00}},
	{0x41,1,{0x00}},
	{0x42,1,{0x00}},
	{0x43,1,{0x00}},
	{0x44,1,{0x00}},
	
	
	//GIP_2
	{0x50,1,{0x01}},
	{0x51,1,{0x23}},
	{0x52,1,{0x45}},
	{0x53,1,{0x67}},
	{0x54,1,{0x89}},
	{0x55,1,{0xab}},
	{0x56,1,{0x01}},
	{0x57,1,{0x23}},
	{0x58,1,{0x45}},
	{0x59,1,{0x67}},
	{0x5a,1,{0x89}},
	{0x5b,1,{0xab}},
	{0x5c,1,{0xcd}},
	{0x5d,1,{0xef}},
	
	//GIP_3
	{0x5e,1,{0x10}},
	{0x5f,1,{0x09}},
	{0x60,1,{0x08}},
	{0x61,1,{0x0F}},
	{0x62,1,{0x0E}},
	{0x63,1,{0x0D}},
	{0x64,1,{0x0C}},
	{0x65,1,{0x02}},
	{0x66,1,{0x02}},
	{0x67,1,{0x02}},
	{0x68,1,{0x02}},
	{0x69,1,{0x02}},
	{0x6a,1,{0x02}},
	{0x6b,1,{0x02}},
	{0x6c,1,{0x02}},
	{0x6d,1,{0x02}},
	{0x6e,1,{0x02}},
	{0x6f,1,{0x02}},
	{0x70,1,{0x02}},
	{0x71,1,{0x06}},
	{0x72,1,{0x07}},
	{0x73,1,{0x02}},
	{0x74,1,{0x02}},
	{0x75,1,{0x06}},
	{0x76,1,{0x07}},
	{0x77,1,{0x0E}},
	{0x78,1,{0x0F}},
	{0x79,1,{0x0C}},
	{0x7a,1,{0x0D}},
	{0x7b,1,{0x02}},
	{0x7c,1,{0x02}},
	{0x7d,1,{0x02}},
	{0x7e,1,{0x02}},
	{0x7f,1,{0x02}},
	{0x80,1,{0x02}},
	{0x81,1,{0x02}},
	{0x82,1,{0x02}},
	{0x83,1,{0x02}},
	{0x84,1,{0x02}},
	{0x85,1,{0x02}},
	{0x86,1,{0x02}},
	{0x87,1,{0x09}},
	{0x88,1,{0x08}},
	{0x89,1,{0x02}},
	{0x8A,1,{0x02}},
	
	//CMD_Page 4
	{0xFF,3,{0x98,0x81,0x04}},
	
	
	
	
	{0x6C,1,{0x15}},//Set VCORE voltage =1.5V
	{0x6E,1,{0x2B}},//di_pwr_reg=0 for power mode 2A //VGH clamp 15V
	{0x6F,1,{0x35}},//0x53 57// reg vcl + pumping ratio VGH=3.5x VGL=-2.5x
	//SPI_WriteData(0x3A);SPI_WriteData(0xA4);//for 0D¨¨£¤¦Ì?                
	{0x8D,1,{0x1A}},//VGL clamp -11V
	{0x87,1,{0xBA}},                              
	{0x26,1,{0x76}},            
	{0xB2,1,{0xD1}},

	//{0x00,1,{0x80}},
	{0x31,1,{0x75}},//new add forV5 
	{0xB5,1,{0x07}},//new add forV5        
	{0x5C,1,{0x83}},//new add forV5 
	{0x6A,1,{0xFC}},//new add forV5 
	//CMD_Page 1
	{0xFF,3,{0x98,0x81,0x01}},
	
	{0x22,1,{0x0A}},//0x08 3B  0A forV5 //BGR);SPI_WriteData(0x SS);
	{0x31,1,{0x00}},//Zig-Zag inversion Type 3 
	{0x53,1,{0x26}},//VCOM1	36 12-10DT??
	{0x55,1,{0x50}},//VCOM2	50
	{0x50,1,{0x95}},// 960X98 0xb7 0xbf	12-10DT??	//VREG1OUT=5V  BF
	{0x51,1,{0x90}},//960x98 0xb7 0xbf	12-10DT??//VREG2OUT=-5V  BF
	{0x60,1,{0x18}},//0x14 0x06 forV5              //SDT
	//{0x61,1,{0x00}},
	//{0x62,1,{0x2f}},
	
	//Page 1
	
	
	
	
	{0xFF,3,{0x98,0x81,0x01}},
	
	
	{0xA0,1,{0x00}},		//VP255	Gamma P
	{0xA1,1,{0x10}},               //VP251        
	{0xA2,1,{0x1D}},              //VP247        
	{0xA3,1,{0x11}},               //VP243        
	{0xA4,1,{0x14}},               //VP239        
	{0xA5,1,{0x27}},               //VP231        
	{0xA6,1,{0x1C}},                //VP203    
	{0xA7,1,{0x1E}},      
	{0xA8,1,{0x77}},               //VP175        
	{0xA9,1,{0x1C}},               //VP144 19       
	{0xAA,1,{0x28}},               //VP111 26       
	{0xAB,1,{0x67}},               //VP80         
	{0xAC,1,{0x1A}},               //VP52         
	{0xAD,1,{0x19}},               //VP36         
	{0xAE,1,{0x4D}},               //VP24         
	{0xAF,1,{0x23}},               //VP16         
	{0xB0,1,{0x24}},               //VP12         
	{0xB1,1,{0x58}},               //VP8          
	{0xB2,1,{0x66}},               //VP4          
	{0xB3,1,{0x35}},               //VP0          
	      
	{0xC0,1,{0x00}},		//VN255 GAMMA N
	{0xC1,1,{0x10}},               //VN251        
	{0xC2,1,{0x1D}},               //VN247        
	{0xC3,1,{0x10}},               //VN243        
	{0xC4,1,{0x14}},               //VN239        
	{0xC5,1,{0x27}},               //VN231        
	{0xC6,1,{0x1D}},               //VN219        
	{0xC7,1,{0x1E}},               //VN203        
	{0xC8,1,{0x76}},               //VN175        
	{0xC9,1,{0x1B}},               //VN144  1E      
	{0xCA,1,{0x28}},               //VN111  2A       
	{0xCB,1,{0x67}},               //VN80         
	{0xCC,1,{0x1A}},               //VN52         
	{0xCD,1,{0x19}},               //VN36         
	{0xCE,1,{0x4D}},               //VN24         
	{0xCF,1,{0x23}},               //VN16         
	{0xD0,1,{0x24}},               //VN12         
	{0xD1,1,{0x58}},              //VN8          
	{0xD2,1,{0x66}},              //VN4          
	{0xD3,1,{0x30}},               //VN0
	
	//Page 0
	
	{0xFF,3,{0x98,0x81,0x00}},
	
	
	
	
//	{0x36,1,{0x08}},//00?y¨¦¡§¡ê? 03¡¤¡ä¨¦¡§
	{0x35,1,{0x00}}, 

	
	{0x11,1,{0x00}},
	{REGFLAG_DELAY,120,{}},	
	{0x29,1,{0x00}},
	{REGFLAG_DELAY,20,{}},	
	

	{REGFLAG_END_OF_TABLE, 0x00, {}}


};

#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	
    // Sleep Out
    {0x11, 0, {}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 0, {}},
	{REGFLAG_DELAY, 10, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0xFF,3,{0x98,0x81,0x01}},
	{0x53,1,{0x10}},
	{0xB3,1,{0x3F}}, 
	{0xD3,1,{0x3F}}, 
	{0xFF,3,{0x98,0x81,0x04}},
	{0x2D,1,{0x02}},
	{0x2F,1,{0x01}},
	{REGFLAG_DELAY, 100, {}}, 
	{0x2F,1,{0x00}}, 
	{0xFF,3,{0x98,0x81,0x00}},
	{0x28, 0, {}},
	{REGFLAG_DELAY, 20, {}},
	// Sleep Mode On
	{0x10, 0, {}},
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

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DSI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    //enable tearing-free
    params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
    params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
    params->dsi.mode   = SYNC_EVENT_VDO_MODE;//SYNC_EVENT_VDO_MODE;//SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;

    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM				= LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    // Highly depends on LCD driver capability.
    // Not support in MT6573
    params->dsi.packet_size=256;
    // Video mode setting		
    params->dsi.intermediat_buffer_num = 2;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active                = 8;
    params->dsi.vertical_backporch                  = 18;
    params->dsi.vertical_frontporch                 = 20;
    params->dsi.vertical_active_line                = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active              = 20;
    params->dsi.horizontal_backporch                = 80;
    params->dsi.horizontal_frontporch               = 100;
    params->dsi.horizontal_active_pixel             = FRAME_WIDTH;
    //params->dsi.HS_TRAIL                            = 15;      

#ifndef CONFIG_FPGA_EARLY_PORTING
	params->dsi.PLL_CLOCK = 215; //this value must be in MTK suggested table
#else
	params->dsi.pll_div1 = 0;
	params->dsi.pll_div2 = 0;
	params->dsi.fbk_div = 0x1;
#endif

	params->dsi.cont_clock=1;
	params->dsi.ssc_disable							= 1;
	params->dsi.ssc_range							= 4;
	
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;
	
	// A: XLLLB-253 daizhiyi 20160627 {
	params->physical_width = 68;
	params->physical_height = 121;
	// A: }
}


static void lcm_init(void)
{
	//mt_set_gpio_out(GPIO_LCM_PWR_EN,GPIO_OUT_ONE);
	//mt_set_gpio_out(GPIO_LCM_PWR2_EN,GPIO_OUT_ONE);
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

#ifdef LCM_DSI_CMD_MODE
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
	unsigned int id=0;
	unsigned char buffer[8];
	unsigned int array[16];  
	//mt_set_gpio_out(GPIO_LCM_PWR_EN,GPIO_OUT_ONE);
	//mt_set_gpio_out(GPIO_LCM_PWR2_EN,GPIO_OUT_ONE);
	
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	array[0]=0x00043902;
	array[1]=0x018198ff;
	dsi_set_cmdq(array, 2, 1);
	
	MDELAY(10);
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x00, buffer, 2);
	
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x01, &buffer[1], 2);
	
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x02, &buffer[2], 2);
	
	id = buffer[0]<<8|buffer[1];
	
	
#ifdef BUILD_LK
	printf("%s, LK  debug:id = 0x%08x\n", __func__, id);
#else
	printk("%s, kernel debug:id = 0x%08x\n", __func__, id);
#endif
	//mt_set_gpio_out(GPIO_LCM_PWR_EN,GPIO_OUT_ZERO);
	//mt_set_gpio_out(GPIO_LCM_PWR2_EN,GPIO_OUT_ZERO);
    if(id == LCM_ID)
    	return 1;
    else
        return 0;

}

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

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
	unsigned int id=0;
	unsigned char buf[8];
	unsigned int array[16];  
	
	array[0]=0x00043902;
	array[1]=0x018198ff;
	dsi_set_cmdq(array, 2, 1);
	
	MDELAY(10);
	
	atomic_set(&ESDCheck_byCPU,1);
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x00, buf, 2);

	
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x01, &buf[1], 2);
	
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x02, &buf[2], 2);
	atomic_set(&ESDCheck_byCPU,0);
	
	id = buf[0]<<8|buf[1];
	
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

LCM_DRIVER ili9881c_ivo_hd720_5p5_longfei_x6068_blu_lcm_drv = 
{
	.name           = "ili9881c_ivo_hd720_5p5_longfei_x6068_blu",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = rgk_lcm_compare_id,
	.ata_check      = lcm_ata_check,
	//.set_backlight  = lcm_setbacklight,
};

