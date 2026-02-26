/*
 * can_platform.c
 *
 *  Created on: Dec 3, 2021
 *      Author: arh
 */
#include "can_platform.h"

extern CAN_HandleTypeDef hcan;

static virtual_mailbox_t virt_mbx[VIRT_MBX_MAX] = {0};

void can_platform_msg_recieve(CAN_RxHeaderTypeDef *rx_header, uint8_t data[])
{
	for(int i = 0; i < VIRT_MBX_MAX; i++)
	{
		if(rx_header->StdId == virt_mbx[i].id)
		{
			virt_mbx[i].arrived = 1;
			virt_mbx[i].filter_no = rx_header->FilterMatchIndex;
			virt_mbx[i].data = (uint64_t)data[0] << 0 |
					(uint64_t)data[1] << 8 |
					(uint64_t)data[2] << 16 |
					(uint64_t)data[3] << 24 |
					(uint64_t)data[4] << 32 |
					(uint64_t)data[5] << 40 |
					(uint64_t)data[6] << 48 |
					(uint64_t)data[7] << 56;
			return;
		}
	}
}

void can_platform_init()
{
	  if(HAL_CAN_Start(&hcan) != HAL_OK) return;
	  if(HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING |
			  	  	  	  	  	  	  	  	 CAN_IT_RX_FIFO0_FULL |
											 //CAN_IT_RX_FIFO1_MSG_PENDING |
											 //CAN_IT_RX_FIFO1_FULL |
											 //CAN_IT_TX_MAILBOX_EMPTY |
											 //CAN_IT_BUSOFF |
											 CAN_IT_RX_FIFO0_OVERRUN
											 ) != HAL_OK) return;
}

void can_init_rx_mb(uint32_t bus_id, uint32_t mbn, uint32_t id, uint32_t dlc)
{
	uint32_t mask_or_id;

	virt_mbx[mbn].id = id;

	  CAN_FilterTypeDef CAN_FilterStructure;
	  CAN_FilterStructure.FilterBank = mbn;
	  CAN_FilterStructure.FilterMode = 0; //TODO dynamic numbering filter by ID

	  // for standart ID
	  id = (id & 0b11111111111) << 21;
	  mask_or_id = 0xFFE00000;

	  CAN_FilterStructure.FilterMaskIdHigh = mask_or_id >> 16;
	  CAN_FilterStructure.FilterMaskIdLow = mask_or_id & 0xFFFF;
	  CAN_FilterStructure.FilterIdHigh = id >> 16;
	  CAN_FilterStructure.FilterIdLow = id & 0xFFFF;

	  CAN_FilterStructure.FilterScale = CAN_FILTERSCALE_32BIT;
	  CAN_FilterStructure.FilterActivation = CAN_FILTER_ENABLE;

	  CAN_FilterStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	  HAL_CAN_ConfigFilter(&hcan, &CAN_FilterStructure);


}

void can_init_tx_mb(uint32_t bus_id, uint32_t mbn, uint32_t id, uint32_t dlc)
{
	virt_mbx[mbn].id = id;
}

void can_xmit_mb(uint32_t bus_id, uint32_t mbn, uint64_t msg)
{
	can_dyn_xmit_mb(bus_id, mbn, virt_mbx[mbn].id, 8, msg);
}

void can_dyn_xmit_mb(uint32_t bus_id, uint32_t mbn, uint32_t id, uint32_t dlc, uint64_t msg)
{
	CAN_TxHeaderTypeDef CAN_TxHeader;
	uint8_t data_byte[8];

	// for standart ID
	CAN_TxHeader.StdId = id;

	CAN_TxHeader.IDE = 0;
	CAN_TxHeader.RTR = 0;
	CAN_TxHeader.DLC = dlc;

	data_byte[0] = (msg >> 0) & 0xFF;
	data_byte[1] = (msg >> 8) & 0xFF;
	data_byte[2] = (msg >> 16) & 0xFF;
	data_byte[3] = (msg >> 24) & 0xFF;
	data_byte[4] = (msg >> 32) & 0xFF;
	data_byte[5] = (msg >> 40) & 0xFF;
	data_byte[6] = (msg >> 48) & 0xFF;
	data_byte[7] = (msg >> 56) & 0xFF;

	HAL_CAN_AddTxMessage(&hcan, &CAN_TxHeader, data_byte, &(virt_mbx[mbn].mbx_no));
}

uint64_t can_get_mb_data(uint32_t bus_id, uint32_t mbn)
{
	virt_mbx[mbn].arrived = 0;
	return virt_mbx[mbn].data;
}

//uint32_t can_get_mb_dlc(uint32_t bus_id, uint32_t mbn)
//{
////	return MODULE_CAN.MO[CAN_NODE_MO(bus_id, mbn)].FCR.B.DLC;
//}

uint32_t can_is_message_arrived(uint32_t bus_id, uint32_t mbn)
{
	return virt_mbx[mbn].arrived;
}

uint32_t can_is_message_sent(uint32_t bus_id, uint32_t mbn)
{
	return !HAL_CAN_IsTxMessagePending(&hcan, virt_mbx[mbn].mbx_no);
}
