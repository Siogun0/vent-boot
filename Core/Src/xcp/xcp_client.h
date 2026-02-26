#ifndef XCP_CLIENT_H_
#define XCP_CLIENT_H_

#include <stdint.h>

typedef enum
{
	XCP_UNIT_1NS	= 0,
	XCP_UNIT_10NS = 1,
	XCP_UNIT_100NS = 2,
	XCP_UNIT_1US	= 3,
	XCP_UNIT_10US = 4,
	XCP_UNIT_100US = 5,
	XCP_UNIT_1MS = 6,
	XCP_UNIT_10MS = 7,
	XCP_UNIT_100MS = 8,
	XCP_UNIT_1S	= 9,
	XCP_UNIT_1PS	= 10,
	XCP_UNIT_10PS = 11,
	XCP_UNIT_100PS = 12
} t_xcp_time_unit;

typedef enum
{
	EV_DIR_DAQ = 1,
	EV_DIR_STIM = 2,
	EV_DIR_BOTH = 3,
} t_event_dir;

typedef struct
{
	t_xcp_time_unit event_time_unit;
	uint32_t event_rate;
	uint8_t priority;
	t_event_dir dir;
} t_xcp_event;

typedef struct
{
	uint32_t mta;
	uint8_t sz;
	uint8_t bit_off;
} t_odt_entry;

typedef enum
{
	DAQ_DIR_DAQ = 1,
	DAQ_DIR_STIM = 2,
	DAQ_DIR_BOTH = 3,
} t_daq_dir;

typedef struct
{
	uint8_t run;
} t_daq_context;

typedef struct
{
	t_odt_entry *odt_entries; //array of ODT entries
	uint16_t total_odt_entries;//max_odt_entry = total_odt_entries / max_odt
	uint8_t max_odt;
	uint8_t en_pid_off; //max_odt assumed '1' if this option enabled
	uint8_t priority;
	uint8_t event_nr;
	uint8_t event_fixed;
	t_daq_dir dir;
	t_daq_context *ctx;
	uint8_t *dto_buf; //at least MAX_DTO bytes
} t_daq_list;


typedef enum
{
	XCP_TIME_STAMP_NONE = 0,
	XCP_TIME_STAMP_BYTE = 1,
	XCP_TIME_STAMP_WORD = 2,
	XCP_TIME_STAMP_DWORD = 4,
} t_time_stamp_size;



typedef struct
{
	uint32_t mta;
	uint32_t session_conf_id;
	uint16_t daq_ptr;
	uint8_t odt_ptr;
	uint8_t odt_entry_ptr;
	uint32_t time_stamp;
} t_xcp_context;

typedef void (*t_xcp_send_cto_cb)(uint8_t *buf, uint32_t len);
typedef void (*t_xcp_send_dto_cb)(uint8_t *buf, uint32_t len, uint16_t daq_nr);

typedef void (*t_xcp_program_start_cb)(void);
typedef uint8_t (*t_xcp_program_clear_cb)(uint32_t mta, uint32_t range);
typedef uint8_t (*t_xcp_program_cb)(uint32_t mta, uint8_t *buf, uint8_t len);
typedef void (*t_xcp_program_reset_cb)(void);

typedef struct
{
	uint8_t max_cto;
	uint16_t max_dto;

	uint8_t en_daq;
	uint8_t en_stim;
	uint8_t en_pgm;
	uint8_t en_cal;
	uint8_t en_blk;
	t_time_stamp_size time_stamp_size;
	t_xcp_time_unit time_stamp_unit;
	uint16_t time_stamp_tick;
	uint8_t time_stamp_fixed;

	uint16_t events_max;
	uint16_t daq_lists_max;
	const t_xcp_event *events;
	const t_daq_list *daq_lists; //array of DAQ list descriptors
	t_xcp_context *ctx;
	uint8_t *cto_resp_buf; //at least MAX_CTO bytes
	t_xcp_send_cto_cb xcp_send_cto_cb;
	t_xcp_send_dto_cb xcp_send_dto_cb;
	t_xcp_program_start_cb program_start_cb;
	t_xcp_program_clear_cb program_clear_cb;
	t_xcp_program_cb program_cb;
	t_xcp_program_reset_cb program_reset_cb;
} t_xcp_client_config;


#define XCP_CONNECT 0xFF
#define XCP_DISCONNECT 0xFE
#define XCP_GET_STATUS 0xFD
#define XCP_SYNCH 0xFC
#define XCP_GET_COMM_MODE_INFO 0xFB
#define XCP_GET_ID 0xFA
#define XCP_SET_REQUEST 0xF9
#define XCP_GET_SEED 0xF8
#define XCP_UNLOCK 0xF7
#define XCP_SET_MTA 0xF6
#define XCP_UPLOAD 0xF5
#define XCP_SHORT_UPLOAD 0xF4
#define XCP_BUILD_CHECKSUM 0xF3
#define XCP_TRANSPORT_LAYER_CMD 0xF2
#define XCP_USER_CMD 0xF1

#define XCP_DOWNLOAD 0xF0
#define XCP_DOWNLOAD_NEXT 0xEF
#define XCP_DOWNLOAD_MAX 0xEE
#define XCP_SHORT_DOWNLOAD 0xED
#define XCP_MODIFY_BITS 0xEC

#define XCP_SET_CAL_PAGE 0xEB
#define XCP_GET_CAL_PAGE 0xEA
#define XCP_GET_PAG_PROCESSOR_INFO 0xE9
#define XCP_GET_SEGMENT_INFO 0xE8
#define XCP_GET_PAGE_INFO 0xE7
#define XCP_SET_SEGMENT_MODE 0xE6
#define XCP_GET_SEGMENT_MODE 0xE5
#define XCP_COPY_CAL_PAGE 0xE4

#define XCP_SET_DAQ_PTR 0xE2
#define XCP_WRITE_DAQ 0xE1
#define XCP_SET_DAQ_LIST_MODE 0xE0
#define XCP_START_STOP_DAQ_LIST 0xDE
#define XCP_START_STOP_SYNCH 0xDD

#define XCP_WRITE_DAQ_MULTIPLE 0xC7
#define XCP_READ_DAQ 0xDB
#define XCP_GET_DAQ_CLOCK 0xDC
#define XCP_GET_DAQ_PROCESSOR_INFO 0xDA
#define XCP_GET_DAQ_RESOLUTION_INFO 0xD9
#define XCP_GET_DAQ_LIST_MODE 0xDF
#define XCP_GET_DAQ_EVENT_INFO 0xD7

#define XCP_CLEAR_DAQ_LIST 0xE3

#define XCP_GET_DAQ_LIST_INFO 0xD8
#define XCP_FREE_DAQ 0xD6
#define XCP_ALLOC_DAQ 0xD5
#define XCP_ALLOC_ODT 0xD4
#define XCP_ALLOC_ODT_ENTRY 0xD3

#define XCP_PROGRAM_START 0xD2
#define XCP_PROGRAM_CLEAR 0xD1
#define XCP_PROGRAM 0xD0
#define XCP_PROGRAM_RESET 0xCF
#define XCP_GET_PGM_PROCESSOR_INFO 0xCE
#define XCP_GET_SECTOR_INFO 0xCD
#define XCP_PROGRAM_PREPARE 0xCC
#define XCP_PROGRAM_FORMAT 0xCB
#define XCP_PROGRAM_NEXT 0xCA
#define XCP_PROGRAM_MAX 0xC9
#define XCP_PROGRAM_VERIFY 0xC8

#define XCP_RESP_POS 0xFF
#define XCP_RESP_NEG 0xFE



#define XCP_ERR_CMD_SYNCH 0x00 
#define XCP_ERR_CMD_BUSY 0x10 
#define XCP_ERR_DAQ_ACTIVE 0x11 
#define XCP_ERR_PGM_ACTIVE 0x12 
#define XCP_ERR_CMD_UNKNOWN 0x20 
#define XCP_ERR_CMD_SYNTAX 0x21 
#define XCP_ERR_OUT_OF_RANGE 0x22 
#define XCP_ERR_WRITE_PROTECTED 0x23 
#define XCP_ERR_ACCESS_DENIED 0x24 
#define XCP_ERR_ACCESS_LOCKED 0x25 
#define XCP_ERR_PAGE_NOT_VALID 0x26 
#define XCP_ERR_MODE_NOT_VALID 0x27 
#define XCP_ERR_SEGMENT_NOT_VALID 0x28 
#define XCP_ERR_SEQUENCE 0x29 
#define XCP_ERR_DAQ_CONFIG 0x2A 
#define XCP_ERR_MEMORY_OVERFLOW 0x30 
#define XCP_ERR_GENERIC 0x31 
#define XCP_ERR_VERIFY 0x32 
#define XCP_ERR_RESOURCE_TEMPORARY_NOT_ACCESSIBLE 0x33 
                  

void xcp_cmd(const t_xcp_client_config *cfg, uint8_t *buf, uint32_t len);
void xcp_daq_event(const t_xcp_client_config *cfg, uint16_t daq_nr);
void xcp_update_time_stamp(const t_xcp_client_config *cfg);
void xcp_init(const t_xcp_client_config *cfg);

extern void *xcp_memcpy(void *dest, const void *src, uint32_t n);

#endif /* XCP_CLIENT_H_ */
