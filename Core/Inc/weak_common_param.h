#ifndef WEAK_COMMON_PARAM_H
#define WEAK_COMMON_PARAM_H

#ifndef COMMON_PARAM_H

#define COMMON_PARAM_SIZE (64)
#define COMMON_PARAM_FREE (COMMON_PARAM_SIZE - 32 - 5)

typedef struct
{
   uint32_t crc;
   uint32_t size;    // COMMON_PARAM_SIZE
   uint32_t xcp_canid_rx;
   uint32_t xcp_canid_tx;      // Or next and possible DAQ
   uint32_t uds_canid_rx;
   uint32_t uds_canid_func;
   uint32_t uds_canid_tx;
   uint8_t  ip_mac[6];
   uint8_t  ip_v4[4];
   uint16_t ip_port_xcp;
   uint8_t  id;

   uint8_t reserved[COMMON_PARAM_FREE];
}common_param_t;

#endif /*COMMON_PARAM_H*/

#endif /*WEAK_COMMON_PARAM_H*/
