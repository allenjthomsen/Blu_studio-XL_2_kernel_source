#include <linux/types.h>
#include <linux/init.h>		/* For init/exit macros */
#include <linux/module.h>	/* For MODULE_ marcros  */
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#endif
#include <mt-plat/charging.h>
#include "ncp1854.h"



//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]start
#include <linux/wait.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/interrupt.h>
//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]end

/**********************************************************
  *
  *   [I2C Slave Setting]
  *
  *********************************************************/
#define NCP1854_SLAVE_ADDR_WRITE   0x6C
#define NCP1854_SLAVE_ADDR_READ	   0x6D

static struct i2c_client *new_client;
static const struct i2c_device_id ncp1854_i2c_id[] = { {"ncp1854", 0}, {} };

kal_bool chargin_hw_init_done = KAL_FALSE;
static int ncp1854_driver_probe(struct i2c_client *client, const struct i2c_device_id *id);

#ifdef CONFIG_OF
static const struct of_device_id ncp1854_of_match[] = {
	{.compatible = "mediatek,SWITHING_CHARGER",},
	{},
};

MODULE_DEVICE_TABLE(of, ncp1854_of_match);
#endif

static struct i2c_driver ncp1854_driver = {
	.driver = {
		   .name = "ncp1854",
#ifdef CONFIG_OF
		   .of_match_table = ncp1854_of_match,
#endif
		   },
	.probe = ncp1854_driver_probe,
	.id_table = ncp1854_i2c_id,
};

/**********************************************************
  *
  *   [Global Variable]
  *
  *********************************************************/
unsigned char ncp1854_reg[ncp1854_REG_NUM] = { 0 };

static DEFINE_MUTEX(ncp1854_i2c_access);
/**********************************************************
  *
  *   [I2C Function For Read/Write ncp1854]
  *
  *********************************************************/
int ncp1854_read_byte(unsigned char cmd, unsigned char *returnData)
{
	char cmd_buf[1] = { 0x00 };
	char readData = 0;
	int ret = 0;

	mutex_lock(&ncp1854_i2c_access);

	/* new_client->addr = ((new_client->addr) & I2C_MASK_FLAG) | I2C_WR_FLAG; */
	new_client->ext_flag =
	    ((new_client->ext_flag) & I2C_MASK_FLAG) | I2C_WR_FLAG | I2C_DIRECTION_FLAG;

	cmd_buf[0] = cmd;
	ret = i2c_master_send(new_client, &cmd_buf[0], (1 << 8 | 1));
	if (ret < 0) {
		/* new_client->addr = new_client->addr & I2C_MASK_FLAG; */
		new_client->ext_flag = 0;

		mutex_unlock(&ncp1854_i2c_access);
		return 0;
	}

	readData = cmd_buf[0];
	*returnData = readData;

	/* new_client->addr = new_client->addr & I2C_MASK_FLAG; */
	new_client->ext_flag = 0;

	mutex_unlock(&ncp1854_i2c_access);
	return 1;
}

int ncp1854_write_byte(unsigned char cmd, unsigned char writeData)
{
	char write_data[2] = { 0 };
	int ret = 0;

	mutex_lock(&ncp1854_i2c_access);

	write_data[0] = cmd;
	write_data[1] = writeData;

	new_client->ext_flag = ((new_client->ext_flag) & I2C_MASK_FLAG) | I2C_DIRECTION_FLAG;

	ret = i2c_master_send(new_client, write_data, 2);
	if (ret < 0) {
		new_client->ext_flag = 0;
		mutex_unlock(&ncp1854_i2c_access);
		return 0;
	}

	new_client->ext_flag = 0;
	mutex_unlock(&ncp1854_i2c_access);
	return 1;
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
unsigned int ncp1854_read_interface(unsigned char RegNum, unsigned char *val, unsigned char MASK,
				  unsigned char SHIFT)
{
	unsigned char ncp1854_reg = 0;
	int ret = 0;

	battery_log(BAT_LOG_FULL, "--------------------------------------------------\n");

	ret = ncp1854_read_byte(RegNum, &ncp1854_reg);

	battery_log(BAT_LOG_FULL, "[ncp1854_read_interface] Reg[%x]=0x%x\n", RegNum, ncp1854_reg);

	ncp1854_reg &= (MASK << SHIFT);
	*val = (ncp1854_reg >> SHIFT);
	battery_log(BAT_LOG_FULL, "[ncp1854_read_interface] Val=0x%x\n", *val);

	return ret;
}

unsigned int ncp1854_config_interface(unsigned char RegNum, unsigned char val, unsigned char MASK,
				    unsigned char SHIFT)
{
	unsigned char ncp1854_reg = 0;
	int ret = 0;

	battery_log(BAT_LOG_FULL, "--------------------------------------------------\n");

	ret = ncp1854_read_byte(RegNum, &ncp1854_reg);
	battery_log(BAT_LOG_FULL, "[ncp1854_config_interface] Reg[%x]=0x%x\n", RegNum, ncp1854_reg);

	ncp1854_reg &= ~(MASK << SHIFT);
	ncp1854_reg |= (val << SHIFT);

	ret = ncp1854_write_byte(RegNum, ncp1854_reg);
	battery_log(BAT_LOG_FULL, "[ncp18546_config_interface] Write Reg[%x]=0x%x\n", RegNum, ncp1854_reg);

	/* Check */
	/* ncp1854_read_byte(RegNum, &ncp1854_reg); */
	/* battery_log(BAT_LOG_FULL, "[ncp1854_config_interface] Check Reg[%x]=0x%x\n", RegNum, ncp1854_reg); */

	return ret;
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/
/* CON0 */
unsigned int ncp1854_get_chip_status(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON0),
				     (unsigned char *) (&val),
				     (unsigned char) (CON0_STATE_MASK), (unsigned char) (CON0_STATE_SHIFT)
	    );
	return val;
}

unsigned int ncp1854_get_batfet(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON0),
				     (unsigned char *) (&val),
				     (unsigned char) (CON0_BATFET_MASK), (unsigned char) (CON0_BATFET_SHIFT)
	    );
	return val;
}

unsigned int ncp1854_get_statint(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON0),
				     (unsigned char *) (&val),
				     (unsigned char) (CON0_STATINT_MASK),
				     (unsigned char) (CON0_STATINT_SHIFT)
	    );
	return val;
}

unsigned int ncp1854_get_faultint(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON0),
				     (unsigned char *) (&val),
				     (unsigned char) (CON0_FAULTINT_MASK),
				     (unsigned char) (CON0_FAULTINT_SHIFT)
	    );
	return val;
}

/* CON1 */
void ncp1854_set_reset(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_REG_RST_MASK),
				       (unsigned char) (CON1_REG_RST_SHIFT)
	    );
}

void ncp1854_set_chg_en(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_CHG_EN_MASK),
				       (unsigned char) (CON1_CHG_EN_SHIFT)
	    );
}

void ncp1854_set_otg_en(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_OTG_EN_MASK),
				       (unsigned char) (CON1_OTG_EN_SHIFT)
	    );
}

unsigned int ncp1854_get_otg_en(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON1),
				     (unsigned char *) (&val),
				     (unsigned char) (CON1_OTG_EN_MASK), (unsigned char) (CON1_OTG_EN_SHIFT)
	    );
	return val;
}

void ncp1854_set_fctry_mode(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_FCTRY_MOD_MASK),
				       (unsigned char) (CON1_FCTRY_MOD_SHIFT)
	    );
}

void ncp1854_set_tj_warn_opt(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_TJ_WARN_OPT_MASK),
				       (unsigned char) (CON1_TJ_WARN_OPT_SHIFT)
	    );
}

unsigned int ncp1854_get_usb_cfg(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON1),
				     (unsigned char *) (&val),
				     (unsigned char) (CON1_JEITA_OPT_MASK),
				     (unsigned char) (CON1_JEITA_OPT_SHIFT)
	    );
	return val;
}

void ncp1854_set_tchg_rst(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_TCHG_RST_MASK),
				       (unsigned char) (CON1_TCHG_RST_SHIFT)
	    );
}

void ncp1854_set_int_mask(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON1),
				       (unsigned char) (val),
				       (unsigned char) (CON1_INT_MASK_MASK),
				       (unsigned char) (CON1_INT_MASK_SHIFT)
	    );
}

/* CON2 */
void ncp1854_set_wdto_dis(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_WDTO_DIS_MASK),
				       (unsigned char) (CON2_WDTO_DIS_SHIFT)
	    );
}

void ncp1854_set_chgto_dis(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_CHGTO_DIS_MASK),
				       (unsigned char) (CON2_CHGTO_DIS_SHIFT)
	    );
}

void ncp1854_set_pwr_path(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_PWR_PATH_MASK),
				       (unsigned char) (CON2_PWR_PATH_SHIFT)
	    );
}

void ncp1854_set_trans_en(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_TRANS_EN_MASK),
				       (unsigned char) (CON2_TRANS_EN_SHIFT)
	    );
}

void ncp1854_set_iinset_pin_en(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_IINSET_PIN_EN_MASK),
				       (unsigned char) (CON2_IINSET_PIN_EN_SHIFT)
	    );
}

void ncp1854_set_iinlim_en(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_IINLIM_EN_MASK),
				       (unsigned char) (CON2_IINLIM_EN_SHIFT)
	    );
}

void ncp1854_set_aicl_en(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON2),
				       (unsigned char) (val),
				       (unsigned char) (CON2_AICL_EN_MASK),
				       (unsigned char) (CON2_AICL_EN_SHIFT)
	    );
}

#if 1//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]start
//CON6
unsigned int ncp1854_get_vobstol1(void)
{
    unsigned int ret=0;
    unsigned int val=0;

    ret=ncp1854_read_interface((unsigned char)(NCP1854_CON6),
	        					      (unsigned char *) (&val),
							      (unsigned char)(CON6_VOBSTOL1_MASK),
							      (unsigned char)(CON6_VOBSTOL1_SHIFT)
							      );
    return val;
}
unsigned int ncp1854_get_vobstol2(void)
{
    unsigned int ret=0;
    unsigned int val=0;

    ret=ncp1854_read_interface((unsigned char)(NCP1854_CON6),
	        					      (unsigned char *) (&val),
							      (unsigned char)(CON6_VOBSTOL2_MASK),
							      (unsigned char)(CON6_VOBSTOL2_SHIFT)
							      );
    return val;
}
#endif//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]end


/* CON8 */
unsigned int ncp1854_get_vfet_ok(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) (NCP1854_CON8),
				     (unsigned char *) (&val),
				     (unsigned char) (CON8_VFET_OK_MASK),
				     (unsigned char) (CON8_VFET_OK_SHIFT)
	    );
	return val;
}

#if 1//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]start
//CON13
void ncp1854_set_vobstol1_mask(unsigned int val)
{
    unsigned int ret=0;

    ret=ncp1854_config_interface((unsigned char)(NCP1854_CON13),
								(unsigned char) (val),
								(unsigned char) (CON13_VOBSTOL1_MASK),
								(unsigned char) (CON13_VOBSTOL1_SHIFT)
								);
}
void ncp1854_set_vobstol2_mask(unsigned int val)
{
    unsigned int ret=0;

    ret=ncp1854_config_interface((unsigned char)(NCP1854_CON13),
								(unsigned char) (val),
								(unsigned char) (CON13_VOBSTOL2_MASK),
								(unsigned char) (CON13_VOBSTOL2_SHIFT)
								);
}
#endif//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]end


/* CON14 */
void ncp1854_set_ctrl_vbat(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON14),
				       (unsigned char) (val),
				       (unsigned char) (CON14_CTRL_VBAT_MASK),
				       (unsigned char) (CON14_CTRL_VBAT_SHIFT)
	    );
}

/* CON15 */
void ncp1854_set_ichg_high(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON15),
				       (unsigned char) (val),
				       (unsigned char) (CON15_ICHG_HIGH_MASK),
				       (unsigned char) (CON15_ICHG_HIGH_SHIFT)
	    );
}

void ncp1854_set_ieoc(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON15),
				       (unsigned char) (val),
				       (unsigned char) (CON15_IEOC_MASK), (unsigned char) (CON15_IEOC_SHIFT)
	    );
}

void ncp1854_set_ichg(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON15),
				       (unsigned char) (val),
				       (unsigned char) (CON15_ICHG_MASK), (unsigned char) (CON15_ICHG_SHIFT)
	    );
}

unsigned int ncp1854_get_ichg(void)
{
	unsigned int ret = 0;
	unsigned int val = 0;

	ret = ncp1854_read_interface((unsigned char) NCP1854_CON15,
				     (unsigned char *) &val,
				     (unsigned char) CON15_ICHG_MASK, (unsigned char) CON15_ICHG_SHIFT);
	return val;
}

/* CON16 */
void ncp1854_set_iweak(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON16),
				       (unsigned char) (val),
				       (unsigned char) (CON16_IWEAK_MASK),
				       (unsigned char) (CON16_IWEAK_SHIFT)
	    );
}

void ncp1854_set_ctrl_vfet(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON16),
				       (unsigned char) (val),
				       (unsigned char) (CON16_CTRL_VFET_MASK),
				       (unsigned char) (CON16_CTRL_VFET_SHIFT)
	    );
}

void ncp1854_set_iinlim(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON16),
				       (unsigned char) (val),
				       (unsigned char) (CON16_IINLIM_MASK),
				       (unsigned char) (CON16_IINLIM_SHIFT)
	    );
}

/* CON17 */
void ncp1854_set_iinlim_ta(unsigned int val)
{
	unsigned int ret = 0;

	ret = ncp1854_config_interface((unsigned char) (NCP1854_CON17),
				       (unsigned char) (val),
				       (unsigned char) (CON17_IINLIM_TA_MASK),
				       (unsigned char) (CON17_IINLIM_TA_SHIFT)
	    );
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/
void ncp1854_dump_register(void)
{
	int i = 0;

	for (i = 0; i < ncp1854_REG_NUM; i++) {
		if ((i == 3) || (i == 4) || (i == 5) || (i == 6))	/* do not dump read clear status register */
			continue;
		if ((i == 10) || (i == 11) || (i == 12) || (i == 13))	/* do not dump interrupt mask bit register */
			continue;
		ncp1854_read_byte(i, &ncp1854_reg[i]);
		battery_log(BAT_LOG_FULL, "[ncp1854_dump_register] Reg[0x%X]=0x%X\n", i, ncp1854_reg[i]);
	}
}

void ncp1854_read_register(int i)
{
	ncp1854_read_byte(i, &ncp1854_reg[i]);
	battery_log(BAT_LOG_FULL, "[ncp1854_read_register] Reg[0x%X]=0x%X\n", i, ncp1854_reg[i]);
}

static void ncp1854_parse_customer_setting(void)
{
#ifdef CONFIG_OF
	unsigned int val;
	struct device_node *np;
	struct pinctrl *pinctrl;
	struct pinctrl_state *pinctrl_drvvbus_init;
	struct pinctrl_state *pinctrl_drvvbus_low;

	/* check customer setting */
	np = new_client->dev.of_node;
	if (np) {
		if (of_property_read_u32(np, "disable_ncp1854_fctry_mod", &val) == 0) {
			if (val)
				ncp1854_set_fctry_mode(0x0);

			battery_log(BAT_LOG_FULL, "%s: disable factory mode, %d\n", __func__, val);
		}
	}

	pinctrl = devm_pinctrl_get(&new_client->dev);
	if (IS_ERR(pinctrl)) {
		battery_log(BAT_LOG_CRTI, "[%s]Cannot find drvvbus pinctrl, err=%d\n",
			__func__, (int)PTR_ERR(pinctrl));
		return;
	}

	pinctrl_drvvbus_init = pinctrl_lookup_state(pinctrl, "drvvbus_init");
	if (IS_ERR(pinctrl_drvvbus_init)) {
		battery_log(BAT_LOG_CRTI, "[%s]Cannot find drvvbus_init state, err=%d\n",
			__func__, (int)PTR_ERR(pinctrl_drvvbus_init));
		return;
	}

	pinctrl_drvvbus_low = pinctrl_lookup_state(pinctrl, "drvvbus_low");
	if (IS_ERR(pinctrl_drvvbus_low)) {
		battery_log(BAT_LOG_CRTI, "[%s]Cannot find drvvbus_low state, err=%d\n",
			__func__, (int)PTR_ERR(pinctrl_drvvbus_low));
		return;
	}

	pinctrl_select_state(pinctrl, pinctrl_drvvbus_init);
	pinctrl_select_state(pinctrl, pinctrl_drvvbus_low);
	devm_pinctrl_put(pinctrl);
	battery_log(BAT_LOG_FULL, "[%s]pinctrl_select_state success\n", __func__);
#endif
}


static int ncp1854_driver_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int err = 0;

	battery_log(BAT_LOG_CRTI, "[ncp1854_driver_probe]\n");

	new_client = kmalloc(sizeof(struct i2c_client), GFP_KERNEL);

	if (!new_client) {
		err = -ENOMEM;
		goto exit;
	}
	memset(new_client, 0, sizeof(struct i2c_client));

	new_client = client;

	chargin_hw_init_done = KAL_TRUE;

	ncp1854_parse_customer_setting();

	return 0;

exit:
	return err;

}

/**********************************************************
  *
  *   [platform_driver API]
  *
  *********************************************************/
unsigned char g_reg_value_ncp1854 = 0;
static ssize_t show_ncp1854_access(struct device *dev, struct device_attribute *attr, char *buf)
{
	battery_log(BAT_LOG_CRTI, "[show_ncp1854_access] 0x%x\n", g_reg_value_ncp1854);
	return sprintf(buf, "%u\n", g_reg_value_ncp1854);
}

static ssize_t store_ncp1854_access(struct device *dev, struct device_attribute *attr,
				    const char *buf, size_t size)
{
	int ret = 0;
	char *pvalue = NULL, *addr, *val;
	unsigned int reg_value = 0;
	unsigned int reg_address = 0;

	battery_log(BAT_LOG_CRTI, "[store_ncp1854_access]\n");

	if (buf != NULL && size != 0) {
		battery_log(BAT_LOG_CRTI, "[store_ncp1854_access] buf is %s and size is %d\n", buf, (int)size);
		/*reg_address = kstrtoul(buf, &pvalue, 16);*/
		pvalue = (char *)buf;
		if (size > 3) {
			addr = strsep(&pvalue, " ");
			ret = kstrtou32(addr, 16, (unsigned int *)&reg_address);
		} else
			ret = kstrtou32(pvalue, 16, (unsigned int *)&reg_address);

		if (size > 3) {
			val =  strsep(&pvalue, " ");
			ret = kstrtou32(val, 16, (unsigned int *)&reg_value);

			battery_log(BAT_LOG_CRTI,
			    "[store_ncp1854_access] write ncp1854 reg 0x%x with value 0x%x !\n",
			     reg_address, reg_value);
			ret = ncp1854_config_interface(reg_address, reg_value, 0xFF, 0x0);
		} else {
			ret = ncp1854_read_interface(reg_address, &g_reg_value_ncp1854, 0xFF, 0x0);
			battery_log(BAT_LOG_CRTI,
			    "[store_ncp1854_access] read ncp1854 reg 0x%x with value 0x%x !\n",
			     reg_address, g_reg_value_ncp1854);
			battery_log(BAT_LOG_CRTI,
			    "[store_ncp1854_access] Please use \"cat ncp1854_access\" to get value\r\n");
		}
	}
	return size;
}

#if 1//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]start

static struct of_device_id ncp1854_chg_stat_eint_of_match[] = {
	{ .compatible = "mediatek,chr_stat",},
	{},
};

unsigned int ncp1854_chg_stat_irq = 0;

static DECLARE_WAIT_QUEUE_HEAD(ncp1854_int_status_wq);
static atomic_t ncp1854_int_status_flag = ATOMIC_INIT(0);


static irqreturn_t ncp1854_int_status_handler(unsigned irq, struct irq_desc *desc)
{
    atomic_set(&ncp1854_int_status_flag, 1);
    wake_up(&ncp1854_int_status_wq);
    return IRQ_HANDLED;
} 

static int ncp1854_int_status_kthread(void *x)
{
    while(1)
    {
        wait_event(ncp1854_int_status_wq, atomic_read(&ncp1854_int_status_flag));
        atomic_set(&ncp1854_int_status_flag, 0);

        printk("%s!!\n",__func__);

        if(1 == ncp1854_get_faultint())
        {
            if((1 == ncp1854_get_vobstol1()) || (1 == ncp1854_get_vobstol2()))
            {
                ncp1854_set_otg_en(0);
                //msleep(5);
                ncp1854_set_otg_en(1);
                ncp1854_set_vobstol1_mask(0x0);
                ncp1854_set_vobstol2_mask(0x0);
                ncp1854_set_int_mask(0x0);
            }
        }

        //enable_irq(ncp1854_chg_stat_irq);
    }
    return 0;
}

static void ncp1854_int_setup(void)
{
    int ret = 0;
    struct device_node *node = NULL;

    node = of_find_matching_node(node, ncp1854_chg_stat_eint_of_match);

    if (node) {
        ncp1854_chg_stat_irq = irq_of_parse_and_map(node, 0);
        ret = request_irq(ncp1854_chg_stat_irq, (irq_handler_t) ncp1854_int_status_handler, IRQF_TRIGGER_FALLING,"ncp1854_chg_stat-eint", NULL);
        disable_irq(ncp1854_chg_stat_irq);
    }
}

void ncp1854_int_enable(int enable)
{
    printk("%s!! enable :%d\n",__func__,enable);
    
    if(enable){
        ncp1854_set_vobstol1_mask(0x0);
        ncp1854_set_vobstol2_mask(0x0);
        ncp1854_set_int_mask(0x0);
        enable_irq(ncp1854_chg_stat_irq);
    }
    else{
        ncp1854_set_vobstol1_mask(0x1);
        ncp1854_set_vobstol2_mask(0x1);
        ncp1854_set_int_mask(0x1);
        disable_irq(ncp1854_chg_stat_irq);
    }
}

#endif//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]end


static DEVICE_ATTR(ncp1854_access, 0664, show_ncp1854_access, store_ncp1854_access);	/* 664 */

static int ncp1854_user_space_probe(struct platform_device *dev)
{
	int ret_device_file = 0;

	battery_log(BAT_LOG_CRTI, "******** ncp1854_user_space_probe!! ********\n");

	ret_device_file = device_create_file(&(dev->dev), &dev_attr_ncp1854_access);

	return 0;
}

#if 0
struct platform_device ncp1854_user_space_device = {
	.name = "ncp1854-user",
	.id = -1,
};
#endif

static struct platform_driver ncp1854_user_space_driver = {
	.probe = ncp1854_user_space_probe,
	.driver = {
		   .name = "ncp1854-user",
           .of_match_table = ncp1854_chg_stat_eint_of_match,   
		   },
};

static int __init ncp1854_init(void)
{
	int ret = 0;   
	battery_log(BAT_LOG_CRTI, "[ncp1854_init] init start\n");

	if (i2c_add_driver(&ncp1854_driver) != 0)
		battery_log(BAT_LOG_CRTI, "[ncp1854_init] failed to register ncp1854 i2c driver.\n");
	else
		battery_log(BAT_LOG_CRTI, "[ncp1854_init] Success to register ncp1854 i2c driver.\n");

	/* ncp1854 user space access interface */
#if 0    
	ret = platform_device_register(&ncp1854_user_space_device);
	if (ret) {
		battery_log(BAT_LOG_CRTI, "****[ncp1854_init] Unable to device register(%d)\n", ret);
		return ret;
	}
#endif    
	ret = platform_driver_register(&ncp1854_user_space_driver);
	if (ret) {
		battery_log(BAT_LOG_CRTI, "****[ncp1854_init] Unable to register driver (%d)\n", ret);
		return ret;
	}
#if 1//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]start
        kthread_run(ncp1854_int_status_kthread, NULL, "ncp1854_int_status_kthread"); 
        ncp1854_int_setup();
#endif//lisong 2014-8-19 [NO BUGID][fix bug:ncp1854 otg vbus overload]end

	return 0;
}

static void __exit ncp1854_exit(void)
{
	i2c_del_driver(&ncp1854_driver);
}

module_init(ncp1854_init);
module_exit(ncp1854_exit);
/* subsys_initcall(ncp1854_subsys_init); */

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("I2C ncp1854 Driver");
MODULE_AUTHOR("YT Lee<yt.lee@mediatek.com>");