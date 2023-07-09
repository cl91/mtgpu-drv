/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */


#ifndef __MTGPU_H__
#define __MTGPU_H__

#include "linux-types.h"
#include "mtgpu_defs.h"
#include "img_types.h"
#include "mtgpu_segment.h"

struct device;
struct mtgpu_device;
struct mtgpu_dma_xfer_desc;
struct ipc_msg;
struct mtgpu_board_configs;
enum reset_type;
enum mtgpu_subsys_id;
struct mtgpu_resource;
typedef struct spinlock spinlock_t;
struct efuse_data;
struct mtlink_ops;
struct mtlink_private_data;
enum mtgpu_clk_domain;
struct ion_device;
struct mgmt_mpc_conf;

#if defined(CONFIG_SUDI104_VPS)
struct vps_dma;
#endif

void mtgpu_devres_release(struct device *dev, void *res);

enum mtgpu_type_t {
	MTGPU_TYPE_SUDI104 = 0,
	MTGPU_TYPE_QUYUAN1,
	MTGPU_TYPE_QUYUAN2,
	MTGPU_TYPE_INVALID = -1,
};

struct mtgpu_interrupt_desc {
	u32 interrupt_src;
	u32 group_id_4;
	u32 group_id_8;
	char *interrupt_name;
	bool enabled;
	bool suspended;
	void (*handler_function)(void *data);
	void *handler_data;
};

struct mtgpu_interrupt_table {
	enum mtgpu_interrupt_id interrupt_id;
	struct mtgpu_interrupt_desc desc;
};

struct mtgpu_region {
	resource_size_t base;
	resource_size_t size;
};

struct mtgpu_io_region {
	struct mtgpu_region region;
	void __iomem *registers;
};

#define MTGPU_GPU_CORE_NON_PRIMARY 0xF
/*
 * group_type is gpu multi primary core group type,
 * example:
 * 4+4,   {4, 4, 0, 0, 0, 0, 0, 0,...}
 * 4+2+2, {4, 2, 2, 0, 0, 0, 0, 0,...}
 * 1+1+1+1+1+1+1+1, {1, 1, 1, 1, 1, 1, 1, 1,...}
 */
struct gpu_cfg_req {
	u8 group_type[MTGPU_CORE_COUNT_MAX];
};

/* eATA configuration IPC transmit payload */
struct gpu_eata_cfg_info {
	u32 core_id;	/* physical core id */
	u32 hyp_id;	/* userbit equal to hyp_id will bypass*/
	/**
	 * qy1: 0:keep user bit 2 1:ignore user bit 2
	 * qy2: 0:hyp id bypass 1:hyp id check or transfer
	 */
	u8 user_ctl;
	u8 size_mode;	/* 0:16G mode 1:32G mode */
	u8 eata_enable;	/* 0:disable eata, bypass whatever eata_mode is 1:eata enable */
	u8 eata_mode;	/* 0:bypass 1:check the descriptor and transfer the address */
};

/* CAUTION: this struct shared with smc, so it MUST synchronized with smc */
struct gpu_cfg_info {
	u8  group_type[MTGPU_CORE_COUNT_MAX];
	u8  primary_physical_id[MTGPU_CORE_COUNT_MAX];
	u8  primary_logical_id[MTGPU_CORE_COUNT_MAX];
	u32 actual_core_count;
};

typedef void (*interrupt_handler)(void *);

struct mtgpu_module_param {
	unsigned long mem_mode;
	unsigned long vpu_mem_size;
	unsigned long smc_mem_size;
	unsigned long cursor_size;
};

struct mtgpu_intc_ops {
	int (*init)(struct mtgpu_device *mtgpu);
	int (*claim)(struct mtgpu_device *mtgpu, int irq_vector);
	void (*clear)(struct mtgpu_device *mtgpu, int irq_vector, int intc_id);
	void (*set_state)(struct mtgpu_device *mtgpu, int interrupt_src,
			  int interrupt_target, int enable);
	void (*exit)(struct mtgpu_device *mtgpu);
	void (*suspend)(struct mtgpu_device *mtgpu);
	int (*resume)(struct mtgpu_device *mtgpu);
	int (*get_desc_table)(const struct mtgpu_interrupt_table **desc_table);
	void (*msi_config)(struct mtgpu_device *mtgpu);
};

struct mtgpu_pcie_local_mgmt_ops {
	int (*init)(struct mtgpu_device *mtgpu);
	void (*exit)(struct mtgpu_device *mtgpu);
};

struct mtgpu_intd_ops {
	void (*sgi_set_state)(struct mtgpu_device *mtgpu, int interrupt_src,
				int target_plic, int event, int enable);
	void (*spi_set_state)(struct mtgpu_device *mtgpu, int interrupt_src,
				int target_plic, int enable);
};

struct mtgpu_display_ops {
	void (*register_display_device)(struct mtgpu_device *mtdev);
};

struct mtgpu_smc_ops {
	int (*smc_get_board_cfg)(struct mtgpu_device *mtdev);
	int (*smc_set_gpu_cfg)(struct mtgpu_device *mtdev, struct gpu_cfg_req *gpu_req);
	int (*smc_get_gpu_cfg)(struct mtgpu_device *mtdev, struct gpu_cfg_info *gpu_info);
	int (*smc_get_vpu_core_info)(struct mtgpu_device *mtdev, u32 *vpu_core_info);
	int (*smc_reset_subsystem)(struct mtgpu_device *mtdev, enum mtgpu_subsys_id ss_id,
				   u64 bit_mask, enum reset_type cmd);
	int (*smc_set_gpu_eata_cfg)(struct mtgpu_device *mtdev,
				    struct gpu_eata_cfg_info *eata_cfg_info);
	s32 (*smc_get_clk_id)(enum mtgpu_clk_domain domain, u16 sub_id);
};

struct mtgpu_llc_ops {
	void (*persisting_get)(struct mtgpu_device *mtdev, u32 *llc_size,
			       u32 *max_llc_persisting_size);
	int (*persisting_set)(struct mtgpu_device *mtdev, u32 replace_mode, u64 max_set_aside_size);
	void (*persisting_reset)(struct mtgpu_device *mtdev);
};

struct pci_dev_config {
	u8 pci_config_data[256];
};

struct mtgpu_device {
	struct pci_dev *pdev;
	struct pci_saved_state *pci_state;

	int mem_mode;

	struct mtgpu_region bar[PCI_STD_NUM_BARS];
	struct mtgpu_region pcie_mem;
	struct mtgpu_region vpu_mem;
	struct mtgpu_region smc_mem;
	struct mtgpu_region pcie_dma;
	struct mtgpu_region gpu_mem;
	struct mtgpu_region cursor_mem;
	struct mtgpu_io_region llc_reg;
	struct mtgpu_io_region intc_reg;
	struct mtgpu_io_region pcie_config_reg;
	struct mtgpu_io_region distributor_reg;
	struct mtgpu_io_region sram_shared_region;
	int disp_cnt;
	struct platform_device *disp_dev[MTGPU_DISP_DEV_NUM];
	struct platform_device *drm_dev[MTGPU_CORE_COUNT_MAX];
	struct platform_device *video_dev;
	struct platform_device *audio_dev;

	u64 real_vram_size;

	struct dp_phy_cfg_hdr *dp_phy_cfg_hdr;

	/*cursor widht/height in pixel*/
	u32 cursor_size;
	int gpu_cnt;
	struct platform_device *gpu_dev[MTGPU_CORE_COUNT_MAX];

	struct mtgpu_chip_info *chip_info;
	struct mtgpu_intc_ops *intc_ops;
	struct mtgpu_pcie_local_mgmt_ops *pcie_local_mgmt_ops;
	struct mtgpu_intd_ops *intd_ops;
	struct mtgpu_dma_ops *dma_ops;
	struct mtgpu_sriov_ops *sriov_ops;
	struct mtgpu_display_ops *disp_ops;
	const struct mtgpu_smc_ops *smc_ops;
	struct mtgpu_llc_ops *llc_ops;
	struct mtlink_ops *link_ops;

	int mtrr;
	spinlock_t *interrupt_handler_lock;
	spinlock_t *interrupt_enable_lock;
	struct mtgpu_interrupt_desc interrupt_desc[MTGPU_INTERRUPT_COUNT];
	u32 irq_type;
	int start_irq;
	int irq_count;

	void *dma_private;
	void *ipc_private;
	void *pstate_private;
	void *fec_private;
	struct mtlink_private_data *link_private_data;
	struct mtgpu_board_configs *board_configs;
	struct mtgpu_local_mgmt_info *local_mgmt;
	struct mtgpu_misc_info *miscinfo[MTGPU_CORE_COUNT_MAX];

#if defined(CONFIG_SUDI104_VPS)
	struct vps_dma *vps_dma;
#endif
#if defined(SUPPORT_ION)
	struct ion_device *ion_dev[MTGPU_CORE_COUNT_MAX];
#endif

	/* virtualization related members */

	/* vpu_guest_mem.base store the card address of VPU heap of VM 1 */
	struct mtgpu_region vpu_guest_mem;

	/* mem_card_base store the card address of MMU table of VM 1*/
	resource_size_t mem_card_base;

	/* gpu_mem_card_base store the card address of GPU memory base */
	resource_size_t gpu_mem_card_base;

	/* mmu_heap store the region of BAR4 in VM*/
	struct mtgpu_region mmu_heap;

	/* mmu_heap store the region of BAR1 in VM*/
	struct mtgpu_io_region vgpu_custom_reg;

	void *mdev_device_state;

	/* struct mtgpu_sriov for sriov virtualization */
	void *sriov;
	/* How many vgpus does the current device support */
	u32 max_vgpu_supported;
	/*
	 * The starting OSID of the guest mapped by the MMU in the FW layout, not include osid0.
	 *  eg:
	 *     QY1 SR-IOV:    osid_start == 4.
	 *     QY1 mdev soft: osid_start == 1.
	 *     QY2 SR-IOV:    osid_start == 2.
	 *     QY2 mdev soft: osid_start == 1.
	 */
	u32 osid_start;
	/*
	 * How many OSIDs are mapped to the MMU in the FW layout, include osid0.
	 * eg:
	 *     max_supported_vm:14:
	 *     QY1 SR-IOV MC8*1:  osid_count = 5; (osid0, 4, 5, 6, 7)
	 *     QY1 SR-IOV MC1*8:  osid_count = 3; (osid0, 4, 5)
	 */
	u32 osid_count;

	void *pm_vddr;
	resource_size_t pm_vddr_size;

	/* There may be multiple discontinuous regions
	   that need to be backed up on s3/s4
	*/
	MEM_REGION_INFO *alloced_mem_region[MTGPU_CORE_COUNT_MAX];
	int alloced_mem_region_cnt[MTGPU_CORE_COUNT_MAX];
	struct notifier_block *mtgpu_nb;
	bool pm_restore_prepare;
	enum mtgpu_type_t gpu_type;
	struct gpu_cfg_info gpu_info;
	bool mpc_is_enabled;
	struct mgmt_mpc_conf *mpc_conf;
	u32 dev_id;

	struct mtgpu_segment_info *segment_info;
	int segment_info_cnt;
	u8 video_group_idx[MTGPU_CORE_COUNT_MAX];
	u8 video_group_cnt;

	/* related to the procfs file directory */
	struct proc_dir_entry *proc_gpu_dir;
	struct proc_dir_entry *proc_vram_info;
	struct proc_dir_entry *proc_ctrl_devname;
	struct proc_dir_entry *proc_memory;
	struct proc_dir_entry *proc_status;
	struct proc_dir_entry *proc_mpc_enable;
	struct proc_dir_entry *proc_gpu_instance_dir[MTGPU_CORE_COUNT_MAX];
	struct proc_dir_entry *proc_mpc_dir;

	/*get platform device information including platform data and recoueses*/
	int (*get_platform_device_info)(struct mtgpu_device *mtdev, u32 hw_module, u32 hw_id,
					struct mtgpu_resource **mtgpu_res, u32 *num_res,
					void **data, size_t *size_data);

	struct pci_dev_config *pci_dev_config_data;
	struct efuse_data *efuse;
	bool pstate_supported;
};

extern struct device_ops sudi_ops;
extern struct mtgpu_chip_info sudi_chip_info;

extern struct device_ops quyuan1_ops;
extern struct mtgpu_chip_info quyuan1_chip_info;

#endif /* __MTGPU_H__ */
