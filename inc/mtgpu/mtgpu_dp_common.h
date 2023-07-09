/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */
#ifndef _MTGPU_DP_COMMON_H_
#define _MTGPU_DP_COMMON_H_

#include "mtgpu_display_debug.h"
#include "mtgpu_dpcd_defs.h"
#include "os-interface-drm.h"

#define MTGPU_DP_IRQ_TYPE_HPD_EVENT	BIT(0)
#define MTGPU_DP_IRQ_TYPE_HPD_IRQ	BIT(1)

#define DP_SINGLE_PIX_MODE	0
#define DP_DUAL_PIX_MODE	1
#define DP_QUAD_PIX_MODE	2

typedef void (*hdmi_codec_plugged_cb)(struct device *dev, bool plugged);

struct wait_queue_head;
typedef struct wait_queue_head wait_queue_head_t;

struct hdmi_codec_params;

enum dpcd_phy_test_pattern {
	PHY_TEST_PATTERN_NONE = 0,
	PHY_TEST_PATTERN_D10_2,
	PHY_TEST_PATTERN_SYMBOL_ERROR,
	PHY_TEST_PATTERN_PRBS7,
	PHY_TEST_PATTERN_80BIT_CUSTOM,/* For DP1.2 only */
	PHY_TEST_PATTERN_CP2520_1,
	PHY_TEST_PATTERN_CP2520_2,
	PHY_TEST_PATTERN_CP2520_3, /* same as TPS4 */
};

struct mtgpu_dp_ctx {
	void __iomem *regs;
	void __iomem *glb_regs;
	void __iomem *tzc_regs;
	void __iomem *amt_regs;
	int irq;
	int id;
	struct videomode *vm;
	u8 bw_code;
	u8 lane_cnt;
	int max_rate;
	u8 max_lanes;
	u16 max_hres;
	u16 max_vres;
	u16 max_pclk_100khz;
	int pclk;
	u8 bpp;
	u32 event_flags;
	u32 pixel_mode;
	wait_queue_head_t *waitq;
	void *private;
};

struct mtgpu_dp_ops {
	void (*hw_init)(struct mtgpu_dp_ctx *ctx);
	void (*hw_deinit)(struct mtgpu_dp_ctx *ctx);
	void (*config)(struct mtgpu_dp_ctx *ctx);
	void (*enable)(struct mtgpu_dp_ctx *ctx);
	void (*disable)(struct mtgpu_dp_ctx *ctx);
	u32 (*isr)(struct mtgpu_dp_ctx *ctx);
	void (*training_pattern)(struct mtgpu_dp_ctx *ctx, u8 patten);
	void (*power_up)(struct mtgpu_dp_ctx *ctx);
	void (*qual_pattern)(struct mtgpu_dp_ctx *ctx, u8 pattern, u8 *data);
	void (*vswing_preemph)(struct mtgpu_dp_ctx *ctx, u8 vswing, u8 preemph);
	void (*enhanced_frame)(struct mtgpu_dp_ctx *ctx, bool enable);
	void (*stream_config)(struct mtgpu_dp_ctx *ctx);
	void (*scramble)(struct mtgpu_dp_ctx *ctx, bool enable);
	void (*downspread)(struct mtgpu_dp_ctx *ctx, u8 data);
	void (*link_bandwidth)(struct mtgpu_dp_ctx *ctx, u8 bw);
	void (*lane_count)(struct mtgpu_dp_ctx *ctx, u8 count);
	bool (*is_plugin)(struct mtgpu_dp_ctx *ctx);
	int (*aux_read)(struct mtgpu_dp_ctx *ctx, u8 request, u16 addr,
			u8 *buf, u8 bytes, u8 *reply);
	int (*aux_write)(struct mtgpu_dp_ctx *ctx, u8 request, u16 addr,
			 u8 *buf, u8 bytes, u8 *reply);
	void (*audio_enable)(struct mtgpu_dp_ctx *ctx);
	void (*audio_disable)(struct mtgpu_dp_ctx *ctx);
	void (*audio_mute)(struct mtgpu_dp_ctx *ctx, bool enable);
	int (*audio_set_param)(struct mtgpu_dp_ctx *ctx, struct hdmi_codec_params *params);
};

struct mtgpu_dp_glb_ops {
	void (*init)(struct mtgpu_dp_ctx *ctx);
	void (*reset)(struct mtgpu_dp_ctx *ctx);
};

struct mtgpu_dp_chip {
	struct mtgpu_dp_ops *core;
	struct mtgpu_dp_glb_ops *glb;
};

struct mtgpu_dp {
	struct drm_encoder *encoder;
	struct drm_connector *connector;
	struct drm_dp_aux *aux;
	struct device *dev;
	struct mtgpu_dp_ctx ctx;
	struct mtgpu_dp_ops *core;
	struct mtgpu_dp_glb_ops *glb;
	struct phy *phy;
	struct work_struct *hpd_work;
	struct work_struct *hpd_irq_work;
	u8 dpcd[DP_RECEIVER_CAP_SIZE];
	u8 train_set[4];
	u8 port_type;

	/* audio codec */
	struct platform_device *dp_audio;
	hdmi_codec_plugged_cb codec_plugged_cb;

	struct device *codec_dev;
	struct mutex *codec_lock; /* codec lock */
	struct edid *edid;
	bool connected;
	bool enabled;
	bool audio_enabled;
	bool encoder_enable_failed;
};

static inline
void dptx_reg_write(struct mtgpu_dp_ctx *ctx, int offset, u32 val)
{
	os_writel(val, ctx->regs + offset);
	/* dummy read to make post write take effect */
	os_readl(ctx->regs + offset);
	DP_DEBUG("offset = 0x%04x value = 0x%08x\n", offset, val);
}

static inline
u32 dptx_reg_read(struct mtgpu_dp_ctx *ctx, int offset)
{
	u32 val = os_readl(ctx->regs + offset);

	DP_DEBUG("offset = 0x%04x value = 0x%08x\n", offset, val);
	return val;
}

static inline
void dptx_tzc_reg_write(struct mtgpu_dp_ctx *ctx, int offset, u32 val)
{
	os_writel(val, ctx->tzc_regs + offset);
	/* dummy read to make post write take effect */
	os_readl(ctx->tzc_regs + offset);
	DP_DEBUG("offset = 0x%04x value = 0x%08x\n", offset, val);
}

static inline
void dptx_amt_reg_write(struct mtgpu_dp_ctx *ctx, int offset, u32 val)
{
	os_writel(val, ctx->amt_regs + offset);
	/* dummy read to make post write take effect */
	os_readl(ctx->amt_regs + offset);
	DP_DEBUG("offset = 0x%04x value = 0x%08x\n", offset, val);
}

static inline
void dptx_glb_reg_write(struct mtgpu_dp_ctx *ctx, int offset, u32 val)
{
	os_writel(val, ctx->glb_regs + offset);
	/* dummy read to make post write take effect */
	os_readl(ctx->glb_regs + offset);
	DP_DEBUG("offset = 0x%04x value = 0x%08x\n", offset, val);
}

static inline
u32 dptx_glb_reg_read(struct mtgpu_dp_ctx *ctx, int offset)
{
	u32 val = os_readl(ctx->glb_regs + offset);

	DP_DEBUG("offset = 0x%04x value = 0x%08x\n", offset, val);
	return val;
}

extern struct mtgpu_dp_chip mtgpu_dp_chip_sudi;
extern struct mtgpu_dp_chip mtgpu_dp_chip_qy1;
extern struct mtgpu_dp_chip mtgpu_dp_chip_qy2;

static inline
struct mtgpu_dp *encoder_to_mtgpu_dp(struct drm_encoder *encoder)
{
	return os_get_drm_encoder_drvdata(encoder);
}

static inline
struct mtgpu_dp *connector_to_mtgpu_dp(struct drm_connector *connector)
{
	return os_get_drm_connector_drvdata(connector);
}

static inline
struct mtgpu_dp *aux_to_mtgpu_dp(struct drm_dp_aux *aux)
{
	return os_get_drm_dp_aux_drvdata(aux);
}

static inline
struct mtgpu_dp *work_to_mtgpu_dp(struct work_struct *work)
{
	return os_get_work_drvdata(work);
}

void mtgpu_dp_encoder_enable(struct drm_encoder *encoder);
void mtgpu_dp_encoder_disable(struct drm_encoder *encoder);
void mtgpu_dp_hpd_work(struct work_struct *work);
void mtgpu_dp_hpd_irq_work(struct work_struct *work);
void mtgpu_dp_irq_handler(void *data);
int  mtgpu_dp_kernel_struct_create(struct mtgpu_dp *dp);
void mtgpu_dp_kernel_struct_destroy(struct mtgpu_dp *dp);

#endif /* _MTGPU_DP_COMMON_H_ */
