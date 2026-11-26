/*
 * Acer One 8 T482L ILI9881C panel driver.
 *
 * Panel sequence was recovered from the stock Acer kernel and verified
 * byte-for-byte against its compressed Image at offset 0x01867780.
 *
 * Copyright (C) 2026 Acer One 8 T482L port contributors
 * SPDX-License-Identifier: GPL-2.0
 */
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include "lcm_drv.h"
#include "disp_dts_gpio.h"

#undef mdelay
#define MDELAY(n)                  lcm_util.mdelay(n)

#define T482L_WIDTH                 800
#define T482L_HEIGHT                1280
#define T482L_REGFLAG_DELAY         0xFFE
#define T482L_REGFLAG_END           0xFFF
#define T482L_DSI_CMD_DELAY_MS      0xFFFC

static struct LCM_UTIL_FUNCS lcm_util;
static unsigned int t482l_backlight_level = 0xFF;

struct t482l_lcm_cmd {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

/* Stock Acer table: 204 records, 72-byte record stride. */
static const struct t482l_lcm_cmd acer_init_sequence[] = {
    {0x0FF, 3, {0x98, 0x81, 0x03}},
    {0x001, 1, {0x00}},
    {0x002, 1, {0x00}},
    {0x003, 1, {0x53}},
    {0x004, 1, {0x13}},
    {0x005, 1, {0x00}},
    {0x006, 1, {0x04}},
    {0x007, 1, {0x00}},
    {0x008, 1, {0x00}},
    {0x009, 1, {0x22}},
    {0x00A, 1, {0x22}},
    {0x00B, 1, {0x00}},
    {0x00C, 1, {0x01}},
    {0x00D, 1, {0x00}},
    {0x00E, 1, {0x00}},
    {0x00F, 1, {0x25}},
    {0x010, 1, {0x25}},
    {0x011, 1, {0x00}},
    {0x012, 1, {0x00}},
    {0x013, 1, {0x00}},
    {0x014, 1, {0x00}},
    {0x015, 1, {0x00}},
    {0x016, 1, {0x00}},
    {0x017, 1, {0x00}},
    {0x018, 1, {0x00}},
    {0x019, 1, {0x00}},
    {0x01A, 1, {0x00}},
    {0x01B, 1, {0x00}},
    {0x01C, 1, {0x00}},
    {0x01D, 1, {0x00}},
    {0x01E, 1, {0x44}},
    {0x01F, 1, {0x80}},
    {0x020, 1, {0x02}},
    {0x021, 1, {0x03}},
    {0x022, 1, {0x00}},
    {0x023, 1, {0x00}},
    {0x024, 1, {0x00}},
    {0x025, 1, {0x00}},
    {0x026, 1, {0x00}},
    {0x027, 1, {0x00}},
    {0x028, 1, {0x33}},
    {0x029, 1, {0x03}},
    {0x02A, 1, {0x00}},
    {0x02B, 1, {0x00}},
    {0x02C, 1, {0x00}},
    {0x02D, 1, {0x00}},
    {0x02E, 1, {0x00}},
    {0x02F, 1, {0x00}},
    {0x030, 1, {0x00}},
    {0x031, 1, {0x00}},
    {0x032, 1, {0x00}},
    {0x033, 1, {0x00}},
    {0x034, 1, {0x04}},
    {0x035, 1, {0x00}},
    {0x036, 1, {0x00}},
    {0x037, 1, {0x00}},
    {0x038, 1, {0x3C}},
    {0x039, 1, {0x00}},
    {0x03A, 1, {0x40}},
    {0x03B, 1, {0x40}},
    {0x03C, 1, {0x00}},
    {0x03D, 1, {0x00}},
    {0x03E, 1, {0x00}},
    {0x03F, 1, {0x00}},
    {0x040, 1, {0x00}},
    {0x041, 1, {0x00}},
    {0x042, 1, {0x00}},
    {0x043, 1, {0x00}},
    {0x044, 1, {0x00}},
    {0x050, 1, {0x01}},
    {0x051, 1, {0x23}},
    {0x052, 1, {0x45}},
    {0x053, 1, {0x67}},
    {0x054, 1, {0x89}},
    {0x055, 1, {0xAB}},
    {0x056, 1, {0x01}},
    {0x057, 1, {0x23}},
    {0x058, 1, {0x45}},
    {0x059, 1, {0x67}},
    {0x05A, 1, {0x89}},
    {0x05B, 1, {0xAB}},
    {0x05C, 1, {0xCD}},
    {0x05D, 1, {0xEF}},
    {0x05E, 1, {0x11}},
    {0x05F, 1, {0x01}},
    {0x060, 1, {0x00}},
    {0x061, 1, {0x15}},
    {0x062, 1, {0x14}},
    {0x063, 1, {0x0C}},
    {0x064, 1, {0x0D}},
    {0x065, 1, {0x0E}},
    {0x066, 1, {0x0F}},
    {0x067, 1, {0x06}},
    {0x068, 1, {0x02}},
    {0x069, 1, {0x02}},
    {0x06A, 1, {0x02}},
    {0x06B, 1, {0x02}},
    {0x06C, 1, {0x02}},
    {0x06D, 1, {0x02}},
    {0x06E, 1, {0x08}},
    {0x06F, 1, {0x02}},
    {0x070, 1, {0x02}},
    {0x071, 1, {0x02}},
    {0x072, 1, {0x02}},
    {0x073, 1, {0x02}},
    {0x074, 1, {0x02}},
    {0x075, 1, {0x01}},
    {0x076, 1, {0x00}},
    {0x077, 1, {0x15}},
    {0x078, 1, {0x14}},
    {0x079, 1, {0x0C}},
    {0x07A, 1, {0x0D}},
    {0x07B, 1, {0x0E}},
    {0x07C, 1, {0x0F}},
    {0x07D, 1, {0x08}},
    {0x07E, 1, {0x02}},
    {0x07F, 1, {0x02}},
    {0x080, 1, {0x02}},
    {0x081, 1, {0x02}},
    {0x082, 1, {0x02}},
    {0x083, 1, {0x02}},
    {0x084, 1, {0x06}},
    {0x085, 1, {0x02}},
    {0x086, 1, {0x02}},
    {0x087, 1, {0x02}},
    {0x088, 1, {0x02}},
    {0x089, 1, {0x02}},
    {0x08A, 1, {0x02}},
    {0x0FF, 3, {0x98, 0x81, 0x04}},
    {0x06C, 1, {0x15}},
    {0x06E, 1, {0x2A}},
    {0x06F, 1, {0x35}},
    {0x03A, 1, {0x24}},
    {0x08D, 1, {0x14}},
    {0x087, 1, {0xBA}},
    {0x026, 1, {0x76}},
    {0x0B2, 1, {0xD1}},
    {0x035, 1, {0x1F}},
    {0x039, 1, {0x00}},
    {0x0B5, 1, {0x27}},
    {0x031, 1, {0x75}},
    {0x03B, 1, {0x98}},
    {0x030, 1, {0x03}},
    {0x033, 1, {0x14}},
    {0x038, 1, {0x02}},
    {0x07A, 1, {0x00}},
    {0x0FF, 3, {0x98, 0x81, 0x01}},
    {0x022, 1, {0x0A}},
    {0x031, 1, {0x00}},
    {0x035, 1, {0x07}},
    {0x053, 1, {0x6E}},
    {0x055, 1, {0x40}},
    {0x050, 1, {0x85}},
    {0x051, 1, {0x85}},
    {0x060, 1, {0x1F}},
    {0x062, 1, {0x07}},
    {0x063, 1, {0x00}},
    {0x0A0, 1, {0x08}},
    {0x0A1, 1, {0x14}},
    {0x0A2, 1, {0x1E}},
    {0x0A3, 1, {0x12}},
    {0x0A4, 1, {0x13}},
    {0x0A5, 1, {0x24}},
    {0x0A6, 1, {0x18}},
    {0x0A7, 1, {0x1A}},
    {0x0A8, 1, {0x56}},
    {0x0A9, 1, {0x18}},
    {0x0AA, 1, {0x25}},
    {0x0AB, 1, {0x57}},
    {0x0AC, 1, {0x22}},
    {0x0AD, 1, {0x24}},
    {0x0AE, 1, {0x58}},
    {0x0AF, 1, {0x2B}},
    {0x0B0, 1, {0x2E}},
    {0x0B1, 1, {0x4D}},
    {0x0B2, 1, {0x5B}},
    {0x0B3, 1, {0x3F}},
    {0x0C0, 1, {0x08}},
    {0x0C1, 1, {0x14}},
    {0x0C2, 1, {0x1E}},
    {0x0C3, 1, {0x12}},
    {0x0C4, 1, {0x13}},
    {0x0C5, 1, {0x24}},
    {0x0C6, 1, {0x17}},
    {0x0C7, 1, {0x1A}},
    {0x0C8, 1, {0x56}},
    {0x0C9, 1, {0x18}},
    {0x0CA, 1, {0x25}},
    {0x0CB, 1, {0x56}},
    {0x0CC, 1, {0x22}},
    {0x0CD, 1, {0x24}},
    {0x0CE, 1, {0x58}},
    {0x0CF, 1, {0x2B}},
    {0x0D0, 1, {0x2D}},
    {0x0D1, 1, {0x4E}},
    {0x0D2, 1, {0x5C}},
    {0x0D3, 1, {0x3F}},
    {0x0FF, 3, {0x98, 0x81, 0x00}},
    {0x011, 0, {}},
    {0xFFE, 120, {}},
    {0x029, 0, {}},
    {0xFFE, 20, {}},
    {0x035, 0, {}},
    {0xFFF, 0, {}},
};

static const struct t482l_lcm_cmd acer_suspend_sequence[] = {
    { 0x28, 0, { 0 } },
    { T482L_DSI_CMD_DELAY_MS, 20, { 0 } },
    { 0x10, 0, { 0 } },
    { T482L_DSI_CMD_DELAY_MS, 120, { 0 } },
    { T482L_REGFLAG_END, 0, { 0 } },
};

static void t482l_push_table(void *cmdq,
                             const struct t482l_lcm_cmd *table,
                             unsigned int count)
{
    unsigned int i;

    for (i = 0; i < count; i++) {
        switch (table[i].cmd) {
        case T482L_REGFLAG_DELAY:
            MDELAY(table[i].count);
            break;
        case T482L_REGFLAG_END:
            return;
        case T482L_DSI_CMD_DELAY_MS:
            MDELAY(table[i].count);
            break;
        default:
            lcm_util.dsi_set_cmdq_V22(cmdq, table[i].cmd,
                                      table[i].count,
                                      (unsigned char *)table[i].para_list,
                                      1);
            break;
        }
    }
}

static void t482l_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(lcm_util));
}

static void t482l_get_params(struct LCM_PARAMS *params)
{
    memset(params, 0, sizeof(*params));
    params->type = LCM_TYPE_DSI;
    params->width = T482L_WIDTH;
    params->height = T482L_HEIGHT;
    params->physical_width = 108;
    params->physical_height = 173;
    params->physical_width_um = 108000;
    params->physical_height_um = 173000;
    params->density = 213;

    params->dsi.mode = BURST_VDO_MODE;
    params->dsi.LANE_NUM = LCM_FOUR_LANE;
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;
    params->dsi.packet_size = 256;
    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.vertical_sync_active = 4;
    params->dsi.vertical_backporch = 4;
    params->dsi.vertical_frontporch = 8;
    params->dsi.vertical_frontporch_for_low_power = 0;
    params->dsi.vertical_active_line = T482L_HEIGHT;
    params->dsi.horizontal_sync_active = 16;
    params->dsi.horizontal_backporch = 48;
    params->dsi.horizontal_frontporch = 16;
    params->dsi.horizontal_active_pixel = T482L_WIDTH;
    params->dsi.PLL_CLOCK = 222;
    params->dsi.PLL_CK_CMD = 222;
    params->dsi.PLL_CK_VDO = 222;
    params->dsi.clk_lp_per_line_enable = 0;

    /* Stock reports a different DCS status value; skip ESD-triggered resets. */
    params->dsi.esd_check_enable = 0;
    params->dsi.customization_esd_check_enable = 0;
    /*
     * The Acer T482L panel (ILI9881C) has no hardware rounded corners.
     * Enabling round_corner_en causes the display stack to reserve corner
     * bitmap layers and inset the visible framebuffer area, producing the
     * top/bottom crop observed on-device. Keep this disabled.
     */
    params->round_corner_en = 0;
}

static void t482l_init_power(void)
{
    int ret = display_bias_enable();

    if (ret)
        pr_warn("t482l-lcm: stock bias helper returned %d\n", ret);
}

static void t482l_suspend_power(void)
{
    display_bias_disable();
}

static void t482l_resume_power(void)
{
    t482l_init_power();
}

static void t482l_write_backlight(void *cmdq)
{
    unsigned char value = min(t482l_backlight_level, 255U);

    lcm_util.dsi_set_cmdq_V22(cmdq, 0x51, 1, &value, 1);
}

static void t482l_init(void)
{
    lcm_util.set_reset_pin(0);
    MDELAY(15);
    lcm_util.set_reset_pin(1);
    MDELAY(1);
    lcm_util.set_reset_pin(0);
    MDELAY(10);
    lcm_util.set_reset_pin(1);
    MDELAY(10);

    t482l_push_table(NULL, acer_init_sequence,
                     ARRAY_SIZE(acer_init_sequence));
    t482l_write_backlight(NULL);
}

static void t482l_suspend(void)
{
    t482l_push_table(NULL, acer_suspend_sequence,
                     ARRAY_SIZE(acer_suspend_sequence));
}

static void t482l_resume(void)
{
    t482l_init();
}

static unsigned int t482l_compare_id(void)
{
    /* This is the sole panel compiled for the T482L target. */
    return 1;
}

static unsigned int t482l_esd_check(void)
{
    return 0;
}

static void t482l_set_backlight(void *cmdq, unsigned int level)
{
    t482l_backlight_level = min(level, 255U);
    t482l_write_backlight(cmdq);
}

struct LCM_DRIVER acer_t482l_ili9881_lcm_drv = {
    .name = "ili9881_bns080h030hd981d31d_wxga_hsd8",
    .set_util_funcs = t482l_set_util_funcs,
    .get_params = t482l_get_params,
    .init = t482l_init,
    .suspend = t482l_suspend,
    .resume = t482l_resume,
    .init_power = t482l_init_power,
    .suspend_power = t482l_suspend_power,
    .resume_power = t482l_resume_power,
    .compare_id = t482l_compare_id,
    .esd_check = t482l_esd_check,
    .set_backlight_cmdq = t482l_set_backlight,
};
