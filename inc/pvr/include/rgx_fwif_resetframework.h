/*************************************************************************/ /*!
@File           rgx_fwif_resetframework.h
@Title          Post-reset work-around framework FW interface
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        Dual MIT/GPLv2

The contents of this file are subject to the MIT license as set out below.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Alternatively, the contents of this file may be used under the terms of
the GNU General Public License Version 2 ("GPL") in which case the provisions
of GPL are applicable instead of those above.

If you wish to allow use of your version of this file only under the terms of
GPL, and not to allow others to use your version of this file under the terms
of the MIT license, indicate your decision by deleting the provisions above
and replace them with the notice and other provisions required by GPL as set
out in the file called "GPL-COPYING" included in this distribution. If you do
not delete the provisions above, a recipient may use your version of this file
under the terms of either the MIT license or GPL.

This License is also included in this distribution in the file called
"MIT-COPYING".

EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ /**************************************************************************/

#if !defined(RGX_FWIF_RESETFRAMEWORK_H)
#define RGX_FWIF_RESETFRAMEWORK_H

#include "img_types.h"
#include "rgx_fwif_shared.h"

typedef struct
{
	union
	{
		IMG_UINT64	uCDMReg_CDM_CB_BASE;			//  defined(RGX_FEATURE_CDM_USER_MODE_QUEUE)
		IMG_UINT64	uCDMReg_CDM_CTRL_STREAM_BASE;	// !defined(RGX_FEATURE_CDM_USER_MODE_QUEUE)
	};
	IMG_UINT64	uCDMReg_CDM_CB_QUEUE;				// !defined(RGX_FEATURE_CDM_USER_MODE_QUEUE)
	IMG_UINT64	uCDMReg_CDM_CB;						// !defined(RGX_FEATURE_CDM_USER_MODE_QUEUE)
} RGXFWIF_RF_REGISTERS;

typedef struct
{

	/* THIS MUST BE THE LAST MEMBER OF THE CONTAINING STRUCTURE */
	RGXFWIF_RF_REGISTERS RGXFW_ALIGN sFWRegisters;

} RGXFWIF_RF_CMD;

/* to opaquely allocate and copy in the kernel */
#define RGXFWIF_RF_CMD_SIZE  sizeof(RGXFWIF_RF_CMD)

#endif /* RGX_FWIF_RESETFRAMEWORK_H */
