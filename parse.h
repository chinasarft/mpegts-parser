#include "table.h"
#define TS_PACKET_SIZE 188
void ts_init(MpegTs *pTs);
int ts_parse_buffer(MpegTs *pTs, uint8_t* pData, int nDataLen, TsParsedFrame pFrames[2]);

// 最后一个pes可能由于没有长度信息，需要调用该函数才能获取
int ts_flush(MpegTs *pTs, TsParsedFrame* pFrame);

void ts_clean(MpegTs *pTs);
