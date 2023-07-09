/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#if defined(OS_DRM_DRMP_H_EXIST)
#include <drm/drmP.h>
#endif
#include <drm/drm_atomic_helper.h>
#if defined(OS_DRM_DRM_DP_HELPER_H_EXIST)
#include <drm/drm_dp_helper.h>
#elif defined(OS_DRM_DP_DRM_DP_HELPER_H_EXIST)
#include <drm/dp/drm_dp_helper.h>
#elif defined(OS_DRM_DISPLAY_DRM_DP_HELPER_H_EXIST)
#include <drm/display/drm_dp_helper.h>
#endif
#include <drm/drm_edid.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_print.h>
#if defined(OS_DRM_DRM_PROBE_HELPER_H_EXIST)
#include <drm/drm_probe_helper.h>
#endif

#include <linux/component.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <sound/hdmi-codec.h>

#include "mtgpu_drv.h"
#include "mtgpu_board_cfg.h"
#include "phy-dp.h"
#include "mtgpu_dp_common.h"
#include "mtgpu_phy_common.h"

static const u8 display_port_type[] = {
	DRM_MODE_CONNECTOR_Unknown,    /*PORT_DISABLED*/
	DRM_MODE_CONNECTOR_DisplayPort,/*PORT_TYPE_DP*/
	DRM_MODE_CONNECTOR_eDP,        /*PORT_TYPE_EDP*/
	DRM_MODE_CONNECTOR_VGA,        /*PORT_TYPE_DP2VGA*/
	DRM_MODE_CONNECTOR_HDMIA,      /*PORT_TYPE_DP2HDMI*/
	DRM_MODE_CONNECTOR_LVDS        /*PORT_TYPE_DP2LVDS*/
};

static ssize_t
mtgpu_dp_aux_transfer(struct drm_dp_aux *aux, struct drm_dp_aux_msg *msg)
{
	int ret = 0;
	struct mtgpu_dp *dp = aux_to_mtgpu_dp(aux);

	/* FIXME: We should check the msg->request first! */

	if (!dp->connected)
		return -EIO;

	if (msg->request & BIT(0))
		ret = dp->core->aux_read(&dp->ctx, msg->request, msg->address,
					msg->buffer, msg->size, &msg->reply);
	else
		ret = dp->core->aux_write(&dp->ctx, msg->request, msg->address,
					 msg->buffer, msg->size, &msg->reply);

	if (ret) {
		DRM_DEV_INFO(dp->dev, "WARNING: try to do aux transfer (%d)\n", ret);
		return ret;
	}

	return msg->size;
}

static enum drm_connector_status
mtgpu_dp_connector_detect(struct drm_connector *connector, bool force)
{
	struct mtgpu_dp *dp = connector_to_mtgpu_dp(connector);

	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	if (!dp->core->is_plugin) {
		DRM_DEV_ERROR(dp->dev, "hotplug detect callback is not implemented\n");
		return connector_status_disconnected;
	}

	dp->connected = dp->core->is_plugin(&dp->ctx);

	if (dp->connected && !dp->edid) {
		dp->edid = drm_get_edid(connector, &dp->aux->ddc);
		if (!dp->edid) {
			DRM_DEV_ERROR(dp->dev, "failed to get edid\n");
			drm_connector_update_edid_property(connector, NULL);
		} else {
			drm_connector_update_edid_property(connector, dp->edid);
		}
	} else if (!dp->connected && dp->edid) {
		kfree(dp->edid);
		dp->edid = NULL;
	}

	return dp->connected ? connector_status_connected : connector_status_disconnected;
}

static int mtgpu_dp_connector_get_modes(struct drm_connector *connector)
{
	struct mtgpu_dp *dp = connector_to_mtgpu_dp(connector);

#if defined(CONFIG_QUYUAN2_HAPS)
	int count;

	DRM_DEV_INFO(dp->dev, "%s()\n", __func__);

	count = drm_add_modes_noedid(connector, 1024, 768);
	drm_set_preferred_mode(connector, 640, 480);

	return count;
#else
	int ret;

	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	if (!dp->edid) {
		DRM_DEV_ERROR(dp->dev, "no edid\n");

		drm_connector_update_edid_property(connector, NULL);

		return -ENODEV;
	}

	ret = drm_add_edid_modes(connector, dp->edid);

	return ret;
#endif
}

static struct drm_encoder *
mtgpu_dp_connector_best_encoder(struct drm_connector *connector)
{
	struct mtgpu_dp *dp = connector_to_mtgpu_dp(connector);

	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	return dp->encoder;
}

static int mtgpu_dp_mode_supported_by_product(struct mtgpu_dp *dp,
					      struct drm_display_mode *mode)
{
	if (mode->clock / 100 > dp->ctx.max_pclk_100khz)
		return MODE_CLOCK_HIGH;

	if (mode->hdisplay > dp->ctx.max_hres)
		return MODE_H_ILLEGAL;

	if (mode->vdisplay > dp->ctx.max_vres)
		return MODE_V_ILLEGAL;

	return MODE_OK;
}

static int mtgpu_dp_mode_compare(struct drm_display_mode *a,
				 struct drm_display_mode *b)
{
	int diff;

	diff = b->hdisplay * b->vdisplay - a->hdisplay * a->vdisplay;
	if (diff)
		return diff;

	diff = drm_mode_vrefresh(b) - drm_mode_vrefresh(a);
	if (diff)
		return diff;

	diff = b->clock - a->clock;
	return diff;
}

static void mtgpu_dp_set_preferred_mode(struct drm_connector *connector,
					struct drm_display_mode *filtered_mode)
{
	struct drm_display_mode *mode;
	struct drm_display_mode tmp_best_mode = {0};
	struct drm_display_mode *preferred_mode = NULL;
	struct mtgpu_dp *dp = connector_to_mtgpu_dp(connector);

	if (list_empty(&connector->modes))
		return;

	/* Find the best mode and set its type preferred. */
	list_for_each_entry(mode, &connector->modes, head) {
		if ((mtgpu_dp_mode_supported_by_product(dp, mode) != MODE_OK) ||
		    mode->status != MODE_OK ||
		    drm_mode_equal(mode, filtered_mode))
			continue;

		if (mtgpu_dp_mode_compare(&tmp_best_mode, mode) > 0) {
			memcpy(&tmp_best_mode, mode, sizeof(struct drm_display_mode));
			preferred_mode = mode;
		}
	}

	if (preferred_mode)
		preferred_mode->type |= DRM_MODE_TYPE_PREFERRED;
}

static int mtgpu_dp_connector_mode_valid(struct drm_connector *connector,
					 struct drm_display_mode *mode)
{
	int mode_status;
	struct mtgpu_dp *dp = connector_to_mtgpu_dp(connector);

	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	/* check if the mode can be supported by product. */
	mode_status = mtgpu_dp_mode_supported_by_product(dp, mode);
	if (mode_status != MODE_OK) {
		if (mode->type & DRM_MODE_TYPE_PREFERRED) {
			/*
			 * When the preferred mode is unsupported,
			 * we should set the remaining best mode preferred.
			 */
			mtgpu_dp_set_preferred_mode(connector, mode);
		}

		return mode_status;
	}

	return mode_status;
}

static const struct drm_connector_funcs mtgpu_dp_connector_funcs = {
	.detect			= mtgpu_dp_connector_detect,
	.fill_modes		= drm_helper_probe_single_connector_modes,
	.destroy		= drm_connector_cleanup,
	.atomic_duplicate_state	= drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_connector_destroy_state,
	.reset			= drm_atomic_helper_connector_reset,
};

static const struct drm_connector_helper_funcs
mtgpu_dp_connector_helper_funcs = {
	.get_modes	= mtgpu_dp_connector_get_modes,
	.best_encoder	= mtgpu_dp_connector_best_encoder,
	.mode_valid	= mtgpu_dp_connector_mode_valid,
};

static void
mtgpu_dp_encoder_atomic_mode_set(struct drm_encoder *encoder,
				 struct drm_crtc_state *crtc_state,
				 struct drm_connector_state *connector_state)
{
	struct mtgpu_dp *dp = encoder_to_mtgpu_dp(encoder);
	struct drm_display_mode *adjusted_mode = &crtc_state->adjusted_mode;

	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	drm_display_mode_to_videomode(adjusted_mode, dp->ctx.vm);
	dp->ctx.bpp = 24;
	dp->ctx.pclk = adjusted_mode->clock;

	return;
}

static int
mtgpu_dp_encoder_atomic_check(struct drm_encoder *encoder,
			      struct drm_crtc_state *crtc_state,
			      struct drm_connector_state *conn_state)
{
	struct mtgpu_dp *dp = encoder_to_mtgpu_dp(encoder);

	/* TODO: maybe we should check something here */
	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	return 0;
}

static const struct drm_encoder_helper_funcs mtgpu_dp_encoder_helper_funcs = {
	.enable			= mtgpu_dp_encoder_enable,
	.disable		= mtgpu_dp_encoder_disable,
	.atomic_mode_set	= mtgpu_dp_encoder_atomic_mode_set,
	.atomic_check		= mtgpu_dp_encoder_atomic_check,
};

static const struct drm_encoder_funcs mtgpu_dp_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static int mtgpu_dp_drm_init(struct mtgpu_dp *dp, struct drm_device *drm)
{
	struct drm_encoder *encoder = dp->encoder;
	struct drm_connector *connector = dp->connector;
	int ret;

	dp->aux->name = "MTGPU DP AUX";
	dp->aux->dev = dp->dev;
	dp->aux->transfer = mtgpu_dp_aux_transfer;

	ret = drm_dp_aux_register(dp->aux);
	if (ret) {
		DRM_DEV_ERROR(dp->dev, "failed to initialize DP aux\n");
		return ret;
	}

	/*
	* WARNING:
	* The possible_crtcs mask is in the logical order of dispc platform device
	* registration, not the dispc hardware ID order. So if the dispc platform
	* device count changed, this possible_crtcs mask should also be changed.
	*/
	encoder->possible_crtcs = 1 << dp->ctx.id;
	ret = drm_encoder_init(drm, encoder, &mtgpu_dp_encoder_funcs,
			       DRM_MODE_ENCODER_TMDS, NULL);
	if (ret) {
		DRM_DEV_ERROR(dp->dev, "failed to create DisplayPort encoder\n");
		return ret;
	}
	drm_encoder_helper_add(encoder, &mtgpu_dp_encoder_helper_funcs);

	connector->polled = DRM_CONNECTOR_POLL_HPD;

	if (dp->port_type >= ARRAY_SIZE(display_port_type)) {
		DRM_DEV_INFO(dp->dev,
			     "WARNING: invalid dp->port_type 0x%x, use DisplayPort type as default\n",
			     dp->port_type);
		dp->port_type = PORT_TYPE_DP;
	}

#if defined(OS_FUNC_DRM_CONNECTOR_INIT_WITH_DDC_EXIST)
	ret = drm_connector_init_with_ddc(drm, connector,
					  &mtgpu_dp_connector_funcs,
					  display_port_type[dp->port_type],
					  &dp->aux->ddc);
#else
	ret = drm_connector_init(drm, connector,
				 &mtgpu_dp_connector_funcs,
				 display_port_type[dp->port_type]);
#endif
	if (ret) {
		DRM_DEV_ERROR(dp->dev, "failed to create DisplayPort connector\n");
		return ret;
	}

	drm_connector_helper_add(connector, &mtgpu_dp_connector_helper_funcs);
	drm_connector_register(connector);
	drm_connector_attach_encoder(connector, encoder);

	return 0;
}

/*
 * DP audio codec callbacks
 */
static int mtgpu_dp_audio_hw_params(struct device *dev, void *data,
				    struct hdmi_codec_daifmt *daifmt,
				    struct hdmi_codec_params *params)
{
	struct mtgpu_dp *dp = data;
	int ret = -1;

	if (!params) {
		DRM_DEV_ERROR(dp->dev, "%s: audio params is NULL.\n", __func__);
		return -EINVAL;
	}

	if (!dp->enabled || !dp->ctx.bw_code) {
		DRM_DEV_INFO(dp->dev, "%s: dp stream has not enabled.\n", __func__);
		return -EPERM;
	}

	DRM_DEV_INFO(dp->dev, "%s()\n", __func__);

	DRM_DEV_DEBUG(dp->dev, "%s: %u Hz, %d bit, %d channels\n", __func__,
		      params->sample_rate, params->sample_width,
		      params->cea.channels);

	if (dp->core->audio_set_param)
		ret = dp->core->audio_set_param(&dp->ctx, params);

	return ret;
}

static int mtgpu_dp_audio_startup(struct device *dev, void *data)
{
	struct mtgpu_dp *dp = data;

	DRM_DEV_DEBUG(dp->dev, "%s()\n", __func__);

	if (dp->core->audio_enable)
		dp->core->audio_enable(&dp->ctx);

	dp->audio_enabled = true;

	return 0;
}

static void mtgpu_dp_audio_shutdown(struct device *dev, void *data)
{
	struct mtgpu_dp *dp = data;

	DRM_DEV_INFO(dp->dev, "%s()\n", __func__);

	if (dp->core->audio_disable)
		dp->core->audio_disable(&dp->ctx);

	dp->audio_enabled = false;
}

#if defined(OS_STRUCT_HDMI_CODEC_OPS_HAS_MUTE_STREAM)
static int mtgpu_dp_audio_mute(struct device *dev, void *data, bool enable, int direction)
#else
static int mtgpu_dp_audio_mute(struct device *dev, void *data, bool enable)
#endif
{
	struct mtgpu_dp *dp = data;

	DRM_DEV_INFO(dp->dev, "%s()\n", __func__);

	if (enable && dp->core->audio_mute)
		dp->core->audio_mute(&dp->ctx, enable);

	return 0;
}

static int mtgpu_dp_audio_get_eld(struct device *dev, void *data, uint8_t *buf, size_t len)
{
	struct mtgpu_dp *dp = data;

	DRM_DEV_INFO(dp->dev, "%s()\n", __func__);

	memcpy(buf, dp->connector->eld, min(sizeof(dp->connector->eld), len));

	return 0;
}

static int mtgpu_dp_audio_hook_plugged_cb(struct device *dev, void *data,
					    hdmi_codec_plugged_cb fn,
					    struct device *codec_dev)
{
	struct mtgpu_dp *dp = data;

	DRM_DEV_INFO(dp->dev, "%s()\n", __func__);

	mutex_lock(dp->codec_lock);
	dp->codec_plugged_cb = fn;
	dp->codec_dev = codec_dev;
	mutex_unlock(dp->codec_lock);

	if (dp->codec_plugged_cb && dp->codec_dev)
		dp->codec_plugged_cb(dp->codec_dev, dp->enabled);

	return 0;
}

#if defined(OS_STRUCT_HDMI_CODEC_OPS_HAS_HOOK_PLUGGED_CB)
static const struct hdmi_codec_ops mtgpu_dp_audio_codec_ops = {
	.hw_params = mtgpu_dp_audio_hw_params,
	.audio_startup = mtgpu_dp_audio_startup,
	.audio_shutdown = mtgpu_dp_audio_shutdown,
#if defined(OS_STRUCT_HDMI_CODEC_OPS_HAS_MUTE_STREAM)
	.mute_stream = mtgpu_dp_audio_mute,
#else
	.digital_mute = mtgpu_dp_audio_mute,
#endif
	.get_eld = mtgpu_dp_audio_get_eld,
	.hook_plugged_cb = mtgpu_dp_audio_hook_plugged_cb,
};
#else
typedef int (*hook_plugged_callback)(struct device *dev, void *data,
					    hdmi_codec_plugged_cb fn,
					    struct device *codec_dev);

struct mtgpu_codec_ops {
	struct hdmi_codec_ops old_dp_audio_codec_ops;
	hook_plugged_callback hook_plugged_cb;
};

static const struct mtgpu_codec_ops mtgpu_dp_audio_codec_ops = {
	.old_dp_audio_codec_ops = {
	.hw_params = mtgpu_dp_audio_hw_params,
	.audio_startup = mtgpu_dp_audio_startup,
	.audio_shutdown = mtgpu_dp_audio_shutdown,
	.digital_mute = mtgpu_dp_audio_mute,
	.get_eld = mtgpu_dp_audio_get_eld,
	},
	.hook_plugged_cb = mtgpu_dp_audio_hook_plugged_cb,
};
#endif

static int mtgpu_dp_audio_register(struct mtgpu_dp *dp)
{
	struct hdmi_codec_pdata codec_data = {
		.ops = (struct hdmi_codec_ops *)&mtgpu_dp_audio_codec_ops,
		.max_i2s_channels = 2,
		.i2s = 1,
		.data = dp,
	};
	char dev_name[64];
	const char *name = dev_name;

	sprintf(dev_name, "mtgpu-dp-audio-codec-%d", dp->ctx.id);

	dp->dp_audio = platform_device_register_data(dp->dev, name,
						     PLATFORM_DEVID_AUTO, &codec_data,
						     sizeof(codec_data));
	if (IS_ERR(dp->dp_audio))
		return PTR_ERR(dp->dp_audio);

	mutex_init(dp->codec_lock);

	return 0;
}

static void mtgpu_dp_audio_unregister(struct mtgpu_dp *dp)
{
	if (dp->dp_audio) {
		platform_device_unregister(dp->dp_audio);
		dp->dp_audio = NULL;
	}
}

static int mtgpu_dp_component_bind(struct device *dev,
				   struct device *master, void *data)
{
	struct mtgpu_dp_platform_data *pdata = dev_get_platdata(dev);
	struct platform_device *pdev = to_platform_device(dev);
	struct drm_device *drm = data;
	struct mtgpu_phy *dp_phy;
	struct mtgpu_dp *dp;
	struct resource *res;
	int ret;
	struct mtgpu_dp_chip *chip;

	dp = kzalloc(sizeof(*dp), GFP_KERNEL);
	if (!dp)
		return -ENOMEM;

	dp->dev = dev;

	dp->ctx.waitq = kzalloc(sizeof(*dp->ctx.waitq), GFP_KERNEL);
	if (!dp->ctx.waitq) {
		DRM_DEV_ERROR(dev, "failed to create waitq\n");
		ret = -ENOMEM;
		goto err_free_dp;
	}

	ret = mtgpu_dp_kernel_struct_create(dp);
	if (ret) {
		DRM_DEV_ERROR(dev, "failed to create dp kernel struct\n");
		ret = -ENOMEM;
		goto err_free_dp;
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "dp-regs");
	if (!res) {
		DRM_DEV_ERROR(dev, "failed to get dp-regs\n");
		ret = -EIO;
		goto err_free_dp;
	}

	dp->ctx.regs = devm_ioremap(dev, res->start, resource_size(res));
	if (IS_ERR(dp->ctx.regs)) {
		ret = PTR_ERR(dp->ctx.regs);
		goto err_free_dp;
	}

	if (pdata->soc_gen == GPU_SOC_GEN2 || pdata->soc_gen == GPU_SOC_GEN3) {
		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "tzc-regs");
		if (!res) {
			DRM_DEV_ERROR(dev, "failed to get display tzc-regs\n");
			ret = -EIO;
			goto err_free_dp;
		}

		dp->ctx.tzc_regs = devm_ioremap(dev, res->start, resource_size(res));
		if (IS_ERR(dp->ctx.tzc_regs)) {
			ret = PTR_ERR(dp->ctx.tzc_regs);
			goto err_free_dp;
		}

		res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "amt-regs");
		if (!res) {
			DRM_DEV_ERROR(dev, "failed to get display amt-regs\n");
			ret = -EIO;
			goto err_free_dp;
		}

		dp->ctx.amt_regs = devm_ioremap(dev, res->start, resource_size(res));
		if (IS_ERR(dp->ctx.amt_regs)) {
			ret = PTR_ERR(dp->ctx.amt_regs);
			goto err_free_dp;
		}
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "glb-regs");
	if (!res) {
		DRM_DEV_ERROR(dev, "failed to get display glb-regs\n");
		ret = -EIO;
		goto err_free_dp;
	}

	dp->ctx.glb_regs = devm_ioremap(dev, res->start, resource_size(res));
	if (IS_ERR(dp->ctx.glb_regs)) {
		ret = PTR_ERR(dp->ctx.glb_regs);
		goto err_free_dp;
	}

	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		DRM_DEV_ERROR(dev, "failed to get DisplayPort irq number\n");
		ret = -EIO;
		goto err_free_dp;
	}

	ret = mtgpu_set_interrupt_handler(dev->parent->parent, res->start,
					  mtgpu_dp_irq_handler, dp);
	if (ret) {
		DRM_DEV_ERROR(dev, "failed to register DisplayPort irq handler\n");
		ret = -EINVAL;
		goto err_free_dp;
	}

	dp->phy = devm_phy_get(dev, "dp-phy");
	if (IS_ERR_OR_NULL(dp->phy)) {
		ret = PTR_ERR(dp->phy);
		DRM_DEV_ERROR(dev, "failed to get dp-phy for %s, ret = %d\n", dev_name(dev), ret);
		goto err_free_dp;
	}
	DRM_DEV_DEBUG(dev, "DP Controller driver get phy device successfully\n");

	dp_phy = (struct mtgpu_phy *)phy_get_drvdata(dp->phy);
	dp_phy->ctx.dp_regs = dp->ctx.regs;
	dp_phy->ctx.glb_regs = dp->ctx.glb_regs;

	dp->ctx.irq = res->start;
	dp->ctx.id = pdata->id;
	dp->ctx.max_hres = pdata->max_hres;
	dp->ctx.max_vres = pdata->max_vres;
	dp->ctx.max_pclk_100khz = pdata->max_pclk_100khz;
	dp->port_type = pdata->port_type;
	dev_set_drvdata(dev, dp);
	INIT_WORK(dp->hpd_work, mtgpu_dp_hpd_work);
	INIT_WORK(dp->hpd_irq_work, mtgpu_dp_hpd_irq_work);
	init_waitqueue_head(dp->ctx.waitq);

	switch (pdata->soc_gen) {
	case GPU_SOC_GEN1:
		chip = &mtgpu_dp_chip_sudi;
		break;
	case GPU_SOC_GEN2:
		chip = &mtgpu_dp_chip_qy1;
		break;
	case GPU_SOC_GEN3:
		chip = &mtgpu_dp_chip_qy2;
		break;
	default:
		DRM_DEV_ERROR(dev, "current SOC_GEN%d is not supported\n", pdata->soc_gen);
		ret = -ENOTSUPP;
		goto err_free_dp;
	}

	dp->core = chip->core;
	if (!dp->core) {
		DRM_DEV_ERROR(dev, "dp core ops is null\n");
		ret = -EINVAL;
		goto err_free_dp;
	}

	dp->glb = chip->glb;
	if (!dp->glb) {
		DRM_DEV_ERROR(dev, "dp glb ops is null\n");
		ret = -EINVAL;
		goto err_free_dp;
	}

	/* disable first in case it's enabled by bios */
	if (dp->core->disable)
		dp->core->disable(&dp->ctx);

	if (dp->glb->init)
		dp->glb->init(&dp->ctx);

	if (dp->core->hw_init)
		dp->core->hw_init(&dp->ctx);

	phy_init(dp->phy);

	ret = mtgpu_dp_audio_register(dp);
	if (ret) {
		DRM_DEV_ERROR(dev, "failed to register DP audio device: %d\n", ret);
		goto err_free_dp;
	}

	mtgpu_dp_drm_init(dp, drm);

	ret = mtgpu_enable_interrupt(dev->parent->parent, res->start);
	if (ret) {
		DRM_DEV_ERROR(dev, "failed to enable DisplayPort irq\n");
		ret = -EINVAL;
		goto err_free_dp;
	}

	DRM_DEV_INFO(dev, "mtgpu DisplayPort driver loaded successfully\n");

	return 0;

err_free_dp:
	kfree(dp->ctx.waitq);
	kfree(dp);

	return ret;
}

static void mtgpu_dp_component_unbind(struct device *dev,
				      struct device *master, void *data)
{
	int ret;
	struct mtgpu_dp *dp = dev_get_drvdata(dev);

	phy_exit(dp->phy);

	if (dp->core->hw_deinit)
		dp->core->hw_deinit(&dp->ctx);

	ret = mtgpu_disable_interrupt(dev->parent->parent, dp->ctx.irq);
	if (ret)
		DRM_DEV_ERROR(dev, "failed to disable DisplayPort irq\n");

	ret = mtgpu_set_interrupt_handler(dev->parent->parent, dp->ctx.irq, NULL, NULL);
	if (ret)
		DRM_DEV_ERROR(dev, "failed to deregister DisplayPort irq handler\n");

	mtgpu_dp_audio_unregister(dp);

	drm_dp_aux_unregister(dp->aux);

	devm_phy_put(dev, dp->phy);

	mtgpu_dp_kernel_struct_destroy(dp);
	kfree(dp->ctx.waitq);
	kfree(dp);

	DRM_DEV_INFO(dev, "unload mtgpu DisplayPort driver\n");
}

static const struct component_ops mtgpu_dp_component_ops = {
	.bind   = mtgpu_dp_component_bind,
	.unbind = mtgpu_dp_component_unbind,
};

static int mtgpu_dp_probe(struct platform_device *pdev)
{
	return component_add(&pdev->dev, &mtgpu_dp_component_ops);
}

static int mtgpu_dp_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &mtgpu_dp_component_ops);
	return 0;
}

static struct platform_device_id mtgpu_dp_device_id[] = {
	{ .name = MTGPU_DEVICE_NAME_DP, },
	{ },
};

static int mtgpu_dp_resume(struct device *dev)
{
	struct mtgpu_dp *dp = dev_get_drvdata(dev);

	DRM_DEV_INFO(dp->dev, "mtgpu dp resume\n");

	/* disable first in case it's enabled by bios */
	if (dp->core->disable)
		dp->core->disable(&dp->ctx);

	if (dp->glb->reset)
		dp->glb->reset(&dp->ctx);

	if (dp->core->hw_init)
		dp->core->hw_init(&dp->ctx);

	phy_init(dp->phy);

	/*
	* The audio service (eg. pluseaudio) will not response to the
	* codec_plugged_cb() in S3/S4 suspend and resume stage since
	* it was freezeed. Restore the audio status if it was enabled
	* before encoder suspend.
	*/
	if (dp->core->audio_enable && dp->audio_enabled)
		dp->core->audio_enable(&dp->ctx);

	if (dp->core->is_plugin)
		dp->connected = dp->core->is_plugin(&dp->ctx);

	return 0;
}

static int mtgpu_dp_suspend(struct device *dev)
{
	struct mtgpu_dp *dp = dev_get_drvdata(dev);

	phy_exit(dp->phy);

	return 0;
}

const struct dev_pm_ops mtgpu_dp_pm_ops = {
	/* this will be called before mtgpu_drm_resume */
	.resume_early	= mtgpu_dp_resume,
	.restore_early	= mtgpu_dp_resume,
	.suspend	= mtgpu_dp_suspend,
	.freeze		= mtgpu_dp_suspend,
};

struct platform_driver mtgpu_dp_driver = {
	.probe    = mtgpu_dp_probe,
	.remove   = mtgpu_dp_remove,
	.driver   = {
		.owner  = THIS_MODULE,
		.name   = "mtgpu-dp-drv",
		.pm	= &mtgpu_dp_pm_ops,
	},
	.id_table = mtgpu_dp_device_id,
};

