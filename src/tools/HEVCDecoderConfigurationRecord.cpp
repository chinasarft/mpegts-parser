#include "HEVCDecoderConfigurationRecord.h"
//#include "bitreader.h" //TODO 后续还是使用bitreader，简单

static uint32_t beBuf2Uint32(uint8_t* p) {
    return ((uint32_t)(p[0])<<24) |  ((uint32_t)(p[1])<<16) |  ((uint32_t)(p[2])<<8) | p[3];
}
static uint64_t beBuf2Uint48(uint8_t* p) {
    return ((uint64_t)(p[0])<<40) |  ((uint64_t)(p[1])<<32) |  ((uint64_t)(p[2])<<24) |
        ((uint64_t)(p[3])<<16) |  ((uint64_t)(p[4])<<8) | p[5];
}

namespace AVD {
    std::pair<std::shared_ptr<HEVCDecoderConfigurationRecord>, int> ParseHEVCDecoderConfigurationRecord(uint8_t* pData, int nDataLen) {
        
        std::shared_ptr<HEVCDecoderConfigurationRecord> r = std::make_shared<HEVCDecoderConfigurationRecord>();
        
        int offset = 0;
        r->configurationVersion = pData[offset++];
        
        r->general_profile_space = (pData[offset] & 0xC0)>>6;
        r->general_tier_flag = (pData[offset] & 0x20)>>5;;
        r->general_profile_idc = pData[offset] & 0x1F;
        offset++;
        
        r->general_profile_compatibility_flags = beBuf2Uint32(&pData[offset]);
        offset+=4;
        r->general_constraint_indicator_flags = beBuf2Uint48(&pData[offset]);
        offset+=6;
        r->general_level_idc = pData[offset++];
        
        r->reserved1 = (pData[offset] & 0xF0)>>4;
        r->min_spatial_segmentation_idc = ((uint16_t)(pData[offset]&0x0F))<<8 | pData[offset+1];
        offset+=2;
        
        r->reserved2 = (pData[offset] & 0xFC)>>2;
        r->parallelismType = pData[offset] & 0x03;
        offset++;
        
        r->reserved3 = (pData[offset] & 0xFC)>>2;
        r->chromaFormat = pData[offset] & 0x03;
        offset++;
        
        r->reserved4 = (pData[offset] & 0xF8)>>3;
        r->bitDepthLumaMinus8 = pData[offset] & 0x07;
        offset++;
        
        r->reserved5 = (pData[offset] & 0xF8)>>3;
        r->bitDepthChromaMinus8 = pData[offset] & 0x07;
        offset++;
        
        r->avgFrameRate = pData[offset] * (uint16_t)256 + pData[offset+1];
        offset+=2;
        
        r->constantFrameRate = (pData[offset] | 0xC0)>>6;
        r->numTemporalLayers = (pData[offset] | 0x38)>>3;
        r->temporalIdNested =  (pData[offset] | 0x04)>>2;
        r->lengthSizeMinusOne = pData[offset] | 0x03;
        offset++;
        
        r->numOfArrays = pData[offset++];
        
        for(int i = 0; i < r->numOfArrays; i++) {
            HEVCMetaArray a;
            a.array_completeness = (pData[offset]&0x80)>>7;
            a.reserved = (pData[offset]&0x40)>>6;
            a.NAL_unit_type = pData[offset]&0x3F;
            offset++;
            a.numNalus = pData[offset] * 256 + pData[offset+1];
            offset+=2;
            
            a.unit.nalUnitLength = pData[offset] * 256 + pData[offset+1];
            offset+=2;
            a.unit.nalUnit = &pData[offset];
            offset += a.unit.nalUnitLength;
            
            r->arrays.push_back(a);
        }
        
        if (offset != nDataLen) {
            r.reset();
            return std::make_pair<std::shared_ptr<HEVCDecoderConfigurationRecord>, int>(r, -1);
        }
        return std::make_pair<std::shared_ptr<HEVCDecoderConfigurationRecord>, int>(r, 0);
    }
}
