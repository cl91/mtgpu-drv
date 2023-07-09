/*
 * @Copyright   Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License     Dual MIT/GPLv2
 */

#ifndef _MTGPU_SEGMENT_H_
#define _MTGPU_SEGMENT_H_

#include "linux-types.h"

#define MTGPU_SEGMENT_ID_DEFAULT	0
#define MTGPU_SEGMENT_ID_KERNEL		1
#define MTGPU_SEGMENT_ID_USER		2

#define MTGPU_SEGMENT_STEP_SIZE         0x400000000ULL
#define MTGPU_MAX_SEGMENT               5
#define MTVPU_MAX_SEGMENT               3
#define MTGPU_SEGMENT_NORMAL_SIZE       0x100000000ULL
#define MTGPU_VPU_GROUP			2

struct mtgpu_device;
struct drm_device;

enum mtgpu_segment_flag {
	SEGMENT_FLAG_VPU_VISIBLE = 0x1,
	SEGMENT_FLAG_DISP_VISIBLE = 0x2,
};

struct mtgpu_segment {
	u64 start;
	u64 size;
	/* 0 is the highest priority */
	int priority;
	int flag;
	int segment_id;
};

struct mtgpu_segment_info {
	int segment_cnt;
	struct mtgpu_segment *segments;
};

int mtgpu_vram_get_segment_for_display(struct drm_device *ddev);
int mtgpu_generate_mem_segments(struct mtgpu_device *mtdev);
void mtgpu_destroy_mem_segments(struct mtgpu_device *mtdev);

#endif /* _MTGPU_SEGMENT_H_ */
