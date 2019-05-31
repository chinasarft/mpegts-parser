#include "table.h"
#define TS_PACKET_SIZE 188

int ts_parse_buffer(MpegTs *pTs, uint8_t* pData, int nDataLen);
