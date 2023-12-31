/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */


#ifndef __MTGPU_DEFS_H__
#define __MTGPU_DEFS_H__

#define MTGPU_CORE_COUNT_MAX        (32)
#define MTGPU_CORE_COUNT_7	    (7)

/* TODO: Currently assume the MTGPU default video ram size is 16G.
 * It should get the video ram size from SMC in the future for each
 * mtgpu graphics card
 */
#define MTGPU_VRAM_DEFAULT_SIZE		(0x400000000UL)

#define MTGPU_DSP_MEM_SIZE		(0x1000000UL)
/* In QY, SMC does not allocate ddr mem anymore. Just extend the smc
 * reserved size to MTGPU_SMC_RESERVED_SIZE but FEC consumes it.
 */
#define MTGPU_SMC_RESERVED_SIZE		(0x200000UL)
#define MTGPU_SMC_MEM_SIZE		(MTGPU_DSP_MEM_SIZE + MTGPU_SMC_RESERVED_SIZE)
#define MTGPU_RESERVED_MEM_SIZE		(0x10000000UL)

/* MTGPU_FEC_BL_BASE follows closely after MTGPU_DSP_MEM_SIZE */
#define MTGPU_FEC_BL_BASE		(0x1000000UL)
#define MTGPU_FEC_IMAGE_BASE		(0x1800000UL)
/* fec total reserved size base from MTGPU_FEC_BL_BASE, ends at 0x9000000 */
#define MTGPU_FEC_RESERVED_SIZE		MTGPU_SMC_RESERVED_SIZE

#define PCI_VENDOR_ID_XIX		(0x10EE)
#define DEVICE_ID_HAPS_SUDI104		(0x9011)

#define PCI_VENDOR_ID_MT		(0x1ED5)
#define DEVICE_ID_SUDI104		(0x0100)
#define DEVICE_ID_MTT_S10		(0x0101)
#define DEVICE_ID_MTT_S30_2_Core	(0x0102)
#define DEVICE_ID_MTT_S30_4_Core	(0x0103)
#define DEVICE_ID_MTT_S1000M		(0x0121)
#define DEVICE_ID_MTT_S4000		(0x0124)
#define DEVICE_ID_MTT_S50		(0x0105)
#define DEVICE_ID_MTT_S60		(0x0106)
#define DEVICE_ID_MTT_S100		(0x0111)
#define DEVICE_ID_MTT_S1000		(0x0122)
#define DEVICE_ID_MTT_S2000		(0x0123)

#define DEVICE_ID_QUYUAN1		(0x0200)
#define DEVICE_ID_MTT_S80		(0x0201)
#define DEVICE_ID_MTT_S70		(0x0202)
#define DEVICE_ID_MTT_S3000		(0x0222)

#define DEVICE_ID_QUYUAN2		(0x0300)

#define DEVICE_ID_QUYUAN1_VF		(0x02aa)
#define DEVICE_ID_QUYUAN2_VF		(0x03aa)

#define GET_DEVICE_ID(mtdev)       (os_get_pci_device_id((mtdev)->pdev) & 0xff00)

#define DEVICE_IS_SUDI(mtdev)     (GET_DEVICE_ID(mtdev) == DEVICE_ID_SUDI104)
#define DEVICE_IS_QUYUAN1(mtdev)  (GET_DEVICE_ID(mtdev) == DEVICE_ID_QUYUAN1)
#define DEVICE_IS_QUYUAN2(mtdev)  (GET_DEVICE_ID(mtdev) == DEVICE_ID_QUYUAN2)

/* ALL MT Soc Platform common definition */
#define PCI_STD_NUM_BARS			6

#define MTGPU_DEVICE_NAME		"mtgpu"

#define MTGPU_DEVICE_NAME_VIDEO		"mtgpu_video"
#define MTGPU_DEVICE_NAME_AUDIO		"mtgpu_audio"

#define MTGPU_DEVICE_NAME_DISPC		"mtgpu-dispc"
#define MTGPU_DEVICE_NAME_DP		"mtgpu-dp"
#define MTGPU_DEVICE_NAME_DP_PHY	"mtgpu-dp-phy"
#define MTGPU_DEVICE_NAME_HDMI		"mtgpu-hdmi"

/* Valid values for the MTGPU_MEMORY_CONFIG configuration option */
#define MTGPU_MEMORY_LOCAL			1
#define MTGPU_MEMORY_HOST			2
#define MTGPU_MEMORY_HYBRID			3

#define MTGPU_SYS_BAR               0
#define MTGPU_DDR_BAR               2

#define MTGPU_MAX_CURSOR_SIZE	64

/* sriov is turned off by default */
#define MTGPU_DISABLE_SRIOV             (0)
#define MTGPU_ENABLE_SRIOV              (1)

/*
 * NATIVE: same as bare-metal
 * HOST: used in hypervisor in virtualization scenario.
 * GUEST: used in guest OS  in virtualization scenario.
 */
#define MTGPU_DRIVER_MODE_NATIVE	-1
#define MTGPU_DRIVER_MODE_HOST		0
#define MTGPU_DRIVER_MODE_GUEST		1

#define MTGPU_VGPU_FW_PAGE_TABLE_SIZE	(1 * 1024 * 1024)

enum mtgpu_interrupt_id {
	MTGPU_INTERRUPT_LOCK_ERR = 0,
	MTGPU_INTERRUPT_FEC,
	MTGPU_INTERRUPT_SMC,
	MTGPU_INTERRUPT_AUDIO,
	MTGPU_INTERRUPT_VID_BODA955,
	MTGPU_INTERRUPT_VID_WAVE517,
	MTGPU_INTERRUPT_VID_WAVE517_1,
	MTGPU_INTERRUPT_VID_WAVE517_2,
	MTGPU_INTERRUPT_VID_WAVE517_3,
	MTGPU_INTERRUPT_VID_WAVE517_4,
	MTGPU_INTERRUPT_VID_WAVE517_5,
	MTGPU_INTERRUPT_VID_WAVE627,
	MTGPU_INTERRUPT_VID_WAVE627_1,
	MTGPU_INTERRUPT_VID_WAVE627_2,
	MTGPU_INTERRUPT_VID_WAVE627_3,
	MTGPU_INTERRUPT_GPU0,
	MTGPU_INTERRUPT_GPU1,
	MTGPU_INTERRUPT_GPU2,
	MTGPU_INTERRUPT_GPU3,
	MTGPU_INTERRUPT_GPU4,
	MTGPU_INTERRUPT_GPU5,
	MTGPU_INTERRUPT_GPU6,
	MTGPU_INTERRUPT_GPU7,
	MTGPU_INTERRUPT_DISPC0,
	MTGPU_INTERRUPT_DISPC1,
	MTGPU_INTERRUPT_DISPC2,
	MTGPU_INTERRUPT_DISPC3,
	MTGPU_INTERRUPT_DPTX0,
	MTGPU_INTERRUPT_DPTX1,
	MTGPU_INTERRUPT_DPTX2,
	MTGPU_INTERRUPT_DPTX3,
	MTGPU_INTERRUPT_DPTX0_PHY,
	MTGPU_INTERRUPT_DPTX1_PHY,
	MTGPU_INTERRUPT_HDMI,
	MTGPU_INTERRUPT_PCIE_DMA,
	MTGPU_INTERRUPT_PCIE_LOCAL,
	MTGPU_INTERRUPT_CTRL_MTLINK0,
	MTGPU_INTERRUPT_CTRL_MTLINK1,
	MTGPU_INTERRUPT_CTRL_MTLINK2,
	MTGPU_INTERRUPT_CTRL_MTLINK3,
	MTGPU_INTERRUPT_WRAP_MTLINK0,
	MTGPU_INTERRUPT_WRAP_MTLINK1,
	MTGPU_INTERRUPT_WRAP_MTLINK2,
	MTGPU_INTERRUPT_WRAP_MTLINK3,
	MTGPU_INTERRUPT_COUNT
};

#define MTGPU_DISP_DEV_NUM		(32)
#define MTGPU_CORE_DEFAULT_BITMASK	BIT(0)

#define BYTES_COUNT_64_BIT		8
#define BYTES_COUNT_32_BIT		4
#define BYTES_COUNT_16_BIT		2
#define BYTES_COUNT_8_BIT		1

#define MTGPU_VPU_MODE_DEFAULT	0
#define MTGPU_VPU_MODE_TEST		1
#define MTGPU_VPU_MODE_DISABLE	2

enum pstate_mode {
	PSTATE_DISABLED = 0, /* pstate disabled */
	PSTATE_ENABLED = 1,  /* pstate enabled */
};

#endif /* _MTGPU_DEFS_H_ */
