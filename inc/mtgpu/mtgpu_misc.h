/*
 * @Copyright Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License Dual MIT/GPLv2
 */

#ifndef __MTGPU_MISC_H__
#define __MTGPU_MISC_H__

#include "mtgpu_ipc.h"

struct device_info {
	u32 ipc_version;
	u16 vendor_id, device_id;
	u64 reserved;
};

struct ipc_msg_interface {
	u64 header;
	u8 data[PCIE_IPC_MSG_MAX_DATA_SIZE];
};

/* WARNING: MKIS Managed! */
/* !!! CAUTION !!! This value MUST match the definition in MTML */
#define MTGPU_MISC_MEM_ALLOC_MAX        (0x400000)  /* max alloc size 4MB */
struct memory_address {
	phys_addr_t mem_phys_addr;
	size_t mem_size;
};

/* only used for sync_io */
#define MTGPU_MISC_IPC_MESSAGE_TRANSMIT		_IOWR('M', 0x1, struct ipc_msg_interface)
/* only used for async_io */
#define MTGPU_MISC_IPC_IOCTR_SUB		_IOW('M', 0x6, unsigned char)
#define MTGPU_MISC_IPC_IOCTR_UNSUB		_IOW('M', 0x7, unsigned char)

/*
 * MKIS Managed IOCTL Number definitions begin
 *
 * !!! BE CAUTIONED !!!
 * Below IOCTL Numbers are managed by MKIS protocol. Any changing to these
 * contents may lead to compatibility problem against libmtml, so please
 * make sure you have fully understood the purpose of MKIS and how it
 * works before doing any code change. If you indeed have necessity to
 * make changes to these interfaces but don't know how to be compliant with
 * MKIS requirement, please contact zheng.cao@mthreads.com for help.
 * Also, please make sure zheng.cao@mthreads.com is included in your PR
 * reviewer list.
 */
#define MTGPU_MISC_MEMORY_ALLOC			_IOWR('M', 0x3, struct memory_address)
#define MTGPU_MISC_MEMORY_FREE			_IOWR('M', 0x4, struct memory_address)
#define MTGPU_MISC_GET_DEVICE_INFO		_IOR('M', 0x8, struct device_info)
/*
 * MKIS Managed IOCTL Number definitions end
 */

bool mtgpu_misc_is_parent_dev(struct mtgpu_misc_info *misc_info);
int mtgpu_find_misc_dev(int misc_id, struct mtgpu_misc_info **miscinfo);
int mtgpu_register_misc_parent_device(struct mtgpu_device *mtdev);
int mtgpu_register_misc_instance_devices(struct mtgpu_device *mtdev);
void mtgpu_unregister_misc_instance_devices(struct mtgpu_device *mtdev);
void mtgpu_unregister_misc_parent_device(struct mtgpu_device *mtdev);
int mtgpu_misc_init(void);
void mtgpu_misc_deinit(void);

#endif /* __MTGPU_MISC_H__ */
