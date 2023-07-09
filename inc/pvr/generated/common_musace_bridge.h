/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#ifndef _COMMON_MUSACE_BRIDGE_H_
#define _COMMON_MUSACE_BRIDGE_H_

#include <powervr/mem_types.h>

#include "img_defs.h"
#include "img_types.h"
#include "pvrsrv_error.h"

#include "rgx_bridge.h"
#include "pvrsrv_sync_km.h"

#define PVRSRV_BRIDGE_MUSACE_CMD_FIRST                  0
#define PVRSRV_BRIDGE_MUSACE_CREATETRANSFERCONTEXT      PVRSRV_BRIDGE_MUSACE_CMD_FIRST + 0
#define PVRSRV_BRIDGE_MUSACE_DESTROYTRANSFERCONTEXT     PVRSRV_BRIDGE_MUSACE_CMD_FIRST + 1
#define PVRSRV_BRIDGE_MUSACE_SETTRANSFERCONTEXTPRIORITY PVRSRV_BRIDGE_MUSACE_CMD_FIRST + 2
#define PVRSRV_BRIDGE_MUSACE_NOTIFYWRITEOFFSETUPDATE    PVRSRV_BRIDGE_MUSACE_CMD_FIRST + 3
#define PVRSRV_BRIDGE_MUSACE_SUBMITTRANSFER             PVRSRV_BRIDGE_MUSACE_CMD_FIRST + 4
#define PVRSRV_BRIDGE_MUSACE_CMD_LAST                   (PVRSRV_BRIDGE_MUSACE_CMD_FIRST + 4)

typedef struct PVRSRV_BRIDGE_IN_MUSACECREATETRANSFERCONTEXT_TAG
{
	IMG_UINT64 ui64RobustnessAddress;
	IMG_HANDLE hPrivData;
	IMG_BYTE *pui8FrameworkCmd;
	IMG_UINT32 ui32ContextFlags;
	IMG_UINT32 ui32FrameworkCmdSize;
	IMG_UINT32 ui32PackedCCBSizeU88;
	IMG_UINT32 ui32Priority;
} __packed PVRSRV_BRIDGE_IN_MUSACECREATETRANSFERCONTEXT;

typedef struct PVRSRV_BRIDGE_OUT_MUSACECREATETRANSFERCONTEXT_TAG
{
	IMG_HANDLE hTransferContext;
	PVRSRV_ERROR eError;
} __packed PVRSRV_BRIDGE_OUT_MUSACECREATETRANSFERCONTEXT;

typedef struct PVRSRV_BRIDGE_IN_MUSACEDESTROYTRANSFERCONTEXT_TAG
{
	IMG_HANDLE hTransferContext;
} __packed PVRSRV_BRIDGE_IN_MUSACEDESTROYTRANSFERCONTEXT;

typedef struct PVRSRV_BRIDGE_OUT_MUSACEDESTROYTRANSFERCONTEXT_TAG
{
	PVRSRV_ERROR eError;
} __packed PVRSRV_BRIDGE_OUT_MUSACEDESTROYTRANSFERCONTEXT;

typedef struct PVRSRV_BRIDGE_IN_MUSACENOTIFYWRITEOFFSETUPDATE_TAG
{
	IMG_HANDLE hTransferContext;
	IMG_UINT32 ui32PDumpFlags;
} __packed PVRSRV_BRIDGE_IN_MUSACENOTIFYWRITEOFFSETUPDATE;

typedef struct PVRSRV_BRIDGE_OUT_MUSACENOTIFYWRITEOFFSETUPDATE_TAG
{
	PVRSRV_ERROR eError;
} __packed PVRSRV_BRIDGE_OUT_MUSACENOTIFYWRITEOFFSETUPDATE;

typedef struct PVRSRV_BRIDGE_IN_MUSACEDMSUBMITTRANSFER_TAG
{
	IMG_UINT64 ui64DeadlineInus;
	IMG_HANDLE hTransferContext;
	IMG_UINT32 *pui32SyncPMRFlags;
	IMG_UINT32 *pui32UpdateSyncOffset;
	IMG_UINT32 *pui32UpdateValue;
	IMG_UINT8 *pui8FWCommand;
	IMG_CHAR *puiUpdateFenceName;
	IMG_HANDLE *phSyncPMRs;
	IMG_HANDLE *phUpdateUFOSyncPrimBlock;
	PVRSRV_FENCE hCheckFenceFD;
	PVRSRV_TIMELINE hUpdateTimeline;
	IMG_UINT32 ui32Characteristic1;
	IMG_UINT32 ui32Characteristic2;
	IMG_UINT32 ui32ClientUpdateCount;
	IMG_UINT32 ui32CommandSize;
	IMG_UINT32 ui32ExternalJobReference;
	IMG_UINT32 ui32PDumpFlags;
	IMG_UINT32 ui32SyncPMRCount;
} __packed PVRSRV_BRIDGE_IN_MUSACEDMSUBMITTRANSFER;

typedef struct PVRSRV_BRIDGE_OUT_MUSACEDMSUBMITTRANSFER_TAG
{
	PVRSRV_ERROR eError;
	PVRSRV_FENCE hUpdateFence;
} __packed PVRSRV_BRIDGE_OUT_MUSACEDMSUBMITTRANSFER;

#endif /* _COMMON_MUSACE_BRIDGE_H_ */
