#include "mpegts_struct.h"
#include <bitreader.h>

namespace AVD {
    int TsHeader::Parse(uint8_t *pData, size_t nDataLen) {
        int nHdrLen = 4;
        if (nDataLen < nHdrLen) {
            return ErrorCode::NOEOUNGH_DATA;
        }

        BitReader bitReader;
        InitBitReader(&bitReader, pData, nHdrLen);
        BitReader* pBitReader = &bitReader;

        this->sync_byte = (uint8_t)GetBits(pBitReader, 8);
        if(this->sync_byte != 47) {
            return ErrorCode::WRONG_SYNC_BYTE;
        }
        this->transport_error_indicator = (uint8_t)GetBits(pBitReader, 1);
        this->payload_unit_start_indicator = (uint8_t)GetBits(pBitReader, 1);
        this->transport_priority = (uint8_t)GetBits(pBitReader, 1);
        this->PID = (uint16_t)GetBits(pBitReader, 13);
        this->transport_scrambling_control = (uint8_t)GetBits(pBitReader, 2);
        this->adaptation_field_control = (uint8_t)GetBits(pBitReader, 2);
        this->continuity_counter = (uint8_t)GetBits(pBitReader, 4);

        return ErrorCode::SUCCESS;
    }

    int PointerField::Parse(uint8_t *pData, size_t nDataLen) {
        is_exist = 1;
        return pData[0]+1;
    }


    int MpegTs::Parse(Reader* r) {
        TsHeader hdr;
        int ret, nParsedLen = 0;
        if ( (ret = hdr.Parse(r)) != ErrorCode::SUCCESS) {
            return ret;
        }
        nParsedLen += ret;




        return ErrorCode::SUCCESS;
    }
}