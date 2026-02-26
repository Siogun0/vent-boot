/*
 * xcp_platform.h
 *
 *  Created on: Dec 3, 2021
 *      Author: arh
 */

#ifndef INC_XCP_PLATFORM_H_
#define INC_XCP_PLATFORM_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
//#include "stm32f3xx_hal_crc.h"
#include "xcp_client.h"
#include "utils.h"

#define XCP_BASE_ID	0x7AE

#define SECTOR_SIZE 1024
//#define SECTOR_0_ADRESS 0x8020000

#define SECTOR_0_FW	0x8004000
#define SECTORS_FOR_FW	48
#define CRC_ADDRESS 0x801FFFC

#define SECTOR_CALIB	0x08003C00

#define BOOT_MGG_CLEAR		0U
#define BOOT_MSG_XCP_REQ	0x01U
#define BOOT_MSG_UDS_REQ	0x10U

void go_to_application(void);

#endif /* INC_XCP_PLATFORM_H_ */
