/*
 * can_platform.h
 *
 *  Created on: Dec 3, 2021
 *      Author: arh
 */

#ifndef INC_CAN_PLATFORM_H_
#define INC_CAN_PLATFORM_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define VIRT_MBX_MAX	14

typedef struct
{
	uint32_t id;
	uint64_t data;
	uint32_t arrived;
	uint32_t filter_no;
	uint32_t mbx_no;
} virtual_mailbox_t;

void can_platform_msg_recieve(CAN_RxHeaderTypeDef *rx_header, uint8_t data[]);

void can_platform_init(void);

void can_init_rx_mb(uint32_t bus_id, uint32_t mbn, uint32_t id, uint32_t dlc);

void can_init_tx_mb(uint32_t bus_id, uint32_t mbn, uint32_t id, uint32_t dlc);

void can_xmit_mb(uint32_t bus_id, uint32_t mbn, uint64_t msg);

void can_dyn_xmit_mb(uint32_t bus_id, uint32_t mbn, uint32_t id, uint32_t dlc, uint64_t msg);

uint64_t can_get_mb_data(uint32_t bus_id, uint32_t mbn);

//uint32_t can_get_mb_dlc(uint32_t bus_id, uint32_t mbn);

uint32_t can_is_message_arrived(uint32_t bus_id, uint32_t mbn);

uint32_t can_is_message_sent(uint32_t bus_id, uint32_t mbn);

#endif /* INC_CAN_PLATFORM_H_ */
