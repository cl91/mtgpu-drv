/*
 * @Copyright Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License Dual MIT/GPLv2
 */

#ifndef __MTGPU_IPC_H__
#define __MTGPU_IPC_H__

#define MTGPU_IPC_VERSION               (2)

/* IPC message send result status */
#define PCIE_IPC_MSG_DONE               (0)
#define PCIE_IPC_MSG_DONE_WITH_RESPONSE (1) /* ipc msg send done and reveice the response msg */
#define PCIE_IPC_MSG_SEND_TIMEOUT       (2) /* ipc msg send fail */
#define PCIE_IPC_MSG_RESPONSE_TIMEOUT   (3) /* ipc msg send done but can not get response msg */
#define PCIE_IPC_MSG_SELF_ERROR         (4) /* msg content has error */
#define PCIE_IPC_INIT_FAIL              (5) /* ipc init fail, check ipc buffer fail */
#define PCIE_IPC_UNSUPPORTED            (6) /* ipc event unsupported */
#define PCIE_IPC_UNPERMITTED            (7) /* ipc event unpermitted */

#define PCIE_IPC_MSG_MAX_DATA_SIZE      (256)

#define MTGPU_IPC_NODE_SMC            0x0	/* SMC */
#define MTGPU_IPC_NODE_FEC            0x1	/* FEC */
#define MTGPU_IPC_NODE_PCIE           0x2	/* PCIe HOST*/
#define MTGPU_IPC_NODE_DSP            0x3	/* DSP */

/* event type start from 0, and the event type would be pair defined */
/* send to smc event type is x(even number), smc reply event type is x+1(odd number) */
enum event_type {
	EVENT_TYPE_HOST_TO_SMC  = 0,
	EVENT_TYPE_SMC_TO_HOST  = 1,
	EVENT_TYPE_HOST_TO_FEC  = 2,
	EVENT_TYPE_FEC_TO_HOST  = 3,
	EVENT_TYPE_FEC_TO_SMC   = 4,
	EVENT_TYPE_SMC_TO_FEC   = 5,
	EVENT_TYPE_MAX
};

enum event_id {
	IPC_EVENT_FW_UPDATE                  = 0,
	IPC_EVENT_FW_UPDATE_PERCENT          = 1,
	IPC_EVENT_GET_VERIF_RET              = 2,
	IPC_EVENT_WRITE_VAR                  = 3,
	IPC_EVENT_READ_VAR                   = 4,
	IPC_EVENT_VBIOS_LOG                  = 5,
	IPC_EVENT_TEST                       = 6,
	IPC_EVENT_PROGRAM_EFUSE,
	IPC_EVENT_GET_EFUSE                  = 8,
	IPC_EVENT_SS_RESET                   = 9,
	IPC_EVENT_GET_PARTITION_TABLE        = 10,
	IPC_EVENT_GET_FW_VERSION,
	IPC_EVENT_I2C_OP,
	IPC_EVENT_SET_NVRAM_ENTRY,
	IPC_EVENT_GET_NVRAM_ENTRY,
	IPC_EVENT_GET_LOADER_INFO            = 15,

	IPC_EVENT_MTC_COMMAND                = 17,

	IPC_EVENT_FEC_BOOT_CMD               = 26,

	IPC_EVENT_FEC_TTY_NOTIFY             = 30,
	IPC_EVENT_FEC_POWER_MGR              = 31,
	IPC_EVENT_FEC_KMD_DRV                = 32,

	IPC_EVENT_GET_MAX_VOLTAGE            = 60,
	IPC_EVENT_GET_VOLTAGE,
	IPC_EVENT_SET_VOLTAGE,
	IPC_EVENT_GET_PSTATE_ENTRY_INFO      = 68,

	IPC_EVENT_GET_MAX_FREQ               = 70,
	IPC_EVENT_GET_FREQ,
	IPC_EVENT_SET_FREQ,

	IPC_EVENT_GET_TMP                    = 80,
	IPC_EVENT_GET_FAN_SPEED,
	IPC_EVENT_SET_FAN_SPEED,
	IPC_EVENT_GET_POWER,
	IPC_EVENT_GET_MAX_TMP,
	IPC_EVENT_GET_PVT_PROCESS,
	IPC_EVENT_GET_PVT_VOLTAGE,

	IPC_EVENT_DVFS                       = 90,
	IPC_EVENT_SET_POWER_STATE,
	IPC_EVENT_GET_POWER_STATE,
	IPC_EVENT_PRE_DVFS,
	IPC_EVENT_POST_DVFS,
	IPC_EVENT_PRE_SET_POWER_STATE,
	IPC_EVENT_POST_SET_POWER_STATE,

	IPC_EVENT_GET_VPU_INFO               = IPC_EVENT_GET_EFUSE,
	IPC_EVENT_GET_GPU_INFO               = 100,
	IPC_EVENT_SET_GPU_INFO               = 101,
	IPC_EVENT_CFG_GPU_EATA               = 102,

	IPC_EVENT_REPORT_EVENT               = 120,
	IPC_EVNET_PERM_BREAK_SET             = 121,

	IPC_EVENT_GET_DEVICE_INFO            = 125,
	IPC_EVENT_DEBUG                      = 126,
	IPC_EVENT_ID_MAX                     = 127
};

enum smc_exception {
	IPC_EVENT_REPORT_EXCEPTION_TEMPERATURE = 0,
	IPC_EVENT_REPORT_EXCEPTION_POWER_LIMIT = 1,
	IPC_EVENT_REPORT_EXCEPTION_FAN	       = 2,
	IPC_EVENT_REPORT_EXCEPTION_SHUTDOWN    = 3,
	IPC_EVENT_REPORT_EXCEPTION_EATA_ERR    = 4,

	IPC_EVENT_REPORT_EXCEPTION_MAX
};

#pragma pack(1)

struct ipc_msg_hdr {
	u64 event_type  : 5; /* type id with maximum 32 types messages */
	u64 event_id    : 7; /* event id, can put into SGI IRQ event */
	u64 event_pri   : 2; /* priority */
	u64 msg_id      : 8; /* for a pair of request/complete msg, msg_id are same */
	u64 msg_type    : 1; /* 0: request message, 1: response message */
	u64 msg_sync    : 1; /* 0:asnyc, 1:sync */
	u64 msg_ack     : 1; /* the message ack */
	u64 response    : 1; /* 0:don't response, 1:need response message */
	u64 pack        : 1; /* 0 - one payload, 1 - multi-payload */
	u64 location    : 2; /* 0:payload place into share sram, 1:vDDR, 2:system DDR, 3:reserve */
	u64 source      : 8; /* souce ID */
	u64 target      : 8; /* target ID */
	u64 data_size   : 8; /* payload size, 1 unit means 1 uint32(4 bytes), minimum value is 1 */
	u64 event_error : 2; /* 0: event ok, 1: event unsupported 2: event unpermitted, 3: reserve*/
	u64 rsv         : 9; /* reserved */
};

/* use fixed payload data size, valid data size must not bigger than PCIE_IPC_MSG_MAX_DATA_SIZE */
struct ipc_msg {
	struct ipc_msg_hdr header;              /* message header */
	u8 data[PCIE_IPC_MSG_MAX_DATA_SIZE];    /* payload */
};

#pragma pack()

struct ipc_report_hdr {
	uint64_t exception_id   : 8;  /* exception id, max 256 types */
	uint64_t rpt_data_size  : 8;  /* report data size, max 256 bytes */
	uint64_t rsv            : 48; /* reserved */
};

struct ipc_report {
	struct ipc_report_hdr rpt_hdr;
	u8 data[0];
};

struct device;
struct mtgpu_device;

int mtgpu_ipc_init(struct mtgpu_device *mtdev);
void mtgpu_ipc_exit(struct mtgpu_device *mtdev);

/**
 * send ipc message, use the send message as reply message buffer if it have a reply message
 *
 * @param  dev gpu device
 * @param  msg message
 *
 * @return status
 */
int mtgpu_ipc_transmit(struct device *dev, struct ipc_msg *msg);

/**
 * set the async_msg_handler. it will be called when receiving a unexpected message.
 *
 * @param  dev gpu device
 * @param  hook handler function
 * @param  param handler function user pointer
 *
 * @return status
 */
int mtgpu_ipc_async_msg_handler_register(struct device *dev,
					 void (*hook)(void *, struct ipc_msg *),
					 void *param);

#endif /* __MTGPU_IPC_H__ */

