/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

/*
 * This is a device driver for the mtgpu framework. It creates platform
 * devices inside the MT GPU, and exports functions to manage the
 * shared interrupt handling
 */

#include <linux/moduleparam.h>

#include "mtgpu_defs.h"

/* TODO: Currently assume the MTGPU default video ram size is 1G.
 * It should get the video ram size from SMC in the future for each
 * mtgpu graphics card
 */
unsigned long mtgpu_vram_size_total = MTGPU_VRAM_DEFAULT_SIZE;
module_param(mtgpu_vram_size_total, ulong, 0444);
MODULE_PARM_DESC(mtgpu_vram_size_total,
		 "mtgpu video memory total size, default to 1GB ");

unsigned long mtgpu_smc_mem_size = MTGPU_SMC_MEM_SIZE;
module_param(mtgpu_smc_mem_size, ulong, 0444);
MODULE_PARM_DESC(mtgpu_smc_mem_size,
		 "mtgpu smc memory total size, default to 2M ");

unsigned long mtgpu_reserved_mem_size = MTGPU_RESERVED_MEM_SIZE;
module_param(mtgpu_reserved_mem_size, ulong, 0444);
MODULE_PARM_DESC(mtgpu_reserved_mem_size,
		 "mtgpu reserved memory size for smc/dma/fw/mmu etc, to meet system requirements, default to 256MB ");

/*
 * mtgpu_resize_enable is used for the option to control the resize bar
 * functionality is enabled or disabled.
 */
int mtgpu_resize_enable = 1;
module_param(mtgpu_resize_enable, int, 0444);
MODULE_PARM_DESC(mtgpu_resize_enable,
		 "mtgpu resize function enable(1)/disable(0) ");

#if defined(CONFIG_VPS)
char *display = "dummy";
#elif (RGX_NUM_OS_SUPPORTED > 1)
char *display = "none";
#else
char *display = "mt";
#endif
module_param(display, charp, 0444);
MODULE_PARM_DESC(display, " <dummy>, <mt> The default value is mt");

unsigned long mtgpu_cursor_size = MTGPU_MAX_CURSOR_SIZE;
module_param(mtgpu_cursor_size, ulong, 0444);
MODULE_PARM_DESC(mtgpu_cursor_size, "max cursor size in pixel");

/* Enable_sriov is used for the sriov capability. */
int enable_sriov = MTGPU_DISABLE_SRIOV;
module_param(enable_sriov, int, 0444);
MODULE_PARM_DESC(enable_sriov, "enable sriov, default to 0 is disable ");

bool force_cpu_copy;
module_param(force_cpu_copy, bool, 0644);
MODULE_PARM_DESC(force_cpu_copy,
		 "dma transfer use cpu copy force, enable(1)/disable(0) ");

int mtgpu_dma_debug;
module_param(mtgpu_dma_debug, int, 0444);
MODULE_PARM_DESC(mtgpu_dma_debug,
		 "mtgpu udma and hdma debug information enable(1)/disable(0) ");

#if (RGX_NUM_OS_SUPPORTED > 1)
int mtgpu_driver_mode = MTGPU_DRIVER_MODE_HOST;
module_param(mtgpu_driver_mode, int, 0444);
MODULE_PARM_DESC(mtgpu_driver_mode,
		 "mtgpu driver mode (native = -1, host = 0, guest = 1 )");
#else
int mtgpu_driver_mode = MTGPU_DRIVER_MODE_NATIVE;
#endif

unsigned long mtvpu_reserved_mem_size;
module_param(mtvpu_reserved_mem_size, ulong, 0444);
MODULE_PARM_DESC(mtvpu_reserved_mem_size, "reserved gpu memory for VPU");

int mtgpu_ipc_debug;
module_param(mtgpu_ipc_debug, int, 0664);
MODULE_PARM_DESC(mtgpu_ipc_debug,
		 "mtgpu ipc debug information enable(1)/disable(0)");

unsigned long mtgpu_ipc_timeout_ms = 1000 * 60;
module_param(mtgpu_ipc_timeout_ms, ulong, 0664);
MODULE_PARM_DESC(mtgpu_ipc_timeout_ms,
		 "mtgpu ipc timeout(unit:ms, default:1 minute/60000 ms)");

#ifdef DEBUG
int mtgpu_ipc_tty_support;
module_param(mtgpu_ipc_tty_support, int, 0444);
MODULE_PARM_DESC(mtgpu_ipc_tty_support,
		 "mtgpu ipc tty console enable(1)/disable(0)");
#endif

bool mtgpu_fec_enable;
module_param(mtgpu_fec_enable, bool, 0444);
MODULE_PARM_DESC(mtgpu_fec_enable, "mtgpu fec enable(1)/disable(0)");

int enable_mtlink;
module_param(enable_mtlink, int, 0444);
MODULE_PARM_DESC(enable_mtlink, "1:enable mtlink, 0:disable mtlink");

unsigned long mtlink_timer_expires = 5000;
module_param(mtlink_timer_expires, ulong, 0444);
MODULE_PARM_DESC(mtlink_timer_expires,
		 "timer expires(ms) from pcie probe to mtlink init, default 5000ms");

/**
 * pstate mode
 * To control mtgpu pstate mode (0 = disable, 1 = enable).
 * The default is 1 (enabled).
 */
unsigned char mtgpu_pstate_mode = PSTATE_ENABLED;
module_param(mtgpu_pstate_mode, byte, 0444);
MODULE_PARM_DESC(mtgpu_pstate_mode,
		 "mtgpu pstate mode: 0 = disable, 1 = enable");

#if defined(CONFIG_VPS)
int mtgpu_vpu_mode = MTGPU_VPU_MODE_DISABLE;
#else
int mtgpu_vpu_mode = MTGPU_VPU_MODE_DEFAULT;
#endif
module_param(mtgpu_vpu_mode, int, 0444);
MODULE_PARM_DESC(mtgpu_vpu_mode, "0: default, 1: test mode, 2: disable all vpu modules.");

int disable_vpu;
module_param(disable_vpu, int, 0444);
MODULE_PARM_DESC(disable_vpu, "0: default, 1: disable vpu module.");

unsigned int mtgpu_page_size = 0;
module_param(mtgpu_page_size, uint, 0444);
MODULE_PARM_DESC(mtgpu_page_size, "gpu page size, default to 0.");

int enable_large_mem_mode;
module_param(enable_large_mem_mode, int, 0444);
MODULE_PARM_DESC(enable_large_mem_mode,
		 "1:enable compute memory mode, 0:disable compute memory mode");

bool disable_driver;
module_param(disable_driver, bool, 0444);
MODULE_PARM_DESC(disable_driver, "disable mtgpu driver, default 0.");
