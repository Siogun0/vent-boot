#include <stdlib.h>
#include "xcp_client_can.h"
#include "xcp_client.h"

#include "utils.h"
#include "can_platform.h"

//#include <machine/intrinsics.h>

#define DPIN_EN 0
//#include "dpin.h"

static uint32_t first_mbn = 0, can_bus = 0, first_id = 0;

void xcp_send_cto_can(uint8_t *buf, uint32_t len);

#define MAX_CTO 8
#define MAX_DTO 8

#define CTO_RX_MBN		first_mbn
#define CTO_TX_MBN		(first_mbn + 1)
#define DTO_MBN(n)		(first_mbn + (n) + 2)

#define CTO_RX_ID		first_id
#define CTO_TX_ID		(first_id + 1)
#define DTO_ID(n)		(first_id + (n) + 2)

static uint64_t cto_tx_msg;

static t_xcp_context ctx;

extern void xcp_program_start(void);
extern void xcp_program_reset(void);
extern uint8_t xcp_program_clear(uint32_t mta, uint32_t range);
extern uint8_t xcp_program(uint32_t mta, uint8_t *buf, uint8_t len);

static const t_xcp_client_config xcp_cfg_can =
{
		.max_cto = MAX_CTO,
		.max_dto = MAX_DTO,

		.en_daq = 0,
		.en_stim = 0,
		.en_pgm = 1,
		.en_cal = 0,
		.en_blk = 0,
		.time_stamp_size = XCP_TIME_STAMP_DWORD,
		.time_stamp_unit = XCP_UNIT_10NS,
		.time_stamp_tick = 6144,
		.time_stamp_fixed = 0,

		.events_max = 0,
		.daq_lists_max = 0,
		.events = NULL,
		.daq_lists = NULL,
		.ctx = &ctx,
		.cto_resp_buf = (uint8_t *)&cto_tx_msg,
		.xcp_send_cto_cb = xcp_send_cto_can,
		.xcp_send_dto_cb = NULL,

		.program_start_cb = xcp_program_start,
		.program_clear_cb = xcp_program_clear,
		.program_cb = xcp_program,
		.program_reset_cb = xcp_program_reset,
};

void xcp_send_cto_can(uint8_t *buf __attribute__((unused)), uint32_t len)
{
	can_dyn_xmit_mb(can_bus, CTO_TX_MBN, CTO_TX_ID, len, cto_tx_msg);
}

void xcp_can_poll()
{
	static uint64_t msg;
	extern uint32_t inhibit_autorun; // TODO

	if(can_is_message_arrived(can_bus, CTO_RX_MBN))
	{
		inhibit_autorun = 1;
		msg = can_get_mb_data(can_bus, CTO_RX_MBN);
		xcp_cmd(&xcp_cfg_can, (uint8_t *)&msg, 8);
	}
}

void xcp_can_init(uint32_t bus, uint32_t mbn, uint32_t id)
{
	xcp_init(&xcp_cfg_can);
	first_mbn = mbn;
	can_bus = bus;
	first_id = id;

	can_init_rx_mb(can_bus, CTO_RX_MBN, CTO_RX_ID, 8);//CTO receive
	can_init_tx_mb(can_bus, CTO_TX_MBN, CTO_TX_ID, 8);//CTO transmit
}

