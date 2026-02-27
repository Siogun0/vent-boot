/*
 * xcp_platform.c
 *
 *  Created on: Dec 3, 2021
 *      Author: arh
 */

#include <stdint.h>
#include <stddef.h>

#include "xcp_platform.h"

extern void deinit_perif(void);

uint8_t check_programm_available(void)
{
//	// check availability firmware
//
	uint8_t available = 1;

	uint32_t* firmware_first_word;
	uint32_t* calibr_first_word;
	firmware_first_word = (uint32_t*)SECTOR_0_FW;
	calibr_first_word = (uint32_t*)SECTOR_CALIB;

	if(firmware_first_word[0] == 0xFFFFFFFF || firmware_first_word[0] == 0x00000000 ||
			firmware_first_word[1] == 0xFFFFFFFF || firmware_first_word[1] == 0x000000000)// ||
			//calibr_first_word[0] == 0xFFFFFFFF || calibr_first_word[1] == 0xFFFFFFFF)
	{
		available = 0;
	}

	return available;
}

size_t flash_range_check(uint32_t mta, uint32_t range)
{
	if((mta < SECTOR_0_FW || (mta + range) > (SECTOR_0_FW + SECTORS_FOR_FW * SECTOR_SIZE)) && (mta < SECTOR_CALIB || (mta + range) > (SECTOR_CALIB + SECTOR_SIZE)))
	{
		return 0;
	}
	return 1;
}

void go_to_application(void)
{
	uint32_t appJumpAddress;

	if(check_programm_available() == 1)
	{
		deinit_perif();

		void (*GoToApp)(void);
		appJumpAddress = *((volatile uint32_t*)(SECTOR_0_FW + 4));
		GoToApp = (void (*)(void))appJumpAddress;
//		SCB->VTOR = SECTOR_0_FW;
		__set_MSP(*((volatile uint32_t*) SECTOR_0_FW)); //stack pointer (to RAM) for USER app in this address
		GoToApp();
	}

}


void *xcp_memcpy(void *dest __attribute__((unused)),
		const void *src __attribute__((unused)),
		uint32_t n __attribute__((unused)))
{
	return NULL;
}

void xcp_program_start(void)
{

}

void xcp_program_reset(void)
{
	go_to_application();
}

uint8_t xcp_program_clear(uint32_t mta, uint32_t range)
{
	uint8_t res = XCP_RESP_POS;
	FLASH_EraseInitTypeDef flash_eraseInitStruct =
	{
			.TypeErase = FLASH_TYPEERASE_PAGES,
			.PageAddress = SECTOR_0_FW,
			.NbPages = SECTORS_FOR_FW
	};
	uint32_t out;


	if(!flash_range_check(mta, range))
	{
		res = XCP_ERR_OUT_OF_RANGE;
	}
	else if(mta % SECTOR_SIZE != 0 || range % SECTOR_SIZE != 0)
	{
		res = XCP_ERR_PAGE_NOT_VALID;
	}
	else
	{
		flash_eraseInitStruct.PageAddress = mta;
		flash_eraseInitStruct.NbPages = range / SECTOR_SIZE;

		HAL_FLASH_Unlock();
		if(HAL_FLASHEx_Erase(&flash_eraseInitStruct, &out) != HAL_OK)
		{
			res = XCP_ERR_PGM_ACTIVE; //TODO check right code
		}
		HAL_FLASH_Lock();
	}

	return res;
}

uint8_t xcp_program(uint32_t mta, uint8_t *buf, uint8_t len)
{
	uint8_t res = XCP_RESP_POS;
	uint64_t data_buf;

	if(!flash_range_check(mta, len))
	{
		res = XCP_ERR_OUT_OF_RANGE;
	}
	else if(len % 2 != 0)
	{
		res = XCP_ERR_SEGMENT_NOT_VALID;
	}
	else
	{
		HAL_FLASH_Unlock();

		for(int i = 0; i < len; i += 2)
		{
			data_buf = buf[i] + (buf[i + 1] << 8);
			HAL_StatusTypeDef flash_res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, mta + i, data_buf);
			if(flash_res != HAL_OK)
			{
				res = XCP_ERR_GENERIC;
			}
		}

		HAL_FLASH_Lock();
	}
	return res;
}
