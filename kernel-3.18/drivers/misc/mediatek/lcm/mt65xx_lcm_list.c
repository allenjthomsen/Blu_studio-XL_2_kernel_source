#include "mt65xx_lcm_list.h"
#include <lcm_drv.h>
#ifdef BUILD_LK
#include <platform/disp_drv_platform.h>
#else
#include <linux/delay.h>
/* #include <mach/mt_gpio.h> */
#endif

/* used to identify float ID PIN status */
#define LCD_HW_ID_STATUS_LOW      0
#define LCD_HW_ID_STATUS_HIGH     1
#define LCD_HW_ID_STATUS_FLOAT 0x02
#define LCD_HW_ID_STATUS_ERROR  0x03

#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  dprintf(CRITICAL, fmt)
#else
#define LCD_DEBUG(fmt, args...)  pr_debug("[KERNEL/LCM]"fmt, ##args)
#endif

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
extern LCM_DRIVER lcm_common_drv;
#else
//----------wtk lcm list start----------
extern LCM_DRIVER r63311_auo_fhd_5p5_ykl_m6010_public_lcm_drv;
extern LCM_DRIVER ili9806e_boe_fwvga_4p5_eq_x2605_otd_lcm_drv;
extern LCM_DRIVER ili9806e_ivo_fwvga_4p5_jd_x2605_otd_lcm_drv;
extern LCM_DRIVER rm68172_boe_fwvga_4p5_linglong_x3619_public_lcm_drv;
extern LCM_DRIVER rm68172_boe_fwvga_4p5_eq_x3619_ld_lcm_drv;
extern LCM_DRIVER jd9161_boe_fwvga_4p5_cm_x3619_public_lcm_drv;
extern LCM_DRIVER rm68200_hd720_dsi_vdo_linglong_x5623_lcm_drv;
extern LCM_DRIVER ili9881c_boe_hd720_5p0_guansheng_x5623_lcm_drv;
extern LCM_DRIVER otm1285a_ctc_hd720_4p7_blj_x5613_lcm_drv;
extern LCM_DRIVER hx8399c_auo_fhd_5p7_jd_m6010_lcm_drv;
extern LCM_DRIVER nt35521_auo_hd720_5p0_huashi_x5623_cq_lcm_drv;
extern LCM_DRIVER otm9605a_cmi_qhd_5p0_chuanma_x5622_lcm_drv;
extern LCM_DRIVER hx8394f_cmi_hd720_5p5_belij_x2605_dg_lcm_drv;
extern LCM_DRIVER r61318a_boe_hd720_5p5_kelai_x2605_dg_lcm_drv;
extern LCM_DRIVER nt35521_cmi_hd720_5p5_xld_x2605_dg_lcm_drv;
extern LCM_DRIVER ili9881_cpt_hd720_5p5_xingliangda_x2605_dg_lcm_drv;
extern LCM_DRIVER rm68200ga1_tm_hd720_5p0_linglong_x5623_cq_lcm_drv;
extern LCM_DRIVER rm68200_tm_hd720_5p0_linglong_x5623_cq_lcm_drv;
extern LCM_DRIVER rm68172_boe_wvga_4p0_belij_x2605_otd_lcm_drv;
extern LCM_DRIVER jd9161_boe_wvga_4p0_jd_x2605_hair_lcm_drv;
extern LCM_DRIVER nt35521sh_auo_hd720_nisan_x5623_cq_lcm_drv;
extern LCM_DRIVER rm68200_tm_hd720_5p5_linglong_x5623_lcm_drv;
extern LCM_DRIVER nt35521sh_auo_hd720_5p5_nisan_x5623_lcm_drv;
extern LCM_DRIVER hx8394f_auo_hd720_5p0_cm_x2605_jk_lcm_drv;
extern LCM_DRIVER nt35521_cmo_hd720_5p0_sh_x2605_jk_lcm_drv;
extern LCM_DRIVER ili9881c_ivo_hd720_5p0_jingtai_x5623_intex_lcm_drv;
extern LCM_DRIVER otm1285a_auo_hd720_5p0_eq_x5623_intex_lcm_drv;
extern LCM_DRIVER otm1287_boe_hd720_tzd_x5623_nflh_lcm_drv;
extern LCM_DRIVER jd9161_ctc_wvga_3p97_tianyifu_x2605_lcm_drv;
extern LCM_DRIVER nt35596h_auo_fhd_belij_x5618_yp_lcm_drv;
extern LCM_DRIVER otm9605a_cmi_qhd_5p0_chuanma_otd_x5613_lcm_drv;
extern LCM_DRIVER r63311_fhd_jdi5p0_chuanma_otd_x5601_lcm_drv;
extern LCM_DRIVER hx8394f_hd720_dsi_vdo_helitai_boe_x5057_lcm_drv;
extern LCM_DRIVER hx8394f_hd720_dsi_vdo_helitai_hsd_x6069_lcm_drv;
extern LCM_DRIVER hx8394d_cmi_hd720_5p0_blj_x5602_lcm_drv;
extern LCM_DRIVER rm68200_hd720_auo_5p0_helitai_x5057_otd_lcm_drv;
extern LCM_DRIVER rm68200_cmi_hd720_5p0_haifei_x5623_gyt_lcm_drv;
extern LCM_DRIVER ili9881c_auo_hd720_5p5_horngjaan_x5057_otd_lcm_drv;
extern LCM_DRIVER nt35521_cmi_hd720_4p5_kld_x2605_chk_lcm_drv;
extern LCM_DRIVER ili9881c_auo_hd720_4p5_hz_x5618_chk_lcm_drv;
extern LCM_DRIVER rm68200_auo_hd720_4p5_kld_x5618_chk_lcm_drv;
extern LCM_DRIVER ili9885_auo_fhd_nisan_x5618_yp_lcm_drv;
extern LCM_DRIVER ili9881c_auo_hd720_5p0_horngjaan_x5057_otd_lcm_drv;
extern LCM_DRIVER otm1285_boe_hd720_xld_x2605_jk_lcm_drv;
extern LCM_DRIVER nt35521sh_auo_hd720_5p5_nisan_x5623_cq_lcm_drv;
extern LCM_DRIVER otm9605_qhd_cmi5p0_chuanma_otd_x5618_lcm_drv;
extern LCM_DRIVER otm8019a_fwe_fwvga_5p0_eq_x2605_public_lcm_drv;
extern LCM_DRIVER r63350_jdi_hd_6p0_eq_x6068_blu_lcm_drv;
extern LCM_DRIVER hx8394f_hc_hd_6p0_holitech_x6068_blu_lcm_drv;
extern LCM_DRIVER hx8394f_hc_hd720_5p0_holitech_x6068_blu_lcm_drv;
extern LCM_DRIVER ili9881_auo_hd720_5p5_xzx_x5057_otd_lcm_drv;
extern LCM_DRIVER otm1285a_boe_hd720_5p0_eq_x5623_public_lcm_drv;
extern LCM_DRIVER jd9365_cmi_hd720_5p5_huangshan_x5623_jk_lcm_drv;//daili add 6-28
extern LCM_DRIVER ili9881c_ivo_hd720_5p5_longfei_x6068_blu_lcm_drv;
extern LCM_DRIVER s6d7aa6x01_hsd_hd720_5p5_helitech_x6068_blu_lcm_drv;
extern LCM_DRIVER s6d7aa6x01_hsd_hd720_6p0_helitech_x6068_blu_lcm_drv;
//----------wtk lcm list end  ----------
#endif

LCM_DRIVER *lcm_driver_list[] = {
#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
	&lcm_common_drv,
#else
	//----------wtk lcm list start----------
#if defined(S6D7AA6X01_HSD_HD720_6P0_HELITECH_X6068_BLU)
	&s6d7aa6x01_hsd_hd720_6p0_helitech_x6068_blu_lcm_drv,
#endif	
#if defined(S6D7AA6X01_HSD_HD720_5P5_HELITECH_X6068_BLU)
	&s6d7aa6x01_hsd_hd720_5p5_helitech_x6068_blu_lcm_drv,
#endif	
#if defined(ILI9881C_IVO_HD720_5P5_LONGFEI_X6068_BLU)
	&ili9881c_ivo_hd720_5p5_longfei_x6068_blu_lcm_drv,
#endif
#if defined(HX8394F_HC_HD720_5P0_HOLITECH_X6068_BLU)
	&hx8394f_hc_hd720_5p0_holitech_x6068_blu_lcm_drv,
#endif
#if defined(HX8394F_HC_HD_6P0_HOLITECH_X6068_BLU)
	&hx8394f_hc_hd_6p0_holitech_x6068_blu_lcm_drv,
#endif
#if defined(R63350_JDI_HD_6P0_EQ_X6068_BLU)
	&r63350_jdi_hd_6p0_eq_x6068_blu_lcm_drv,
#endif
#if defined(R63311_AUO_FHD_5P5_YKL_M6010_PUBLIC)
	&r63311_auo_fhd_5p5_ykl_m6010_public_lcm_drv,
#endif

#if defined(ILI9806E_BOE_FWVGA_4P5_EQ_X2605_OTD)
	&ili9806e_boe_fwvga_4p5_eq_x2605_otd_lcm_drv,
#endif

#if defined(ILI9806E_IVO_FWVGA_4P5_JD_X2605_OTD)
	&ili9806e_ivo_fwvga_4p5_jd_x2605_otd_lcm_drv,
#endif

#if defined(RM68172_BOE_FWVGA_4P5_LINGLONG_X3619_PUBLIC)
	&rm68172_boe_fwvga_4p5_linglong_x3619_public_lcm_drv,
#endif

#if defined(RM68172_BOE_FWVGA_4P5_EQ_X3619_LD)
	&rm68172_boe_fwvga_4p5_eq_x3619_ld_lcm_drv,
#endif

#if defined(HX8392A_DSI_CMD_3LANE_QHD)
	&hx8392a_dsi_cmd_3lane_qhd_lcm_drv,
#endif

#if defined(JD9161_BOE_FWVGA_4P5_CM_X3619_PUBLIC)
	&jd9161_boe_fwvga_4p5_cm_x3619_public_lcm_drv,
#endif

#if defined(RM68200_HD720_DSI_VDO_LINGLONG_X5623)
	&rm68200_hd720_dsi_vdo_linglong_x5623_lcm_drv,
#endif

#if defined(ILI9881C_BOE_HD720_5P0_GUANSHENG_X5623)
	&ili9881c_boe_hd720_5p0_guansheng_x5623_lcm_drv,
#endif
#if defined(OTM1285A_CTC_HD720_4P7_BLJ_X5613)
	&otm1285a_ctc_hd720_4p7_blj_x5613_lcm_drv,
#endif

#if defined(HX8399C_AUO_FHD_5P7_JD_M6010)
	&hx8399c_auo_fhd_5p7_jd_m6010_lcm_drv,
#endif

#if defined(NT35521_AUO_HD720_5P0_HUASHI_X5623_CQ)
	&nt35521_auo_hd720_5p0_huashi_x5623_cq_lcm_drv,
#endif

#if defined(HX8394F_CMI_HD720_5P5_BELIJ_X2605_DG)
	&hx8394f_cmi_hd720_5p5_belij_x2605_dg_lcm_drv,
#endif
#if defined(R61318A_BOE_HD720_5P5_KELAI_X2605_DG)
	&r61318a_boe_hd720_5p5_kelai_x2605_dg_lcm_drv,
#endif
#if defined(NT35521_CMI_HD720_5P5_XLD_X2605_DG)
	&nt35521_cmi_hd720_5p5_xld_x2605_dg_lcm_drv,
#endif
#if defined(ILI9881_CPT_HD720_5P5_XINGLIANGDA_X2605_DG)
	&ili9881_cpt_hd720_5p5_xingliangda_x2605_dg_lcm_drv,
#endif


#if defined(OTM9605A_CMI_QHD_5P0_CHUANMA_X5622)
	&otm9605a_cmi_qhd_5p0_chuanma_x5622_lcm_drv,
#endif

#if defined(RM68200GA1_TM_HD720_5P0_LINGLONG_X5623_CQ)
	&rm68200ga1_tm_hd720_5p0_linglong_x5623_cq_lcm_drv,
#endif

#if defined(RM68200_TM_HD720_5P0_LINGLONG_X5623_CQ)
	&rm68200_tm_hd720_5p0_linglong_x5623_cq_lcm_drv,
#endif

#if defined(RM68172_BOE_WVGA_4P0_BELIJ_X2605_OTD)
	&rm68172_boe_wvga_4p0_belij_x2605_otd_lcm_drv,
#endif

#if defined(JD9161_BOE_WVGA_4P0_JD_X2605_HAIR)
	&jd9161_boe_wvga_4p0_jd_x2605_hair_lcm_drv,
#endif

#if defined(NT35521SH_AUO_HD720_NISAN_X5623_CQ)
	&nt35521sh_auo_hd720_nisan_x5623_cq_lcm_drv,
#endif

#if defined(RM68200_TM_HD720_5P5_LINGLONG_X5623)
	&rm68200_tm_hd720_5p5_linglong_x5623_lcm_drv,
#endif

#if defined(NT35521SH_AUO_HD720_5P5_NISAN_X5623)
	&nt35521sh_auo_hd720_5p5_nisan_x5623_lcm_drv,
#endif

#if defined(HX8394F_AUO_HD720_5P0_CM_X2605_JK)
	&hx8394f_auo_hd720_5p0_cm_x2605_jk_lcm_drv,
#endif

#if defined(NT35521_CMO_HD720_5P0_SH_X2605_JK)
	&nt35521_cmo_hd720_5p0_sh_x2605_jk_lcm_drv,
#endif

#if defined(ILI9881C_IVO_HD720_5P0_JINGTAI_X5623_INTEX)
	&ili9881c_ivo_hd720_5p0_jingtai_x5623_intex_lcm_drv,
#endif

#if defined(OTM1285A_AUO_HD720_5P0_EQ_X5623_INTEX)
	&otm1285a_auo_hd720_5p0_eq_x5623_intex_lcm_drv,
#endif

#if defined(OTM1287_BOE_HD720_TZD_X5623_NFLH)
	&otm1287_boe_hd720_tzd_x5623_nflh_lcm_drv,
#endif

#if defined(JD9161_CTC_WVGA_3P97_TIANYIFU_X2605)
	&jd9161_ctc_wvga_3p97_tianyifu_x2605_lcm_drv,
#endif

#if defined(NT35596H_AUO_FHD_BELIJ_X5618_YP)
	&nt35596h_auo_fhd_belij_x5618_yp_lcm_drv,
#endif

#if defined(OTM9605A_CMI_QHD_5P0_CHUANMA_OTD_X5613)
	&otm9605a_cmi_qhd_5p0_chuanma_otd_x5613_lcm_drv,
#endif
#if defined(R63311_FHD_JDI5P0_CHUANMA_OTD_X5601)
	&r63311_fhd_jdi5p0_chuanma_otd_x5601_lcm_drv,
#endif

#if defined(HX8394F_HD720_DSI_VDO_HELITAI_BOE_X5057)
	&hx8394f_hd720_dsi_vdo_helitai_boe_x5057_lcm_drv,
#endif
#if defined(HX8394F_HD720_DSI_VDO_HELITAI_HSD_X6069)
	&hx8394f_hd720_dsi_vdo_helitai_hsd_x6069_lcm_drv,
#endif

#if defined(HX8394D_CMI_HD720_5P0_BLJ_X5602)
	&hx8394d_cmi_hd720_5p0_blj_x5602_lcm_drv,
#endif

#if defined(RM68200_HD720_AUO_5P0_HELITAI_X5057_OTD)
	&rm68200_hd720_auo_5p0_helitai_x5057_otd_lcm_drv,
#endif

#if defined(RM68200_CMI_HD720_5P0_HAIFEI_X5623_GYT)
	&rm68200_cmi_hd720_5p0_haifei_x5623_gyt_lcm_drv,
#endif

#if defined(ILI9881C_AUO_HD720_5P5_HORNGJAAN_X5057_OTD)
	&ili9881c_auo_hd720_5p5_horngjaan_x5057_otd_lcm_drv,
#endif

#if defined(NT35521_CMI_HD720_4P5_KLD_X2605_CHK)
	&nt35521_cmi_hd720_4p5_kld_x2605_chk_lcm_drv,
#endif

#if defined(ILI9881C_AUO_HD720_4P5_HZ_X5618_CHK)
	&ili9881c_auo_hd720_4p5_hz_x5618_chk_lcm_drv,
#endif

#if defined(RM68200_AUO_HD720_4P5_KLD_X5618_CHK)
	&rm68200_auo_hd720_4p5_kld_x5618_chk_lcm_drv,
#endif

#if defined(ILI9885_AUO_FHD_NISAN_X5618_YP)
	&ili9885_auo_fhd_nisan_x5618_yp_lcm_drv,
#endif

#if defined(ILI9881C_AUO_HD720_5P0_HORNGJAAN_X5057_OTD)
	&ili9881c_auo_hd720_5p0_horngjaan_x5057_otd_lcm_drv,
#endif

#if defined(OTM1285_BOE_HD720_XLD_X2605_JK)
	&otm1285_boe_hd720_xld_x2605_jk_lcm_drv,
#endif

#if defined(NT35521SH_AUO_HD720_5P5_NISAN_X5623_CQ)
	&nt35521sh_auo_hd720_5p5_nisan_x5623_cq_lcm_drv,
#endif

#if defined(OTM9605_QHD_CMI5P0_CHUANMA_OTD_X5618)
	&otm9605_qhd_cmi5p0_chuanma_otd_x5618_lcm_drv,
#endif
#if defined(OTM8019A_FWE_FWVGA_5P0_EQ_X2605_PUBLIC)
	&otm8019a_fwe_fwvga_5p0_eq_x2605_public_lcm_drv,
#endif
#if defined(ILI9881_AUO_HD720_5P5_XZX_X5057_OTD)
	&ili9881_auo_hd720_5p5_xzx_x5057_otd_lcm_drv,
#endif
#if defined(OTM1285A_BOE_HD720_5P0_EQ_X5623_PUBLIC)
	&otm1285a_boe_hd720_5p0_eq_x5623_public_lcm_drv,
#endif
#if defined(JD9365_CMI_HD720_5P5_HUANGSHAN_X5623_JK)
	&jd9365_cmi_hd720_5p5_huangshan_x5623_jk_lcm_drv,    //daili add 6-28
#endif

	//----------wtk lcm list end  ----------
#endif
};

#if defined(MTK_LCM_DEVICE_TREE_SUPPORT)
unsigned char lcm_name_list[][128] = {
#if defined(HX8392A_DSI_CMD)
	"hx8392a_dsi_cmd",
#endif

#if defined(HX8392A_DSI_VDO)
	"hx8392a_vdo_cmd",
#endif

#if defined(HX8392A_DSI_CMD_FWVGA)
	"hx8392a_dsi_cmd_fwvga",
#endif

#if defined(OTM9608_QHD_DSI_CMD)
	"otm9608a_qhd_dsi_cmd",
#endif

#if defined(OTM9608_QHD_DSI_VDO)
	"otm9608a_qhd_dsi_vdo",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358)
	"r63417_fhd_dsi_cmd_truly_nt50358_drv",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_QHD)
	"r63417_fhd_dsi_cmd_truly_nt50358_qhd_drv",
#endif

#if defined(R63417_FHD_DSI_VDO_TRULY_NT50358)
	"r63417_fhd_dsi_vdo_truly_nt50358_drv",
#endif
};
#endif

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition) ? 1 : -1]

unsigned int lcm_count = sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *));
#if defined(NT35520_HD720_DSI_CMD_TM) | defined(NT35520_HD720_DSI_CMD_BOE) | \
	defined(NT35521_HD720_DSI_VDO_BOE) | defined(NT35521_HD720_DSI_VIDEO_TM)
static unsigned char lcd_id_pins_value = 0xFF;

/**
 * Function:       which_lcd_module_triple
 * Description:    read LCD ID PIN status,could identify three status:highlowfloat
 * Input:           none
 * Output:         none
 * Return:         LCD ID1|ID0 value
 * Others:
 */
unsigned char which_lcd_module_triple(void)
{
	unsigned char  high_read0 = 0;
	unsigned char  low_read0 = 0;
	unsigned char  high_read1 = 0;
	unsigned char  low_read1 = 0;
	unsigned char  lcd_id0 = 0;
	unsigned char  lcd_id1 = 0;
	unsigned char  lcd_id = 0;
	/*Solve Coverity scan warning : check return value*/
	unsigned int ret = 0;

	/*only recognise once*/
	if (0xFF != lcd_id_pins_value)
		return lcd_id_pins_value;

	/*Solve Coverity scan warning : check return value*/
	ret = mt_set_gpio_mode(GPIO_DISP_ID0_PIN, GPIO_MODE_00);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_mode fail\n");

	ret = mt_set_gpio_dir(GPIO_DISP_ID0_PIN, GPIO_DIR_IN);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_dir fail\n");

	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID0_PIN, GPIO_PULL_ENABLE);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_enable fail\n");

	ret = mt_set_gpio_mode(GPIO_DISP_ID1_PIN, GPIO_MODE_00);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_mode fail\n");

	ret = mt_set_gpio_dir(GPIO_DISP_ID1_PIN, GPIO_DIR_IN);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_dir fail\n");

	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID1_PIN, GPIO_PULL_ENABLE);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_enable fail\n");

	/*pull down ID0 ID1 PIN*/
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

	/* delay 100ms , for discharging capacitance*/
	mdelay(100);
	/* get ID0 ID1 status*/
	low_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	low_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);
	/* pull up ID0 ID1 PIN */
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");

	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");

	/* delay 100ms , for charging capacitance */
	mdelay(100);
	/* get ID0 ID1 status */
	high_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	high_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);

	if (low_read0 != high_read0) {
		/*float status , pull down ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read0) && (LCD_HW_ID_STATUS_LOW == high_read0)) {
		/*low status , pull down ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read0) && (LCD_HW_ID_STATUS_HIGH == high_read0)) {
		/*high status , pull up ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_HIGH;
	} else {
		LCD_DEBUG(" Read LCD_id0 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DISABLE);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Disbale fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_ERROR;
	}


	if (low_read1 != high_read1) {
		/*float status , pull down ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read1) && (LCD_HW_ID_STATUS_LOW == high_read1)) {
		/*low status , pull down ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read1) && (LCD_HW_ID_STATUS_HIGH == high_read1)) {
		/*high status , pull up ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_HIGH;
	} else {

		LCD_DEBUG(" Read LCD_id1 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DISABLE);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Disable fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_ERROR;
	}
#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#else
	LCD_DEBUG("which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	LCD_DEBUG("which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#endif
	lcd_id =  lcd_id0 | (lcd_id1 << 2);

#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#else
	LCD_DEBUG("which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#endif

	lcd_id_pins_value = lcd_id;
	return lcd_id;
}
#endif
