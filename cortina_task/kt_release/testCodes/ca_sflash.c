// SPDX-License-Identifier: GPL-2.0+
/*
 * Driver for Cortina SPI-FLASH Controller
 *
 * Copyright (C) 2020 Cortina Access Inc. All Rights Reserved.
 *
 * Author: PengPeng Chen <pengpeng.chen@cortina-access.com>
 */

#include <common.h>
#include <malloc.h>
#include <clk.h>
#include <dm.h>
#include <errno.h>
#include <fdtdec.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/ioport.h>
#include <linux/sizes.h>
#include <spi.h>
#include <spi-mem.h>
#include <reset.h>

DECLARE_GLOBAL_DATA_PTR;

struct ca_sflash_regs {
	u32 idr;		/* 0x00:Flash word ID Register */
	u32 tc;			/* 0x04:Flash Timeout Counter Register */
	u32 sr;			/* 0x08:Flash Status Register */
	u32 tr;			/* 0x0C:Flash Type Register */
	u32 asr;		/* 0x10:Flash ACCESS START/BUSY Register */
	u32 isr;		/* 0x14:Flash Interrupt Status Register */
	u32 imr;		/* 0x18:Flash Interrupt Mask Register */
	u32 fcr;		/* 0x1C:NAND Flash FIFO Control Register */
	u32 ffsr;		/* 0x20:Flash FIFO Status Register */
	u32 ffar;		/* 0x24:Flash FIFO ADDRESS Register */
	u32 ffmar;		/* 0x28:Flash FIFO MATCHING ADDRESS Register */
	u32 ffdr;		/* 0x2C:Flash FIFO Data Register */
	u32 ar;			/* 0x30:Serial Flash Access Register */
	u32 ear;		/* 0x34:Serial Flash Extend Access Register */
	u32 adr;		/* 0x38:Serial Flash ADdress Register */
	u32 dr;			/* 0x3C:Serial Flash Data Register */
	u32 tmr;		/* 0x40:Serial Flash Timing Register */
};

/*
 * FLASH_TYPE
 */
#define CA_FLASH_TR_PIN            	BIT(15)
#define CA_FLASH_TR_TYPE_MASK      	GENMASK(14, 12)
#define CA_FLASH_TR_TYPE(tp)		(((tp) << 12) & CA_FLASH_TR_TYPE_MASK)
#define CA_FLASH_TR_WIDTH			BIT(11)
#define CA_FLASH_TR_SIZE_MASK		GENMASK(10, 9)
#define CA_FLASH_TR_SIZE(sz)		(((sz) << 9) & CA_FLASH_TR_SIZE_MASK)

/*
 * FLASH_FLASH_ACCESS_START
 */
#define CA_FLASH_ASR_IND_START_EN  	BIT(1)
#define CA_FLASH_ASR_DMA_START_EN  	BIT(3)
#define CA_FLASH_ASR_WR_ACCESS_EN	BIT(9)

/*
 * FLASH_FLASH_INTERRUPT
 */
#define CA_FLASH_ISR_REG_IRQ   		BIT(1)
#define CA_FLASH_ISR_FIFO_IRQ  		BIT(2)

/*
 * FLASH_SF_ACCESS
 */
#define CA_SF_AR_OPCODE_MASK   		GENMASK(7, 0)
#define CA_SF_AR_OPCODE(op)			(((op) << 0) & CA_SF_AR_OPCODE_MASK)
#define CA_SF_AR_ACCODE_MASK   		GENMASK(11, 8)
#define CA_SF_AR_ACCODE(ac)			(((ac) << 8) & CA_SF_AR_ACCODE_MASK)
#define CA_SF_AR_FORCE_TERM   		BIT(12)
#define CA_SF_AR_FORCE_BURST   		BIT(13)
#define CA_SF_AR_AUTO_MODE_EN   	BIT(15)
#define CA_SF_AR_CHIP_EN_ALT  		BIT(16)
#define CA_SF_AR_HI_SPEED_RD   		BIT(17)
#define CA_SF_AR_MIO_INF_DC   		BIT(24)
#define CA_SF_AR_MIO_INF_AC   		BIT(25)
#define CA_SF_AR_MIO_INF_CC   		BIT(26)
#define CA_SF_AR_DDR_MASK   		GENMASK(29, 28)
#define CA_SF_AR_DDR(ddr)			(((ddr) << 28) & CA_SF_AR_DDR_MASK)
#define CA_SF_AR_MIO_INF_MASK   	GENMASK(31, 30)
#define CA_SF_AR_MIO_INF(io)		(((io) << 30) & CA_SF_AR_MIO_INF_MASK)

/*
 * FLASH_SF_EXT_ACCESS
 */
#define CA_SF_EAR_OPCODE_MASK   	GENMASK(7, 0)
#define CA_SF_EAR_OPCODE(op)		(((op) << 0) & CA_SF_EAR_OPCODE_MASK)
#define CA_SF_EAR_DATA_CNT_MASK   	GENMASK(20, 8)
#define CA_SF_EAR_DATA_CNT(cnt)		(((cnt) << 8) & CA_SF_EAR_DATA_CNT_MASK)
#define CA_SF_EAR_DATA_CNT_MAX		(4096)
#define CA_SF_EAR_ADDR_CNT_MASK   	GENMASK(23, 21)
#define CA_SF_EAR_ADDR_CNT(cnt)		(((cnt) << 21) & CA_SF_EAR_ADDR_CNT_MASK)
#define CA_SF_EAR_ADDR_CNT_MAX		(5)
#define CA_SF_EAR_DUMY_CNT_MASK   	GENMASK(29, 24)
#define CA_SF_EAR_DUMY_CNT(cnt)		(((cnt) << 24) & CA_SF_EAR_DUMY_CNT_MASK)
#define CA_SF_EAR_DUMY_CNT_MAX		(32)
#define CA_SF_EAR_DRD_CMD_EN		BIT(31)

/*
 * FLASH_SF_ADDRESS
 */
#define CA_SF_ADR_REG_MASK   		GENMASK(31, 0)
#define CA_SF_ADR_REG(addr)			(((addr) << 0) & CA_SF_ADR_REG_MASK)

/*
 * FLASH_SF_DATA
 */
#define CA_SF_DR_REG_MASK   		GENMASK(31, 0)
#define CA_SF_DR_REG(addr)			(((addr) << 0) & CA_SF_DR_REG_MASK)

/*
 * FLASH_SF_TIMING
 */
#define CA_SF_TMR_IDLE_MASK   		GENMASK(7, 0)
#define CA_SF_TMR_IDLE(idle)		(((idle) << 0) & CA_SF_TMR_IDLE_MASK)
#define CA_SF_TMR_HOLD_MASK   		GENMASK(15, 8)
#define CA_SF_TMR_HOLD(hold)		(((hold) << 8) & CA_SF_TMR_HOLD_MASK)
#define CA_SF_TMR_SETUP_MASK   		GENMASK(23, 16)
#define CA_SF_TMR_SETUP(setup)		(((setup) << 16) & CA_SF_TMR_SETUP_MASK)
#define CA_SF_TMR_CLK_MASK   		GENMASK(26, 24)
#define CA_SF_TMR_CLK(clk)			(((clk) << 24) & CA_SF_TMR_CLK_MASK)

#define CA_SFLASH_IND_WRITE			0
#define CA_SFLASH_IND_READ			1
#define CA_SFLASH_MEM_MAP			3
#define CA_SFLASH_FIFO_TIMEOUT_US	30000
#define CA_SFLASH_BUSY_TIMEOUT_US	40000

#define CA_SF_AC_OPCODE                		0x00
#define CA_SF_AC_OPCODE_1_DATA           	0x01
#define CA_SF_AC_OPCODE_2_DATA           	0x02
#define CA_SF_AC_OPCODE_3_DATA           	0x03
#define CA_SF_AC_OPCODE_4_DATA           	0x04
#define CA_SF_AC_OPCODE_3_ADDR         		0x05
#define CA_SF_AC_OPCODE_4_ADDR         		(CA_SF_AC_OPCODE_3_ADDR << 2)
#define CA_SF_AC_OPCODE_3_ADDR_1_DATA    	0x06
#define CA_SF_AC_OPCODE_4_ADDR_1_DATA    	(CA_SF_AC_OPCODE_3_ADDR_1_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_2_DATA    	0x07
#define CA_SF_AC_OPCODE_4_ADDR_2_DATA    	(CA_SF_AC_OPCODE_3_ADDR_2_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_3_DATA    	0x08
#define CA_SF_AC_OPCODE_4_ADDR_3_DATA    	(CA_SF_AC_OPCODE_3_ADDR_3_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_4_DATA    	0x09
#define CA_SF_AC_OPCODE_4_ADDR_4_DATA    	(CA_SF_AC_OPCODE_3_ADDR_4_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_X_1_DATA   	0x0A
#define CA_SF_AC_OPCODE_4_ADDR_X_1_DATA   	(CA_SF_AC_OPCODE_3_ADDR_X_1_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_X_2_DATA   	0x0B
#define CA_SF_AC_OPCODE_4_ADDR_X_2_DATA   	(CA_SF_AC_OPCODE_3_ADDR_X_2_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_X_3_DATA   	0x0C
#define CA_SF_AC_OPCODE_4_ADDR_X_3_DATA   	(CA_SF_AC_OPCODE_3_ADDR_X_3_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_X_4_DATA   	0x0D
#define CA_SF_AC_OPCODE_4_ADDR_X_4_DATA   	(CA_SF_AC_OPCODE_3_ADDR_X_4_DATA << 2)
#define CA_SF_AC_OPCODE_3_ADDR_4X_1_DATA  	0x0E
#define CA_SF_AC_OPCODE_4_ADDR_4X_1_DATA  	(CA_SF_AC_OPCODE_3_ADDR_4X_1_DATA << 2)
#define CA_SF_AC_OPCODE_EXTEND				0x0F

#define CA_SF_ACCESS_MIO_SINGLE		0
#define CA_SF_ACCESS_MIO_DUAL		1
#define CA_SF_ACCESS_MIO_QUARD		2

struct ca_sflash_priv {
	struct ca_sflash_regs *regs;
	ulong clock_rate;
	int cs_used;
};

/*
 * This function doesn't do anything except help with debugging
 */
static int ca_sflash_claim_bus(struct udevice *dev)
{
	debug("%s:\n", __func__);
	return 0;
}

static int ca_sflash_release_bus(struct udevice *dev)
{
	debug("%s:\n", __func__);
	return 0;
}

static int ca_sflash_set_speed(struct udevice *dev, uint speed)
{
	debug("%s:\n", __func__);
	return 0;
}

static int ca_sflash_set_mode(struct udevice *dev, uint mode)
{
	debug("%s:\n", __func__);
	return 0;
}

static int _ca_sflash_wait_for_not_busy(struct ca_sflash_priv *priv)
{
	u32 asr;

	if (readl_poll_timeout(&priv->regs->asr, asr,
				 !(asr & CA_FLASH_ASR_IND_START_EN),
				 CA_SFLASH_BUSY_TIMEOUT_US)) {
		pr_err("busy timeout (stat:%#x)\n", asr);
		return -1;
	}

	return 0;
}

static int _ca_sflash_wait_access_complete(struct ca_sflash_priv *priv,
							u8 wrd_flag)
{
	if (wrd_flag == 1) {
		/* Enable write access and start the sflash indirect access */
		setbits_le32(&priv->regs->asr, 
			CA_FLASH_ASR_WR_ACCESS_EN
			| CA_FLASH_ASR_IND_START_EN);
	} else {
		/* Start the sflash indirect access */
		setbits_le32(&priv->regs->asr, CA_FLASH_ASR_IND_START_EN);
	}
	/* Wait til the action(rd/wr) completed */
	return _ca_sflash_wait_for_not_busy(priv);
}

static int _ca_sflash_read(struct ca_sflash_priv *priv,
							u8 *buf, unsigned int data_len)
{
	u32 reg_data;
	unsigned int len = data_len;

	while (len >= 4) {
		if (_ca_sflash_wait_access_complete(priv, 0))
			return -1;
		reg_data = readl(&priv->regs->dr);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
		debug("%s: reg_data=%#08x\n",
			__func__, reg_data);
	}
	if (len > 0) {
		if (_ca_sflash_wait_access_complete(priv, 0))
			return -1;
		reg_data = readl(&priv->regs->dr);
		debug("%s: reg_data=%#08x\n",
			__func__, reg_data);
	}
	switch (len) {
	case 3:
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		break;
	case 2:
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		break;
	case 1:
		*buf++ = reg_data & 0xFF;
		break;
	case 0:
		break;
	default:
		debug("%s: error data_length %d!\n", __func__, len);
	}

	return 0;
}

static int _ca_sflash_read_data(struct ca_sflash_priv *priv,
							u8 *data, unsigned int cnt)
{
	unsigned int len;
	u8 *buf = NULL;

	while (cnt > 0) {
		if (cnt < CA_SF_EAR_DATA_CNT_MAX)
			len = cnt;
		else
			len = CA_SF_EAR_DATA_CNT_MAX;
	
		if (_ca_sflash_read(priv, buf, len))
			break;
		memset(buf, 0, len);
		memcpy(data, buf, len);
		cnt -= len;
		data += len;
	}

	return cnt > 0 ? -1 : 0;
}

static int _ca_sflash_write(struct ca_sflash_priv *priv,
							u8 *buf, unsigned int data_len)
{
	u32 reg_data;
	unsigned int len = data_len;

	while (len > 0) {
		reg_data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		/* Fill data */
		clrsetbits_le32(&priv->regs->dr,
				GENMASK(31, 0), reg_data);
		if (_ca_sflash_wait_access_complete(priv, 1))
			return -1;
		len -= 4;
		buf += 4;
	}

	return 0;
}

static int _ca_sflash_write_data(struct ca_sflash_priv *priv,
							u8 *data, unsigned int cnt)
{
	unsigned int len;
	u8 *buf = NULL;

	while (cnt > 0) {
		if (cnt < CA_SF_EAR_DATA_CNT_MAX)
			len = cnt;
		else
			len = CA_SF_EAR_DATA_CNT_MAX;

		memset(buf, 0, len);
		memcpy(buf, data, len);
		if (_ca_sflash_write(priv, buf, len))
			break;
		cnt -= len;
		data += len;
	}

	return cnt > 0 ? -1 : 0;
}

static int _ca_sflash_regs_cfg(struct ca_sflash_priv *priv,
					const struct spi_mem_op *op, u8 access_type)
{
	u8 *data_buf;
	u8 addr_cnt = op->addr.nbytes;
	u8 dummy_cnt = op->dummy.nbytes;
	unsigned int data_cnt = op->data.nbytes;
	u32 addr_offset = (u32)op->addr.val;

	/* Set the access register */
	clrsetbits_le32(&priv->regs->ar,
			GENMASK(31, 0), CA_SF_AR_ACCODE(access_type));
	if (access_type == CA_SF_AC_OPCODE_EXTEND) {
		if (data_cnt % 4 != 0)
			data_cnt = ((data_cnt + 4) / 4) * 4;
		/* Use command in extend_access register */
		clrsetbits_le32(&priv->regs->ear,
				GENMASK(31, 0), CA_SF_EAR_OPCODE(op->cmd.opcode)
				| CA_SF_EAR_DUMY_CNT(dummy_cnt * 8 - 1)
				| CA_SF_EAR_ADDR_CNT(addr_cnt - 1)
				| CA_SF_EAR_DATA_CNT(data_cnt - 1)
				| CA_SF_EAR_DRD_CMD_EN);
		debug("%s: FLASH EXT ACCESS reg=%#08x\n",
			__func__, readl(&priv->regs->ear));
	} else {
		setbits_le32(&priv->regs->ar, CA_SF_AR_OPCODE(op->cmd.opcode));	
	}
	debug("%s: FLASH ACCESS reg=%#08x\n",
		__func__, readl(&priv->regs->ar));

	/* Fill address */
	if (addr_cnt > 0)
		clrsetbits_le32(&priv->regs->adr, GENMASK(31, 0), addr_offset);

	if (op->data.dir == SPI_MEM_DATA_IN) {
		data_buf = op->data.buf.in;
		if (_ca_sflash_read_data(priv, data_buf, data_cnt))
			return -1;
	} else {
		data_buf = op->data.buf.out;
		if (_ca_sflash_write_data(priv, data_buf, data_cnt))
			return -1;
	}
	debug("%s:\n", __func__);

	return 0;
}

static void _ca_sflash_regs_cls(struct ca_sflash_priv * priv,
					u8 access_type)
{
	/* Clear regs flags */
	clrbits_le32(&priv->regs->ar, GENMASK(31, 0));
	if (access_type == CA_SF_AC_OPCODE_EXTEND)
		clrsetbits_le32(&priv->regs->ear, GENMASK(31, 0), 0x3fffff00);
	clrbits_le32(&priv->regs->adr, GENMASK(31, 0));
	clrbits_le32(&priv->regs->asr, GENMASK(31, 0));
	/* elapse 10us before issuing any other command */
	udelay(10);
	debug("%s:\n", __func__);
}

#if 0
static int _ca_sflash_opcode_only_cmd(struct ca_sflash_priv *priv,
				   struct spi_mem_op *op)
{
	int ret;

	/* Set the acc register */
	clrsetbits_le32(&priv->regs->ar,
			GENMASK(31, 0), CA_SF_AR_OPCODE(op->cmd.opcode)
			| CA_SF_AR_ACCODE(CA_SF_AC_OPCODE));
	debug("%s: FLASH ACCESS reg=%#08x\n",
	      __func__, readl(&priv->regs->ar));
	/* Start the sflash indirect access */
	setbits_le32(&priv->regs->asr, CA_FLASH_ASR_IND_START_EN);
	/* Wait til the action(rd/wr) completed */
	if (ret = _ca_sflash_wait_for_not_busy(priv))
		return ret;
	clrbits_le32(&priv->regs->ar, GENMASK(31, 0));
	clrbits_le32(&priv->regs->asr, GENMASK(31, 0));
	debug("%s:\n", __func__);
	return 0;
}
#endif

#if 0
static int _ca_sflash_read_data_cmd(struct ca_sflash_priv *priv,
				   struct spi_mem_op *op)
{
	u8 *buf = op->data.buf.in;
	u8 count_addr = op->addr.nbytes;
	u8 count_dummy = op->dummy.nbytes;
	unsigned int count_data = op->data.nbytes;
	unsigned int data_len = op->data.nbytes;
	u32 addr_offset = (u32)op->addr.val;
	u32 data_loop, reg_data;
	int ret;

	if (data_len > CA_SF_EAR_DATA_CNT_MAX) {
		data_loop = data_len / CA_SF_EAR_DATA_CNT_MAX;
		data_len = data_len % CA_SF_EAR_DATA_CNT_MAX;
		printf("%s: data_loop=%d, data_len=%d\n",
			__func__, data_loop, data_len);
	}
	/* Set the acc register */
	clrsetbits_le32(&priv->regs->ar,
			GENMASK(31, 0), CA_SF_AR_ACCODE(CA_SF_AC_OPCODE_EXTEND));
	if (data_len > 4)
		setbits_le32(&priv->regs->ar, CA_SF_AR_FORCE_BURST);		
	debug("%s: FLASH ACCESS reg=%#08x\n",
	      __func__, readl(&priv->regs->ar));
	if (count_data % 4 != 0) {
		count_data = ((count_data + 4) / 4) * 4;
	}
	/* Use command in extend_acc register */
	clrsetbits_le32(&priv->regs->ear,
			GENMASK(31, 0), CA_SF_EAR_OPCODE(op->cmd.opcode)
			| CA_SF_EAR_DUMY_CNT(count_dummy * 8 - 1)
			| CA_SF_EAR_ADDR_CNT(count_addr - 1)
			| CA_SF_EAR_DATA_CNT(count_data - 1)
			| CA_SF_EAR_DRD_CMD_EN);
	debug("%s: FLASH EXT ACCESS reg=%#08x\n",
	      __func__, readl(&priv->regs->ear));
	if (count_addr > 0)
		/* Fill address */
		clrsetbits_le32(&priv->regs->adr, GENMASK(31, 0), (u32)addr_offset);
	while (data_len >= 4) {
		/* Start the sflash indirect access */
		setbits_le32(&priv->regs->asr, CA_FLASH_ASR_IND_START_EN);
		/* Wait til the action(rd/wr) completed */
		ret = _ca_sflash_wait_for_not_busy(priv);
		if (ret)
			return ret;
		reg_data = readl(&priv->regs->dr);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		data_len -= 4;
		debug("%s: reg_data=%#08x\n",
			__func__, reg_data);
	}
	if (data_len > 0) {
		/* Start the sflash indirect access */
		setbits_le32(&priv->regs->asr, CA_FLASH_ASR_IND_START_EN);
		/* Wait til the action(rd/wr) completed */
		ret = _ca_sflash_wait_for_not_busy(priv);
		if (ret)
			return ret;
		reg_data = readl(&priv->regs->dr);
	}
	debug("%s: reg_data=%#08x\n",
		__func__, reg_data);
	}

	/* Clear regs flags */
	clrbits_le32(&priv->regs->ar, GENMASK(31, 0));
	clrsetbits_le32(&priv->regs->ear, GENMASK(31, 0), 0x3fffff00);
	clrbits_le32(&priv->regs->adr, GENMASK(31, 0));
	clrbits_le32(&priv->regs->asr, GENMASK(31, 0));

	debug("%s:\n", __func__);
	return 0;
}
#endif


#if 0
static int _ca_sflash_write_data_cmd(struct ca_sflash_priv *priv,
				   struct spi_mem_op *op)
{
	u8 *buf = op->data.buf.out;
	u8 count_dummy = op->dummy.nbytes;
	u8 count_addr = op->addr.nbytes;
	unsigned int data_len = op->data.nbytes;
	unsigned int count_data = op->data.nbytes;
	u32 addr_offset = op->addr.val;
	u32 loop, reg_data;
	int ret;

	loop = data_len / CA_SF_EAR_DATA_CNT_MAX;
	data_len = data_len % CA_SF_EAR_DATA_CNT_MAX;
	/* Set the acc register */
	clrsetbits_le32(&priv->regs->ar,
			GENMASK(31, 0), CA_SF_AR_ACCODE(CA_SF_AC_OPCODE_EXTEND));
	if (data_len >= 4)
		setbits_le32(&priv->regs->ar, CA_SF_AR_FORCE_BURST);
	debug("%s: FLASH ACCESS reg=%#08x\n",
	      __func__, readl(&priv->regs->ar));

	if (count_data % 4 != 0) {
		count_data = ((count_data + 4) / 4) * 4;
	}
	/* Use command in extend_acc register */
	clrsetbits_le32(&priv->regs->ear,
			GENMASK(31, 0), CA_SF_EAR_OPCODE(op->cmd.opcode)
			| CA_SF_EAR_DUMY_CNT(count_dummy * 8 - 1)
			| CA_SF_EAR_ADDR_CNT(count_addr - 1)
			| CA_SF_EAR_DATA_CNT(count_data - 1)
			| CA_SF_EAR_DRD_CMD_EN);
	debug("%s: FLASH EXT ACCESS reg=%#08x\n",
	      __func__, readl(&priv->regs->ear));
	/* Fill address */
	clrsetbits_le32(&priv->regs->adr,
			GENMASK(31, 0), (u32)addr_offset);
	while (data_len > 0) {
		reg_data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		/* Fill data */
		clrsetbits_le32(&priv->regs->dr,
				GENMASK(31, 0), reg_data);
		/* Enable write access and start the sflash indirect access */
		setbits_le32(&priv->regs->asr, 
			CA_FLASH_ASR_WR_ACCESS_EN
			| CA_FLASH_ASR_IND_START_EN);
		/* Wait til the action(rd/wr) completed */
		ret = _ca_sflash_wait_for_not_busy(priv);
		if (ret)
			return ret;
		data_len -= 4;
		buf += 4;
	}

	/* Clear regs flags */
	clrbits_le32(&priv->regs->ar, GENMASK(31, 0));
	clrsetbits_le32(&priv->regs->ear, GENMASK(31, 0), 0x3fffff00);
	clrbits_le32(&priv->regs->adr, GENMASK(31, 0));
	clrbits_le32(&priv->regs->asr, GENMASK(31, 0));
	debug("%s:\n", __func__);
	return 0;
}

static int _ca_sflash_erase_cmd(struct ca_sflash_priv *priv,
				   struct spi_mem_op *op)
{
	clrbits_le32(&priv->regs->asr, GENMASK(31, 0));
	debug("%s:\n", __func__);
	return 0;
}
#endif

static int _ca_sflash_cmd_exe(struct ca_sflash_priv *priv,
				   const struct spi_mem_op *op, u8 access_type)
{

	if (_ca_sflash_regs_cfg(priv, op, access_type))
		return -1;
	_ca_sflash_regs_cls(priv, access_type);
	debug("%s:\n", __func__);

	return 0;
}

static int ca_sflash_exec_op(struct spi_slave *slave,
			     const struct spi_mem_op *op)
{
	struct ca_sflash_priv *priv = dev_get_priv(slave->dev->parent);
	u8 access_type;

	debug("%s: cmd:%#02x addr.val:%#llx addr.len:%#x data.len:%#x data.dir:%#x\n",
	     __func__, op->cmd.opcode, op->addr.val,
	    op->addr.nbytes, op->data.nbytes, op->data.dir);

	if (op->data.nbytes == 0 && op->addr.nbytes == 0)
		access_type = CA_SF_AC_OPCODE;//reset_op, wr_enable, wr_disable
	else if (op->data.nbytes == 0 && op->addr.nbytes > 0)
		access_type = CA_SF_AC_OPCODE_4_ADDR;//erase_op
	else if (op->data.nbytes > 0)
		access_type = CA_SF_AC_OPCODE_EXTEND;//read_data, wirte_data
	else {
		printf("%s: can't support cmd.opcode:(%#02x) type currently!\n",
			__func__, op->cmd.opcode);
		return -1;
	}

	return _ca_sflash_cmd_exe(priv, op, access_type);
}

static void ca_sflash_init(struct ca_sflash_priv *priv)
{
	/* Set FLASH_TYPE as serial flash, 
	 * NAND value:0x8000
	 * NOR  vaule:0x400
	 */
#ifdef CONFIG_CORTINA_SFLASH_NAND
	clrsetbits_le32(&priv->regs->tr,
		GENMASK(31, 0), CA_FLASH_TR_PIN);
#endif
#ifdef CONFIG_CORTINA_SFLASH_NOR
	clrsetbits_le32(&priv->regs->tr,
		GENMASK(31, 0), CA_FLASH_TR_SIZE(2));
#endif
	debug("%s: FLASH_TYPE reg=%#x\n",
		__func__, readl(&priv->regs->tr));
	/* Minimize flash timing, value:0x07010101 */
	clrsetbits_le32(&priv->regs->tmr,
		GENMASK(31, 0),
		CA_SF_TMR_CLK(0x07)
		| CA_SF_TMR_SETUP(0x01)
		| CA_SF_TMR_HOLD(0x01)
		| CA_SF_TMR_IDLE(0x01));
	debug("%s: FLASH_TIMING reg=%#x\n", __func__,
	      readl(&priv->regs->tmr));
}

static int ca_sflash_probe(struct udevice *dev)
{
	struct ca_sflash_priv *priv = dev_get_priv(dev);
	struct resource res;
	int ret;

	/* Map the registers */
	ret = dev_read_resource_byname(dev, "sflash-regs", &res);
	if (ret) {
		dev_err(dev, "can't get regs base addresses(ret = %d)!\n", ret);
		return ret;
	}
	priv->regs = devm_ioremap(dev, res.start, resource_size(&res));
	if (IS_ERR(priv->regs))
		return PTR_ERR(priv->regs);

	ca_sflash_init(priv);
	printf("%s:\n", __func__);
	return 0;
}

static const struct spi_controller_mem_ops ca_sflash_mem_ops = {
	.exec_op = ca_sflash_exec_op,
};

static const struct dm_spi_ops ca_sflash_ops = {
	.claim_bus = ca_sflash_claim_bus,
	.release_bus = ca_sflash_release_bus,
	.set_speed = ca_sflash_set_speed,
	.set_mode = ca_sflash_set_mode,
	.mem_ops = &ca_sflash_mem_ops,
};

static const struct udevice_id ca_sflash_ids[] = {
	{.compatible = "cortina,ca-sflash"},
	{}
};

U_BOOT_DRIVER(ca_sflash) = {
	.name = "ca_sflash",
	.id = UCLASS_SPI,
	.of_match = ca_sflash_ids,
	.ops = &ca_sflash_ops,
	.priv_auto_alloc_size = sizeof(struct ca_sflash_priv),
	.probe = ca_sflash_probe,
};
