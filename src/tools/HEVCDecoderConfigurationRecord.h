#ifndef __HEVCDecoderConfigurationRecord_H__
#define __HEVCDecoderConfigurationRecord_H__

#include <cstdint>
#include <vector>
#include <memory>

namespace AVD {

    struct HEVCDecoderConfigurationRecordNalUnit {
        uint16_t nalUnitLength;
        uint8_t* nalUnit;
    };

    struct HEVCMetaArray {
            uint8_t array_completeness:1;
            uint8_t  reserved:1; // = 0 
            uint8_t NAL_unit_type:6;
            uint16_t numNalus;
            HEVCDecoderConfigurationRecordNalUnit unit;
    };

    struct HEVCDecoderConfigurationRecord {
        /*
        unsigned int(8)  configurationVersion;
        unsigned int(2)  general_profile_space;
        unsigned int(1)  general_tier_flag;
        unsigned int(5)  general_profile_idc;
        unsigned int(32) general_profile_compatibility_flags;
        unsigned int(48) general_constraint_indicator_flags;
        unsigned int(8)  general_level_idc;
        bit(4) reserved = ‘1111’b;
        unsigned int(12) min_spatial_segmentation_idc;
        bit(6) reserved = ‘111111’b;
        unsigned int(2)  parallelismType;
        bit(6) reserved = ‘111111’b;
        unsigned int(2)  chromaFormat;
        bit(5) reserved = ‘11111’b;
        unsigned int(3)  bitDepthLumaMinus8;
        bit(5) reserved = ‘11111’b;
        unsigned int(3)  bitDepthChromaMinus8;
        bit(16) avgFrameRate;
        bit(2)  constantFrameRate;
        bit(3)  numTemporalLayers;
        bit(1)  temporalIdNested;
        unsigned int(2) lengthSizeMinusOne;
        unsigned int(8) numOfArrays;
        for (j=0; j < numOfArrays; j++) {
            bit(1) array_completeness;
            unsigned int(1)  reserved = 0; 
            unsigned int(6)  NAL_unit_type;
            unsigned int(16) numNalus;
            for (i=0; i< numNalus; i++) {
                unsigned int(16) nalUnitLength;
                bit(8*nalUnitLength) nalUnit;
            }
        }
        */

        uint8_t configurationVersion;
        uint8_t  general_profile_space:2;
        uint8_t  general_tier_flag:1;
        uint8_t  general_profile_idc:5;

        uint32_t general_profile_compatibility_flags;
        uint64_t general_constraint_indicator_flags; // 48bit
        uint8_t  general_level_idc;

        uint16_t reserved1:4;
        uint16_t min_spatial_segmentation_idc:12;
        uint8_t reserved2:6;
        uint8_t parallelismType:2;
        uint8_t reserved3:6;
        uint8_t chromaFormat:2;
        uint8_t reserved4:5;
        uint8_t bitDepthLumaMinus8:3;
        uint8_t reserved5:5;
        uint8_t bitDepthChromaMinus8:3;

        uint16_t avgFrameRate;
        uint8_t constantFrameRate:2;
        uint8_t numTemporalLayers:3;
        uint8_t temporalIdNested:1;
        uint8_t lengthSizeMinusOne:2;
        uint8_t numOfArrays;
        std::vector<HEVCMetaArray> arrays;
    };
    
    std::pair<std::shared_ptr<HEVCDecoderConfigurationRecord>, int> ParseHEVCDecoderConfigurationRecord(uint8_t* pData, int nDataLen);
}

#endif
