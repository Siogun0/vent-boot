#include "xcp_client.h"
#include <string.h>

//#include "console/console.h"

#define DPIN_EN 0
//#include "dpin.h"


uint16_t xcp_get_word(void *addr)
{
	uint16_t ret;

	memcpy(&ret, addr, sizeof(uint16_t));
	return ret;
}

uint32_t xcp_get_dword(void *addr)
{
	uint32_t ret;

	memcpy(&ret, addr, sizeof(uint32_t));
	return ret;
}

uint32_t xcp_set_byte(void *addr, uint8_t v)
{
	memcpy(addr, &v, sizeof(uint8_t));
	return sizeof(uint8_t);
}

uint32_t xcp_set_word(void *addr, uint16_t v)
{
	memcpy(addr, &v, sizeof(uint16_t));
	return sizeof(uint16_t);
}

uint32_t xcp_set_dword(void *addr, uint32_t v)
{
	memcpy(addr, &v, sizeof(uint32_t));
	return sizeof(uint32_t);
}

uint32_t xcp_get_max_odt(const t_xcp_client_config *cfg, uint16_t daq_nr)
{
	return cfg->daq_lists[daq_nr].en_pid_off ? 1 : cfg->daq_lists[daq_nr].max_odt;
}

uint32_t xcp_get_max_odt_entries(const t_xcp_client_config *cfg, uint16_t daq_nr)
{
	return cfg->daq_lists[daq_nr].total_odt_entries / xcp_get_max_odt(cfg, daq_nr);
}

t_odt_entry *xcp_get_odt_entry(const t_xcp_client_config *cfg, uint16_t daq_nr, uint8_t odt_nr, uint8_t entry_nr)
{
	return &cfg->daq_lists[daq_nr].odt_entries[entry_nr + odt_nr * xcp_get_max_odt_entries(cfg, daq_nr)];
}

uint8_t xcp_get_abs_odt_number(const t_xcp_client_config *cfg, uint16_t daq_nr, uint8_t odt_nr)
{
	uint8_t abs_first_odt_nr = 0;
	uint8_t i;

	for(i = 0; i < daq_nr; i++)
	{
		abs_first_odt_nr += xcp_get_max_odt(cfg, daq_nr);
	}
	return abs_first_odt_nr + odt_nr;
}

//void print_cmd(const char *prefix, uint8_t *buf, uint32_t len)
//{
//	char str[256];
//	int p = 0;
//	uint32_t i;
//
//	p += sprintf(str, "%s ", prefix);
//
//	for(i = 0; i < (len > 20 ? 20 : len); i++)
//	{
//		p += sprintf(str + p, "%.2X ", buf[i]);
//	}
//
//	if(len > 20)
//	{
//		p += sprintf(str + p, " ...");
//	}
//
//	PRINTF("%s\n", str);
//}

void xcp_send(const t_xcp_client_config *cfg, uint8_t *buf, uint32_t len)
{
	//print_cmd("TX:", buf, len);

	cfg->xcp_send_cto_cb(buf, len);
}

void xcp_cmd(const t_xcp_client_config *cfg, uint8_t *buf, uint32_t len)
{
	if(len < 1)
	{
		return;
	}

	//print_cmd("RX:", buf, len);

	switch(buf[0])
	{
	case XCP_CONNECT:
		xcp_init(cfg);
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = (cfg->en_cal ? 0x01 : 0) |
				(cfg->en_daq ? 0x04 : 0) |
				(cfg->en_stim ? 0x08 : 0) |
				(cfg->en_pgm ? 0x10 : 0);
		cfg->cto_resp_buf[2] = (cfg->en_blk ? 0x40 : 0);
		cfg->cto_resp_buf[3] = cfg->max_cto;
		xcp_set_word(&cfg->cto_resp_buf[4], cfg->max_dto);
		cfg->cto_resp_buf[6] = 1; //version major
		cfg->cto_resp_buf[7] = 0; //version minor
		xcp_send(cfg, cfg->cto_resp_buf, 8);
		break;
	case XCP_DISCONNECT:
		xcp_init(cfg);
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		xcp_send(cfg, cfg->cto_resp_buf, 1);
		break;

	case XCP_GET_COMM_MODE_INFO:
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = 0;
		cfg->cto_resp_buf[2] = 0;
		cfg->cto_resp_buf[3] = 0;
		cfg->cto_resp_buf[4] = 0;
		cfg->cto_resp_buf[5] = 0;
		cfg->cto_resp_buf[6] = 0;
		cfg->cto_resp_buf[7] = 0x10;
		xcp_send(cfg, cfg->cto_resp_buf, 8);
		break;

	case XCP_SYNCH:
		cfg->cto_resp_buf[0] = XCP_RESP_NEG;
		cfg->cto_resp_buf[1] = XCP_ERR_CMD_SYNCH;
		xcp_send(cfg, cfg->cto_resp_buf, 2);
		break;

	case XCP_SET_REQUEST:
		cfg->ctx->session_conf_id = xcp_get_word(&buf[2]);
		if(buf[1] & 0xe)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		else
		{
			if(buf[1] & 1)
			{
				/*TODO: store calibration here*/
			}
			cfg->cto_resp_buf[0] = XCP_RESP_POS;
			xcp_send(cfg, cfg->cto_resp_buf, 1);
		}
		break;

	case XCP_GET_STATUS:
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = 0;
		cfg->cto_resp_buf[2] = 0;
		cfg->cto_resp_buf[3] = 0;
		xcp_set_word(&cfg->cto_resp_buf[4], cfg->ctx->session_conf_id);
		xcp_send(cfg, cfg->cto_resp_buf, 6);
		break;

	case XCP_SET_MTA:
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		memcpy(&cfg->ctx->mta, &buf[4], sizeof(uint32_t));
		xcp_send(cfg, cfg->cto_resp_buf, 1);
		break;

	case XCP_SHORT_UPLOAD:
		cfg->ctx->mta = xcp_get_dword(&buf[4]);
		/* no break */
	case XCP_UPLOAD:
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		memcpy(&cfg->cto_resp_buf[1], (void *)cfg->ctx->mta, buf[1]);
		cfg->ctx->mta += buf[1];
		xcp_send(cfg, cfg->cto_resp_buf, buf[1] + 1);
		break;


	case XCP_SHORT_DOWNLOAD:
		if(!cfg->en_cal)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->ctx->mta = xcp_get_dword(&buf[4]);
		xcp_memcpy((void *)cfg->ctx->mta, &buf[8], buf[1]);
		cfg->ctx->mta += buf[1];
		xcp_send(cfg, cfg->cto_resp_buf, buf[1] + 1);
		break;

	case XCP_DOWNLOAD:
		if(!cfg->en_cal)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		xcp_memcpy((void *)cfg->ctx->mta, &buf[2], buf[1]);
		cfg->ctx->mta += buf[1];
		xcp_send(cfg, cfg->cto_resp_buf, buf[1] + 1);
		break;


	case XCP_SET_DAQ_PTR:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		if(xcp_get_word(&buf[2]) >= cfg->daq_lists_max)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		else
		{
			if((buf[4] >= xcp_get_max_odt(cfg, xcp_get_word(&buf[2]))) ||
					(buf[5] >= xcp_get_max_odt_entries(cfg, xcp_get_word(&buf[2]))))
			{
				cfg->cto_resp_buf[0] = XCP_RESP_NEG;
				cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
				xcp_send(cfg, cfg->cto_resp_buf, 2);
			}
			else
			{
				cfg->ctx->daq_ptr = xcp_get_word(&buf[2]);
				cfg->ctx->odt_ptr = buf[4];
				cfg->ctx->odt_entry_ptr = buf[5];
				//PRINTF("XCP_SET_DAQ_PTR: daq = %d, odt = %d, entry = %d\n", cfg->ctx->daq_ptr, cfg->ctx->odt_ptr, cfg->ctx->odt_entry_ptr);

				cfg->cto_resp_buf[0] = XCP_RESP_POS;
				xcp_send(cfg, cfg->cto_resp_buf, 1);
			}
		}
		break;

	case XCP_WRITE_DAQ:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		if(cfg->ctx->daq_ptr >= cfg->daq_lists_max)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}

		if(cfg->ctx->odt_ptr >= xcp_get_max_odt(cfg, cfg->ctx->daq_ptr))
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}

		if(cfg->ctx->odt_entry_ptr >= xcp_get_max_odt_entries(cfg, cfg->ctx->daq_ptr))
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}

		{
			t_odt_entry *e = xcp_get_odt_entry(cfg, cfg->ctx->daq_ptr, cfg->ctx->odt_ptr, cfg->ctx->odt_entry_ptr);
			e->bit_off = buf[1];
			e->sz = buf[2];
			e->mta = xcp_get_dword(&buf[4]);
			//PRINTF("XCP_WRITE_DAQ: mta = 0x%.8X, sz = %d, bit_off = %d\n", e->mta, e->sz, e->bit_off);
			cfg->ctx->odt_entry_ptr++;
			cfg->cto_resp_buf[0] = XCP_RESP_POS;
			xcp_send(cfg, cfg->cto_resp_buf, 1);
		}

		break;

	case XCP_CLEAR_DAQ_LIST:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		if(xcp_get_word(&buf[2]) >= cfg->daq_lists_max)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		else
		{
			cfg->daq_lists[xcp_get_word(&buf[2])].ctx->run = 0;
			memset(cfg->daq_lists[xcp_get_word(&buf[2])].odt_entries, 0,
					sizeof(t_odt_entry) * cfg->daq_lists[xcp_get_word(&buf[2])].total_odt_entries);

			cfg->cto_resp_buf[0] = XCP_RESP_POS;
			xcp_send(cfg, cfg->cto_resp_buf, 1);
		}
		break;

	case XCP_START_STOP_DAQ_LIST:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		if(xcp_get_word(&buf[2]) >= cfg->daq_lists_max)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		else
		{
			cfg->daq_lists[xcp_get_word(&buf[2])].ctx->run = buf[1];
			cfg->cto_resp_buf[0] = XCP_RESP_POS;
			cfg->cto_resp_buf[1] = xcp_get_abs_odt_number(cfg, xcp_get_word(&buf[2]), 0);
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		break;

	case XCP_START_STOP_SYNCH:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		{
			uint16_t i;

			for(i = 0; i < cfg->daq_lists_max; i++)
			{
				switch(buf[1])
				{
				case 1: //run selected
					cfg->daq_lists[i].ctx->run = cfg->daq_lists[i].ctx->run == 2 ? 1 : cfg->daq_lists[i].ctx->run;
					break;
				case 2: //stop selected
					cfg->daq_lists[i].ctx->run = cfg->daq_lists[i].ctx->run == 2 ? 0 : cfg->daq_lists[i].ctx->run;
					break;
				case 0: //stop all
				default:
					cfg->daq_lists[i].ctx->run = 0;
					break;
				}
			}
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		xcp_send(cfg, cfg->cto_resp_buf, 1);
		break;

	case XCP_GET_DAQ_PROCESSOR_INFO:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = 1 << 1 | 1 << 4 | 1 << 5;
		xcp_set_word(&cfg->cto_resp_buf[2], cfg->daq_lists_max);
		xcp_set_word(&cfg->cto_resp_buf[4], cfg->events_max);
		cfg->cto_resp_buf[6] = cfg->daq_lists_max;
		cfg->cto_resp_buf[7] = 0x30;
		xcp_send(cfg, cfg->cto_resp_buf, 8);
		break;

	case XCP_GET_DAQ_RESOLUTION_INFO:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = 0;
		cfg->cto_resp_buf[2] = 4;
		cfg->cto_resp_buf[3] = 0;
		cfg->cto_resp_buf[4] = 4;
		cfg->cto_resp_buf[5] = cfg->time_stamp_size | cfg->time_stamp_unit << 4 | cfg->time_stamp_fixed << 3;
		xcp_set_word(&cfg->cto_resp_buf[6], cfg->time_stamp_tick);
		xcp_send(cfg, cfg->cto_resp_buf, 8);
		break;

	case XCP_GET_DAQ_LIST_INFO:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		if(xcp_get_word(&buf[2]) >= cfg->daq_lists_max)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		else
		{
			cfg->cto_resp_buf[0] = XCP_RESP_POS;
			cfg->cto_resp_buf[1] = 	0x01 | //predefined
					(cfg->daq_lists[xcp_get_word(&buf[2])].event_fixed ? 0x02 : 0) |
					cfg->daq_lists[xcp_get_word(&buf[2])].dir << 2;
			cfg->cto_resp_buf[2] = xcp_get_max_odt(cfg, xcp_get_word(&buf[2]));
			cfg->cto_resp_buf[3] = xcp_get_max_odt_entries(cfg, xcp_get_word(&buf[2]));
			xcp_set_word(&cfg->cto_resp_buf[4], cfg->daq_lists[xcp_get_word(&buf[2])].event_nr);
			xcp_send(cfg, cfg->cto_resp_buf, 6);
		}
		break;

	case XCP_GET_DAQ_EVENT_INFO:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		if(xcp_get_word(&buf[2]) >= cfg->events_max)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_OUT_OF_RANGE;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		else
		{
			cfg->cto_resp_buf[0] = XCP_RESP_POS;
			cfg->cto_resp_buf[1] = 0x04 | cfg->events[xcp_get_word(&buf[2])].dir << 2;
			cfg->cto_resp_buf[2] = 255;
			cfg->cto_resp_buf[3] = 0;
			cfg->cto_resp_buf[4] = cfg->events[xcp_get_word(&buf[2])].event_rate;
			cfg->cto_resp_buf[5] = cfg->events[xcp_get_word(&buf[2])].event_time_unit;
			cfg->cto_resp_buf[6] = cfg->events[xcp_get_word(&buf[2])].priority;
			xcp_send(cfg, cfg->cto_resp_buf, 7);
		}
		break;

	case XCP_GET_DAQ_CLOCK:
		if(!cfg->en_daq)
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
			break;
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = 0;
		cfg->cto_resp_buf[2] = 0;
		cfg->cto_resp_buf[3] = 0;
		xcp_set_dword(&cfg->cto_resp_buf[4], cfg->ctx->time_stamp);
		xcp_send(cfg, cfg->cto_resp_buf, 8);
		break;

	case XCP_PROGRAM_START:
		if(cfg->program_start_cb)
		{
			cfg->program_start_cb();
		}
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		cfg->cto_resp_buf[1] = 0;
		cfg->cto_resp_buf[2] = 0;
		cfg->cto_resp_buf[3] = cfg->max_cto;
		cfg->cto_resp_buf[4] = 0;
		cfg->cto_resp_buf[5] = 0;
		cfg->cto_resp_buf[6] = 0;
		xcp_send(cfg, cfg->cto_resp_buf, 7);
		break;

	case XCP_PROGRAM_CLEAR:
		if(cfg->program_clear_cb)
		{
			uint8_t result;

			switch((result = cfg->program_clear_cb(cfg->ctx->mta, xcp_get_dword(&buf[4]))))
			{
			case XCP_RESP_POS:
				cfg->cto_resp_buf[0] = XCP_RESP_POS;
				xcp_send(cfg, cfg->cto_resp_buf, 1);
				break;
			default:
				cfg->cto_resp_buf[0] = XCP_RESP_NEG;
				cfg->cto_resp_buf[1] = result;
				xcp_send(cfg, cfg->cto_resp_buf, 2);
				break;
			}
		}
		else
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		break;

	case XCP_PROGRAM:
		if(cfg->program_cb)
		{
			uint8_t result;

			switch((result = cfg->program_cb(cfg->ctx->mta, &buf[2], buf[1])))
			{
			case XCP_RESP_POS:
				cfg->cto_resp_buf[0] = XCP_RESP_POS;
				xcp_send(cfg, cfg->cto_resp_buf, 1);
				break;
			default:
				cfg->cto_resp_buf[0] = XCP_RESP_NEG;
				cfg->cto_resp_buf[1] = result;
				xcp_send(cfg, cfg->cto_resp_buf, 2);
				break;
			}
			cfg->ctx->mta += buf[1];
		}
		else
		{
			cfg->cto_resp_buf[0] = XCP_RESP_NEG;
			cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
			xcp_send(cfg, cfg->cto_resp_buf, 2);
		}
		break;

	case XCP_PROGRAM_RESET:
		cfg->cto_resp_buf[0] = XCP_RESP_POS;
		xcp_send(cfg, cfg->cto_resp_buf, 1);
		if(cfg->program_reset_cb)
		{
			cfg->program_reset_cb();
		}
		break;

	default:
		cfg->cto_resp_buf[0] = XCP_RESP_NEG;
		cfg->cto_resp_buf[1] = XCP_ERR_CMD_UNKNOWN;
		xcp_send(cfg, cfg->cto_resp_buf, 2);
		break;
	}
}

__attribute__ ((section(".ram_func"))) void xcp_update_time_stamp(const t_xcp_client_config *cfg)
{
	cfg->ctx->time_stamp++;
}

void xcp_init(const t_xcp_client_config *cfg)
{
	uint32_t i;

	memset(cfg->ctx, 0, sizeof(t_xcp_context));

	for(i = 0; i < cfg->daq_lists_max; i++)
	{
		memset(cfg->daq_lists[i].ctx, 0, sizeof(t_daq_context));
		memset(cfg->daq_lists[i].odt_entries, 0, sizeof(t_odt_entry) * cfg->daq_lists[i].total_odt_entries);
	}
}

__attribute__ ((section(".ram_func"))) void xcp_daq_event(const t_xcp_client_config *cfg, uint16_t daq_nr)
{
	uint32_t ptr = 0;
	uint32_t i;
	t_odt_entry *e;

	if(cfg->daq_lists[daq_nr].ctx->run == 1)
	{
		for(i = 0; i < cfg->daq_lists[daq_nr].max_odt; i++)
		{
			ptr = 0;
			ptr += xcp_set_byte(&cfg->daq_lists[daq_nr].dto_buf[ptr], xcp_get_abs_odt_number(cfg, daq_nr, i));
			if(i == 0)
			{
				switch(cfg->time_stamp_size)
				{
				case XCP_TIME_STAMP_BYTE:
					ptr += xcp_set_byte(&cfg->daq_lists[daq_nr].dto_buf[ptr], cfg->ctx->time_stamp);
					break;
				case XCP_TIME_STAMP_WORD:
					ptr += xcp_set_word(&cfg->daq_lists[daq_nr].dto_buf[ptr], cfg->ctx->time_stamp);
					break;
				case XCP_TIME_STAMP_DWORD:
					ptr += xcp_set_dword(&cfg->daq_lists[daq_nr].dto_buf[ptr], cfg->ctx->time_stamp);
					break;
				case XCP_TIME_STAMP_NONE:
				default:
					break;
				}
			}

			for(e = xcp_get_odt_entry(cfg, daq_nr, i, 0); e->mta; e++)
			{
				memcpy(&cfg->daq_lists[daq_nr].dto_buf[ptr], (void *)e->mta, e->sz);
				ptr += e->sz;
			}

			cfg->xcp_send_dto_cb(cfg->daq_lists[daq_nr].dto_buf, ptr, daq_nr);
		}
	}
}
