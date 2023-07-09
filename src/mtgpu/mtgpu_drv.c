/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

/*
 * This is a device driver for the mtgpu framework. It creates platform
 * devices inside the MT GPU, and exports functions to manage the
 * shared interrupt handling
 */

#if defined(OS_DRM_DRMP_H_EXIST)
#include <drm/drmP.h>
#else
#include <drm/drm_device.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <linux/device.h>
#endif

#include <linux/pci.h>
#include <linux/module.h>

#ifndef CONFIG_LOONGARCH
#include <asm/dmi.h>
#else
#include "dmi_loongarch.h"
#endif

#if defined(CONFIG_MTRR)
#include <asm/mtrr.h>
#endif

#include "mtgpu_drv.h"
#include "mtgpu_drm_drv.h"
#include "mtgpu_board_cfg.h"
#include "mtgpu_snd.h"
#ifdef SUPPORT_ION
#include "ion/ion.h"
#include "mtgpu_ion.h"
#endif
#include "mtgpu_procfs.h"
#include "mtgpu_drv_common.h"
#include "mtgpu_module_param.h"
#include "mtgpu_misc.h"
#include "mtlink.h"

MODULE_DESCRIPTION("MooreThreads mtgpu drm driver");
MODULE_LICENSE("Dual MIT/GPL");

#if defined(OS_STRUCT_PROC_OPS_EXIST)
#include <linux/proc_fs.h>
const struct proc_ops config_proc_ops = {
	.proc_open = mtgpu_proc_config_open,
	.proc_read = os_seq_read,
	.proc_write = mtgpu_proc_config_write,
	.proc_lseek = os_seq_lseek,
	.proc_release = os_single_release,
};

const struct proc_ops mpc_enable_proc_ops = {
	.proc_open = mtgpu_proc_mpc_enable_open,
	.proc_read = os_seq_read,
	.proc_write = mtgpu_proc_mpc_enable_write,
	.proc_lseek = os_seq_lseek,
	.proc_release = os_single_release,
};
#else
const struct file_operations config_proc_ops = {
	.open = mtgpu_proc_config_open,
	.read = os_seq_read,
	.write = mtgpu_proc_config_write,
	.llseek = os_seq_lseek,
	.release = os_single_release,
};

const struct file_operations mpc_enable_proc_ops = {
	.open = mtgpu_proc_mpc_enable_open,
	.read = os_seq_read,
	.write = mtgpu_proc_mpc_enable_write,
	.llseek = os_seq_lseek,
	.release = os_single_release,
};
#endif

static u64 mtgpu_get_vram_size(struct mtgpu_device *mtdev)
{
	if (mtdev->board_configs && mtdev->board_configs->board_cap.ddr_cfg.ddr_size)
		return mtdev->board_configs->board_cap.ddr_cfg.ddr_size;

	return pci_resource_len(mtdev->pdev, MTGPU_DDR_BAR);
}

static ssize_t mtgpu_info_show(struct device *dev,
			       struct device_attribute *attr, char *buf)
{
	struct mtgpu_device *mtdev = devres_find(dev, mtgpu_devres_release, NULL, NULL);

	return scnprintf(buf, PAGE_SIZE, "VRAM total size:0x%llx\n",
			 mtgpu_get_vram_size(mtdev));
}

struct device_attribute dev_attr_mtgpu_info = {
	.attr = {
		.name = "gpu-info",
		.mode = 0444
	},
	.show = mtgpu_info_show,
};

static pci_ers_result_t mtgpu_error_detected(struct pci_dev *pdev,
					     pci_channel_state_t state)
{
	/* TODO: need stop video gpu display to prevent the use of pcie */
	switch (state) {
	case pci_channel_io_normal:
		return PCI_ERS_RESULT_CAN_RECOVER;
	case pci_channel_io_frozen:
		return PCI_ERS_RESULT_NEED_RESET;
	case pci_channel_io_perm_failure:
		return PCI_ERS_RESULT_DISCONNECT;
	default:
		return PCI_ERS_RESULT_DISCONNECT;
	}

	return PCI_ERS_RESULT_NEED_RESET;
}

static pci_ers_result_t mtgpu_mmio_enabled(struct pci_dev *dev)
{
	dev_info(&dev->dev, "mmio enabled done\n");

	/* TODO - dump whatever for debugging purposes */

	/* This called only if mtgpu_error_detected returns
	 * PCI_ERS_RESULT_CAN_RECOVER. Read/write to the device still
	 * works, no need to reset slot.
	 */

	return PCI_ERS_RESULT_RECOVERED;
}

static pci_ers_result_t mtgpu_slot_reset(struct pci_dev *pdev)
{
	if (!mtgpu_device_restore_pci_state(pdev)) {
		dev_err(&pdev->dev, "restore pci state error\n");
		return PCI_ERS_RESULT_DISCONNECT;
	}

	dev_info(&pdev->dev, "slot reset done\n");

	return PCI_ERS_RESULT_RECOVERED;
}

static void mtgpu_resume(struct pci_dev *dev)
{
	/* TODO: need resume video gpu display */
	dev_info(&dev->dev, "resume done\n");
}

const struct pci_error_handlers err_handler = {
	.error_detected = mtgpu_error_detected,
	.mmio_enabled = mtgpu_mmio_enabled,
	.slot_reset = mtgpu_slot_reset,
	.resume = mtgpu_resume,
};

static struct pci_device_id mtgpu_pci_tbl[] = {
	{ PCI_VENDOR_ID_MT, DEVICE_ID_SUDI104, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S10, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S30_2_Core, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S30_4_Core, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S1000M, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S4000, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S50, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S60, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S100, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S1000, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S2000, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VDEVICE(XIX, DEVICE_ID_HAPS_SUDI104) },
#ifdef CONFIG_SUDI104_VPS
	{ PCI_VDEVICE(MT, DEVICE_ID_SUDI104), .driver_data = (unsigned long)&sudi_drvdata},
#endif
	{ PCI_VDEVICE(MT, DEVICE_ID_QUYUAN1), .driver_data = (unsigned long)&quyuan1_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_QUYUAN1_VF, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&quyuan1_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S80, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&quyuan1_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S70, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&quyuan1_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S3000, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&quyuan1_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_QUYUAN2, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_3D << 8, ~0, .driver_data = (unsigned long)&quyuan2_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S10, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S30_2_Core, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S30_4_Core, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S50, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S60, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&sudi_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S80, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&quyuan1_drvdata},
	{ PCI_VENDOR_ID_MT, DEVICE_ID_MTT_S70, PCI_ANY_ID, PCI_ANY_ID,
	  PCI_CLASS_DISPLAY_VGA << 8, ~0, .driver_data = (unsigned long)&quyuan1_drvdata},
	{ },
};

static const struct dev_pm_ops mtgpu_pm_ops = {
	.suspend = mtgpu_pm_suspend,
	.resume  = mtgpu_pm_resume,
	.resume_early = mtgpu_pm_resume_early,
	.freeze  = mtgpu_pm_suspend,
	.restore = mtgpu_pm_resume,
	.restore_early = mtgpu_pm_resume_early,
};

static struct pci_driver mtgpu_pci_driver = {
	.name = DRIVER_NAME,
	.id_table = mtgpu_pci_tbl,
	.probe = mtgpu_probe,
	.remove = mtgpu_remove,
	.err_handler = &err_handler,
	.driver.pm = &mtgpu_pm_ops,
};

MODULE_DEVICE_TABLE(pci, mtgpu_pci_tbl);

static int __init mtgpu_driver_init(void)
{
	int ret;

	pr_info("MTGPU Driver Version: %s %s build\n",
		MT_BUILD_TAG, PVR_BUILD_TYPE);

	if (disable_driver) {
		pr_info("mtgpu: driver was disabled\n");
		return 0;
	}

	ret = mtgpu_proc_musa_dir_create();
	if (unlikely(ret))
		return ret;

	ret = mtgpu_misc_init();
	if (unlikely(ret))
		goto mtgpu_misc_init_err;

	ret = mtlink_driver_init();
	if (unlikely(ret))
		goto mtlink_driver_init_err;

	ret = pci_register_driver(&mtgpu_pci_driver);
	if (unlikely(ret))
		goto pci_register_driver_err;

	ret = mtgpu_drm_init();
	if (unlikely(ret))
		goto mtgpu_drm_init_err;

	ret = mtsnd_init();
	if (unlikely(ret))
		pr_warn("mtgpu: failed to init the audio function\n");

	return 0;

mtgpu_drm_init_err:
	pci_unregister_driver(&mtgpu_pci_driver);
pci_register_driver_err:
	mtlink_driver_exit();
mtlink_driver_init_err:
	mtgpu_misc_deinit();
mtgpu_misc_init_err:
	mtgpu_proc_musa_dir_remove();
	return ret;
}

static void __exit mtgpu_driver_exit(void)
{
	if (disable_driver)
		return;

	mtsnd_deinit();
	mtgpu_drm_fini();
	pci_unregister_driver(&mtgpu_pci_driver);
	mtlink_driver_exit();
	mtgpu_misc_deinit();
	mtgpu_proc_musa_dir_remove();
}

module_init(mtgpu_driver_init);
module_exit(mtgpu_driver_exit);

MODULE_FIRMWARE(FIRMWARE_LOAD_PATH("musa.fw.1.0.0.0"));
MODULE_FIRMWARE(FIRMWARE_LOAD_PATH("musa.sh.1.0.0.0"));
MODULE_FIRMWARE(FIRMWARE_LOAD_PATH("mtvpu-00-1.0.bin"));
MODULE_FIRMWARE(FIRMWARE_LOAD_PATH("mtvpu-01-1.0.bin"));
MODULE_FIRMWARE(FIRMWARE_LOAD_PATH("mtvpu-02-1.0.bin"));
