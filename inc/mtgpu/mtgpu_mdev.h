/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#ifndef __MTGPU_MDEV_H__
#define __MTGPU_MDEV_H__

#include "mtgpu.h"

#define VFIO_PCI_OFFSET_SHIFT   40
#define VFIO_PCI_OFFSET_TO_INDEX(off)   (off >> VFIO_PCI_OFFSET_SHIFT)
#define VFIO_PCI_INDEX_TO_OFFSET(index) ((u64)(index) << VFIO_PCI_OFFSET_SHIFT)
#define VFIO_PCI_OFFSET_MASK    (((u64)(1) << VFIO_PCI_OFFSET_SHIFT) - 1)

#define MTGPU_SYSMEM_DEV_BASE                     0x8000000000UL

/* Windows guest PC entry invalidate value */
#define MTGPU_VGPU_WINDOWS_PC_ENTRY_INVALIDATE    0x80000000

/* Windows guest PD&PT entry invalidate value */
#define MTGPU_VGPU_WINDOWS_PD_PT_ENTRY_INVALIDATE 0x8000000000UL

#define MTGPU_FW_MMU_RESERVED_SIZE      0x100000
#define MTGPU_FW_HEAP_SIZE              (1 << RGX_FW_HEAP_SHIFT)
#define MTGPU_VGPU_CONFIG_SPACE_SIZE    0x100 // 256 bytes

/* The number of vgpu_state arrays in memory */
#define MTGPU_VGPU_STATE_COUNT         (48)

/* sudi supported 10 per chip */
#define MTGPU_SUDI_MAX_VGPU_COUNT      (10)
/**
 * QY1 supported 14(sriov_enable), 20(sriov_disable);
 * TODO: Temporarily set the value to 14, the value will be
 * changed to 20 when the vpu PR and multi MC PR is ready.
 */
#define MTGPU_QY1_SRIOV_MAX_VGPU_COUNT (14)
#define MTGPU_QY1_SOFT_MAX_VGPU_COUNT  (20)
/*
 * TODO: Temporarily set the value to 8, The final value is confirmed by PRD.
 */
#define MTGPU_QY2_SRIOV_MAX_VGPU_COUNT (8)
#define MTGPU_QY2_SOFT_MAX_VGPU_COUNT  (8)

// SUDI GPU regs in BAR0 2MB~18MB range.
#define MTGPU_SUDI_GPU_REGS_RANGE      (0x1000000)

#if defined(SUPPORT_SW_OSID_EXTENSION)
/* TODO:
 * Due to video hardware capabilities and memory size limitations, can only support 7 guest os now
 */
#define MTGPU_PER_MPC_USED_MAX_OSID     PVRSRV_VZ_NUM_OSID
#define MTGPU_VGPU_MAX_HW_OSID          8
#define MTGPU_SW_OSID_EXT_KICK_DOMAIN_HOST 4
#define MTGPU_SW_OSID_EXT_DOMAIN_BEGIN  1
#define MTGPU_SW_OSID_EXT_DOMAIN_END    3
#define MTGPU_SW_OSID_EXT_DOMAIN_OFFSET 4
#else
#define MTGPU_PER_MPC_USED_MAX_OSID      (8)
#endif

#define MTGPU_VGPU_INVALID_OSID         0
#define MTGPU_VGPU_DEDICATED_REG_SIZE   (64 * 1024)
#define MTGPU_VGPU_CUSTOM_REG_SIZE      (64 * 1024)

#define MTGPU_VGPU_MAX_MEM_CONTEXT      32

#define MTGPU_VGPU_DISPLAY_REGION       16

#define STORE_LE16(addr, val)           (*(u16 *)addr = val)
#define STORE_LE32(addr, val)           (*(u32 *)addr = val)

/* MMU Table */
#define MTGPU_VGPU_MMU_PC_MASK          0xFFFFFFF0
#define MTGPU_VGPU_MMU_PD_MASK          0xFFFFFFF000
#define MTGPU_VGPU_MMU_PT_MASK          0xFFFFFFF000

/* pci ids */
#define MTGPU_VGPU_PCI_VENDOR_ID        PCI_VENDOR_ID_MT
#define MTGPU_VGPU_PCI_SUBVENDOR_ID     PCI_VENDOR_ID_MT
#define MTGPU_VGPU_PCI_SUBDEVICE_ID     PCI_SUBDEVICE_ID_QEMU

/* pci cfg space offsets for fb config (dword) */
#define MTGPU_VGPU_VENDORCAP_OFFSET	0x40
#define MTGPU_VGPU_VENDORCAP_SIZE	0x10
#define MTGPU_VGPU_FORMAT_OFFSET	(MTGPU_VGPU_VENDORCAP_OFFSET + 0x04)
#define MTGPU_VGPU_WIDTH_OFFSET 	(MTGPU_VGPU_VENDORCAP_OFFSET + 0x08)
#define MTGPU_VGPU_HEIGHT_OFFSET	(MTGPU_VGPU_VENDORCAP_OFFSET + 0x0c)


#define MTGPU_VGPU_INT_ID_BODA955	23
#define MTGPU_VGPU_INT_ID_WAVE517	24
#define MTGPU_VGPU_INT_ID_WAVE627	25
#define MTGPU_VGPU_INT_ID_GPU		26

#define MTGPU_VGPU_NAME      		"mtgpu"
#define MTGPU_VGPU_CLASS_NAME		"mtgpu"

/* TODO
 * It has not yet been decided that the 1GB vgpu type corresponds to the actual memory size,
 * so the temporary definition is as follows, it will be modified later
 */
#define MTGPU_VGPU_TYPE_1GB      "2001"
/* 1G~2G vgpu type temporary definition, which will be removed later*/
#define MTGPU_VGPU_TYPE_1GB_2    "2100"
#define MTGPU_VGPU_TYPE_1GB_3    "2101"
#define MTGPU_VGPU_TYPE_1GB_5    "2102"
#define MTGPU_VGPU_TYPE_1GB_7    "2103"
#define MTGPU_VGPU_TYPE_2GB      "2002"
#define MTGPU_VGPU_TYPE_3GB      "2003"
#define MTGPU_VGPU_TYPE_4GB      "2004"
#define MTGPU_VGPU_TYPE_5GB      "2005"
#define MTGPU_VGPU_TYPE_6GB      "2006"
#define MTGPU_VGPU_TYPE_7GB      "2007"
#define MTGPU_VGPU_TYPE_8GB      "2008"
#define MTGPU_VGPU_TYPE_16GB     "2016"

#define MTGPU_VGPU_DDR_GRANULARITY_SIZE 0x8000000UL
#define MTGPU_VGPU_DDR_SIZE_COEFFICIENT 7/8

#define MAX_VM_MEM_SEGMENT 64

/* Temporary code for VPU, 0 disable, 1 enable  */
#define MTGPU_VPU_GROUP_TEST 0

enum mtgpu_vgpu_vpu_group {
	MTGPU_VGPU_VPU_GROUP1 = 0,   /* [ 0, 3G)   */
	MTGPU_VGPU_VPU_GROUP2,       /* [4G, 6.5G) */
	MTGPU_VGPU_VPU_GROUP3,       /* [8G, 9G)   */
	MTGPU_VGPU_VPU_GROUP_MAX
};

/* indicate vpu_group_id is invalid */
#define MTGPU_VGPU_VPU_GROUP_INVALID      (MTGPU_VGPU_VPU_GROUP_MAX + 1)

/**
 * For gen3, vpu heap is an independent heap.
 * Don't need to alloc memory from vpu group but from vpu heap instead.
 */
#define MTGPU_VGPU_VPU_NON_GROUP	  (0xff)

/* vpu vRAM physical addr can't exceed 4G */
#define MTGPU_VPU_MEM_GROUP_MAX_RANGE     (0x100000000ULL)  /* 4096MB */
/* group1 (default) [ 0, 3G) */
#define MTGPU_VGPU_VPU_GROUP1_BASE        (0x00000000ULL)
#define MTGPU_VGPU_VPU_GROUP1_SIZE        (0xC0000000ULL)   /* 3072MB */
/* group2 [4G, 6.5G) */
#define MTGPU_VGPU_VPU_GROUP2_BASE        (0x100000000ULL)  /* 4096MB */
#define MTGPU_VGPU_VPU_GROUP2_SIZE        (0xA0000000ULL)   /* 2560MB */
/* group3 [8G, 9G) */
#define MTGPU_VGPU_VPU_GROUP3_BASE        (0x200000000ULL)  /* 8192MB */
#define MTGPU_VGPU_VPU_GROUP3_SIZE        (0x40000000ULL)   /* 1024MB */

/* The number of VM supported by each vpu group */
#if MTGPU_VPU_GROUP_TEST
#define MTGPU_VGPU_VPU_GROUP1_MAX_VM      (2)
#define MTGPU_VGPU_VPU_GROUP2_MAX_VM      (2)
#define MTGPU_VGPU_VPU_GROUP3_MAX_VM      (2)
#else
#define MTGPU_VGPU_VPU_GROUP1_MAX_VM      (10)              /* max is 10 */
#define MTGPU_VGPU_VPU_GROUP2_MAX_VM      (10)
#define MTGPU_VGPU_VPU_GROUP3_MAX_VM      (0)
#endif

/* virtual display num */
#define MTGPU_VGPU_DISPLAY_NUM_MAX      2

#if defined(VDI_PLATFORM_RUIJIE) || \
    defined(VDI_PLATFORM_CTYUN) || \
    defined(VDI_PLATFORM_CSTACK)
#define MTGPU_VGPU_DISPLAY_NUM_DEFAULT  1
#else
#define MTGPU_VGPU_DISPLAY_NUM_DEFAULT  0
#endif

/* For gfn_to_pfn workaround.
 * It seems the gfn_to_pfn will fail if it is called from ioctl or other
 * context different from vcpu context, so use the workaround before fixing it
 */
#define MTGPU_VGPU_GFN_TO_PFN_WORKAROUND

enum mtgpu_mdev_interrupt_status_id {
	MTGPU_MDEV_INT_STATUS_GPU,
	MTGPU_MDEV_INT_STATUS_BODA955,
	MTGPU_MDEV_INT_STATUS_WAVE517,
	MTGPU_MDEV_INT_STATUS_WAVE627,
	MTGPU_MDEV_INT_STATUS_LAST
};

/* Value write into MTGPU_VGPU_SHARED_MEM_UPDATE virtual register */
#define MTGPU_VPU_SHARE_MEM_REL 0
#define MTGPU_VPU_SHARE_MEM_GET 1
#define MTGPU_SYSTEM_SHARE_MEM_GET 2
#define MTGPU_SYSTEM_SHARE_MEM_REL 3

struct mem_segment {
	u32 first_slot_number;
	u64 base;
	u64 size;
};

/* The definition of the structure must be consistent with the definition of wddm */
struct vm_mem_segment {
	volatile u64 base;
	volatile u64 size;
};

struct vgpu_info_ext {
	u64 padding;
};

#define MTGPU_VGPU_INFO_MAGIC			(0xAA557491U)
#define MTGPU_VGPU_INFO_VERSION1		(1U)
#define MTGPU_VGPU_INFO_FLAG_ENABLE_VPU		BIT_ULL(0)
#define MTGPU_VGPU_INFO_FLAG_ENABLE_IOMMU_DMA	BIT_ULL(1)
#define MTGPU_VGPU_INFO_FLAG_ENABLE_IOMMU_IRQ	BIT_ULL(2)
#define MTGPU_VGPU_INFO_FLAG_VPU_BAR4		BIT_ULL(3)

struct vgpu_info {
	volatile u32 magic;
	volatile u32 version;
	volatile u32 osid;
	volatile u64 flag;
	struct vm_mem_segment vm_mem_segment[MAX_VM_MEM_SEGMENT];
	volatile u32 vm_segment_ount;
	volatile u64 vm_mem_size;
	volatile u64 vm_bar2_actual_mem_size;
	volatile u64 fw_heap_base;
	volatile u32 fw_heap_size;
	volatile u64 mmu_heap_base;
	volatile u32 mmu_heap_size;
	volatile u64 vpu_mem_base;
	volatile u64 vpu_mem_size;
	volatile u64 ext_size;

	/*
	 * WARNING: Do not change anything above here!
	 * New added variables should be stored in 'struct vgpu_info_ext'
	 */
	struct vgpu_info_ext ext_info;
};

/**
 * MPC info indicates the logical core id, physical core id and multi primary core id start with 0.
 * Each primary core can create multi osids, and osid may be same on different primary core. To
 * locate the resources, the vgpu instance has to know which primary core it is on. So it is
 * necessary to touch the primary core info as convenient as possible.
 */
struct mtgpu_mpc_info {
	/* id of the multicore that this core on. Start with 0. */
	u32 mpc_id;
	/* the logical id of the primary core of the multicore that this core on. */
	u32 logical_core_id;
	/* the physical id of the primary core of the multicore that this core on. */
	u32 physical_core_id;
};

enum shared_mem_vz_id {
	SHARED_MEM_VZ_VPU,
	SHARED_MEM_VZ_LAST
};

struct shared_mem_info {
	volatile uint64_t ui64CardAddr;
	volatile uint64_t ui64CpuAddr;
	volatile uint64_t ui64Offset;
	volatile uint32_t ui32Size;
};

struct shared_mem_vz_list {
	struct shared_mem_info shared_mem_vz_info[SHARED_MEM_VZ_LAST];
};

struct mtgpu_mdev_device_info {
	u32 video_mem_group_id;
	u64 video_mem_group_base;
	u64 video_mem_cpu_phy_addr;
	u64 video_mem_card_phy_addr;
	u64 video_mem_size;
};

struct mtgpu_mdev_ops {
	int (*create)(int osid, struct mtgpu_mdev_device_info *info, struct device *dev);
	int (*remove)(int osid, struct device *dev);

	/*
	 * Called when accessing the custom registers.
	 * Return value:
	 *     0: the operation is not handled, MDEV will continue to handle this access.
	 *     1: the operation is handled, MDEV will not handle this access anymore.
	 */
	int (*reg_write)(int osid, u32 offset, u64 val);
	int (*reg_read)(int osid, u32 offset, u64 *val);
};

#if (RGX_NUM_OS_SUPPORTED > 1)
int mtgpu_mdev_init(struct mtgpu_device *mtgpu);
void mtgpu_mdev_exit(struct mtgpu_device *mtgpu);
int mtgpu_vz_init(struct mtgpu_device *mtgpu);
void mtgpu_vz_exit(struct mtgpu_device *mtgpu);

void mtgpu_vgpu_set_vpu_status(struct device *dev, u32 status);
u64 mtgpu_vgpu_get_host_available_mem_size(struct device *dev);
u64 mtgpu_vgpu_get_host_reserved_mem_size(struct device *dev);
u64 mtgpu_vgpu_get_vpu_guest_mem_size(void);
bool mtgpu_vgpu_is_last_mpc(struct device *dev, u32 mpc_id);
bool mtgpu_vgpu_enabled_vpu_multi_mem_group(struct device *dev);
bool mtgpu_vgpu_use_vpu_multi_mem_group(struct device *dev);
u64 mtgpu_vgpu_get_vpu_multi_group_base(u32 group_id, struct device *dev);
u64 mtgpu_vgpu_get_vpu_multi_group_size(u32 group_id, struct device *dev);
int mtgpu_vgpu_osid_count_per_core(struct mtgpu_device *mtdev, int vf_per_gpu_core, int mpc_cnt);
u64 mtgpu_vgpu_get_card_page_table_size(void);
u64 mtgpu_vgpu_get_mpc_mem_card_base(struct mtgpu_device *mtdev, u32 mpc_id);
u64 mtgpu_vgpu_get_mpc_mem_size(struct mtgpu_device *mtdev, u32 mpc_id);
u64 mtgpu_vgpu_get_mpc_non_gpumem_size(u32 osid_count);
int mtgpu_mdev_gpa_to_hva(struct device *dev, u32 instance_id, u64 gpa, u64 *hva);
int mtgpu_mdev_gpa_to_hpa(struct device *dev, u32 instance_id, u64 gpa, u64 *hpa);
int mtgpu_mdev_dpa_to_gpa(struct device *dev, u32 instance_id, u64 dpa, u64 *gpa);
int mtgpu_mdev_trigger_interrupt(struct device *dev, u32 instance_id, int int_id);
void mtgpu_mdev_register_callback(struct mtgpu_mdev_ops ops, struct device *dev);
int mtgpu_vgpu_get_scheduling_policy(void);
int mtgpu_vgpu_get_time_sliced_value(void);
unsigned long mtgpu_vgpu_get_host_mem_size(void);
void mtgpu_vgpu_set_host_mem_size(unsigned long size);
bool mtgpu_vgpu_should_force_mmio_in_4g(void);
int mtgpu_vgpu_get_vpu_count(void);
void mtgpu_vgpu_set_vpu_count(int count);
#else

#ifndef MT_UNREFERENCED_PARAMETER
#define MT_UNREFERENCED_PARAMETER(param) ((void)(param))
#endif

static inline int mtgpu_mdev_init(struct mtgpu_device *mtgpu)
{
	MT_UNREFERENCED_PARAMETER(mtgpu);
	return -1;
}

static inline void mtgpu_mdev_exit(struct mtgpu_device *mtgpu)
{
	MT_UNREFERENCED_PARAMETER(mtgpu);
}

static inline int mtgpu_vz_init(struct mtgpu_device *mtgpu)
{
	MT_UNREFERENCED_PARAMETER(mtgpu);
	return 0;
}

static inline void mtgpu_vz_exit(struct mtgpu_device *mtgpu)
{
	MT_UNREFERENCED_PARAMETER(mtgpu);
}

static inline void mtgpu_vgpu_set_vpu_status(struct device *dev, u32 status)
{
	MT_UNREFERENCED_PARAMETER(dev);
	MT_UNREFERENCED_PARAMETER(status);
}

static inline u64 mtgpu_vgpu_get_host_available_mem_size(struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(dev);
	return 0;
}

static inline u64 mtgpu_vgpu_get_host_reserved_mem_size(struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(dev);
	return 0;
}

static inline u64 mtgpu_vgpu_get_vpu_guest_mem_size(void)
{
	return 0;
}

static inline bool mtgpu_vgpu_enabled_vpu_multi_mem_group(struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(dev);
	return false;
}

static inline bool mtgpu_vgpu_use_vpu_multi_mem_group(struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(dev);
	return false;
}

static inline u64 mtgpu_vgpu_get_vpu_multi_group_base(u32 group_id, struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(group_id);
	MT_UNREFERENCED_PARAMETER(dev);
	return 0;
}

static inline u64 mtgpu_vgpu_get_vpu_multi_group_size(u32 group_id, struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(group_id);
	MT_UNREFERENCED_PARAMETER(dev);
	return 0;
}

static inline int mtgpu_vgpu_osid_count_per_core(struct mtgpu_device *mtdev,
						 int vf_per_gpu_core,
						 int mpc_cnt)
{
	MT_UNREFERENCED_PARAMETER(mtdev);
	MT_UNREFERENCED_PARAMETER(vf_per_gpu_core);
	MT_UNREFERENCED_PARAMETER(mpc_cnt);
	return 0;
}

static inline u64 mtgpu_vgpu_get_card_page_table_size(void)
{
	return 0;
}

static inline u64 mtgpu_vgpu_get_mpc_mem_card_base(struct mtgpu_device *mtdev, u32 mpc_id)
{
	MT_UNREFERENCED_PARAMETER(mtdev);
	MT_UNREFERENCED_PARAMETER(mpc_id);
	return 0;
}

static inline u64 mtgpu_vgpu_get_mpc_mem_size(struct mtgpu_device *mtdev, u32 mpc_id)
{
	MT_UNREFERENCED_PARAMETER(mtdev);
	MT_UNREFERENCED_PARAMETER(mpc_id);
	return 0;
}

static inline u64 mtgpu_vgpu_get_mpc_non_gpumem_size(u32 osid_count)
{
	MT_UNREFERENCED_PARAMETER(osid_count);
	return 0;
}

static inline int mtgpu_mdev_gpa_to_hva(struct device *dev, u32 instance_id, u64 gpa, u64 *hva)
{
	MT_UNREFERENCED_PARAMETER(dev);
	MT_UNREFERENCED_PARAMETER(instance_id);
	MT_UNREFERENCED_PARAMETER(gpa);
	MT_UNREFERENCED_PARAMETER(hva);
	return -1;
}

static inline int mtgpu_mdev_gpa_to_hpa(struct device *dev, u32 instance_id, u64 gpa, u64 *hpa)
{
	MT_UNREFERENCED_PARAMETER(dev);
	MT_UNREFERENCED_PARAMETER(instance_id);
	MT_UNREFERENCED_PARAMETER(gpa);
	MT_UNREFERENCED_PARAMETER(hpa);
	return -1;
}

static inline int mtgpu_mdev_dpa_to_gpa(struct device *dev, u32 instance_id, u64 dpa, u64 *gpa)
{
	MT_UNREFERENCED_PARAMETER(dev);
	MT_UNREFERENCED_PARAMETER(instance_id);
	MT_UNREFERENCED_PARAMETER(dpa);
	MT_UNREFERENCED_PARAMETER(gpa);
	return -1;
}

static inline int mtgpu_mdev_trigger_interrupt(struct device *dev, u32 instance_id, int int_id)
{
	MT_UNREFERENCED_PARAMETER(dev);
	MT_UNREFERENCED_PARAMETER(instance_id);
	MT_UNREFERENCED_PARAMETER(int_id);
	return -1;
}

static inline void mtgpu_mdev_register_callback(struct mtgpu_mdev_ops ops, struct device *dev)
{
	MT_UNREFERENCED_PARAMETER(ops);
	MT_UNREFERENCED_PARAMETER(dev);
}

static inline int mtgpu_vgpu_get_scheduling_policy(void)
{
	return -1;
}

static inline int mtgpu_vgpu_get_time_sliced_value(void)
{
	return -1;
}

static inline unsigned long mtgpu_vgpu_get_host_mem_size(void)
{
	return 0;
}

static inline void mtgpu_vgpu_set_host_mem_size(unsigned long size)
{
	MT_UNREFERENCED_PARAMETER(size);
}

static inline bool mtgpu_vgpu_should_force_mmio_in_4g(void)
{
	return false;
}

static inline int mtgpu_vgpu_get_vpu_count(void)
{
        return 0;
}

static inline void mtgpu_vgpu_set_vpu_count(int count)
{
        MT_UNREFERENCED_PARAMETER(count);
}

#endif

#if defined(SUPPORT_SW_OSID_EXTENSION)
void mtgpu_mdev_set_master_kick_reg(void *master_kick_reg, void *priv_data, u32 mpc_id);
void mtgpu_mdev_vgpu_kick(int osid, u32 kick_value, void *priv_data);
void mtgpu_mdev_vgpu_int_cb(u32 int_id, bool is_osid0, void *priv_data);

#if defined(SUPPORT_LOAD_WINDOWS_FIRMWARE)
void mtgpu_mdev_set_fw_init_mem(void *fw_init, void *priv_data, u32 mpc_id);
void mtgpu_mdev_set_ext_osid_info_mem(void *ext_osid_info, void *priv_data, u32 mpc_id);
#endif /* #if defined(SUPPORT_LOAD_WINDOWS_FIRMWARE) */
#endif /* #if defined(SUPPORT_SW_OSID_EXTENSION) */

#if defined(SUPPORT_LOAD_WINDOWS_FIRMWARE)
void mtgpu_mdev_get_win_fw_info(void *win_fw_info, void *dev, u32 mpc_id);
u64 mtgpu_mdev_actual_mem_size_sum(struct device *dev);
bool mtgpu_get_intr_info(void *dev, u32 instance_id,
			 u32 *osid, u64 *intr_enqueue, u64 *intr_inject,
			 u64 *intr_eoi, u64 *intr_vm_handled);
int mtgpu_mdev_init_fw_heap_ring_buffer(struct device *dev);
#endif

static inline u64 roundup_power2(const u64 x)
{
	u64 y = x;

	y--;
	y |= y >> 1;
	y |= y >> 2;
	y |= y >> 4;
	y |= y >> 8;
	y |= y >> 16;
	y |= y >> 32;
	y++;
	return y;
}

#endif /* __MTGPU_MDEV_H__ */
