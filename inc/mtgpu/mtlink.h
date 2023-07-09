/*
 * @Copyright Copyright (c) Moorethreads Technologies Ltd. All Rights Reserved
 * @License Dual MIT/GPLv2
 */

#ifndef __MTGPU_MTLINK_H__
#define __MTGPU_MTLINK_H__

struct mtgpu_device;

struct mtlink_status {
	unsigned long long crc_data_errs;
	unsigned long long crc_flit_errs;
	unsigned long long ecc_errs;
	unsigned long long recovery;
	unsigned long long replay;
};

int mtlink_driver_init(void);
void mtlink_driver_exit(void);

int mtlink_device_init(struct mtgpu_device *mtdev);
void mtlink_device_exit(struct mtgpu_device *mtdev);

int mtlink_get_link_status(struct mtgpu_device *mtdev, int link_id,
			   struct mtlink_status *link_status);
bool mtlink_supported_betwin_devices(struct device *dev1, struct device *dev2);
int mtlink_get_card_id(struct device *dev, int *card_id, int *card_id_shift, int *card_id_mask);
int mtlink_is_enabled(void);

#endif /*__MTGPU_MTLINK_H__*/
