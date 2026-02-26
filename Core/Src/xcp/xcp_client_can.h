#ifndef XCP_CLIENT_CAN_H_
#define XCP_CLIENT_CAN_H_

#include <stdint.h>
#include <string.h>
//#include "can_plus/can_plus.h"

void xcp_can_init(uint32_t bus, uint32_t mbn, uint32_t id);
void xcp_can_poll(void);

#endif /* XCP_CLIENT_UDP_H_ */
