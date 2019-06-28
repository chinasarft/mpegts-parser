#ifndef __AVCDecoderConfigurationRecord_H__
#define __AVCDecoderConfigurationRecord_H__

#include <cstdint>
#include <vector>
#include <memory>

namespace AVD {

    struct AVCSequenceParameterSet {
        uint16_t sequenceParameterSetLength;
        uint8_t* sequenceParameterSetNALUnit;
    };
    struct AVCPictureParameterSet {
        uint16_t pictureParameterSetLength;
        uint8_t* pictureParameterSetNALUnit;
    };
    /*
    aligned(8) class AVCDecoderConfigurationRecord {
        unsigned int(8) configurationVersion = 1; 
        unsigned int(8) AVCProfileIndication;
        unsigned int(8) profile_compatibility;
        unsigned int(8) AVCLevelIndication;
        bit(6) reserved = ‘111111’b;
        unsigned int(2) lengthSizeMinusOne;
        bit(3) reserved = ‘111’b;
        unsigned int(5) numOfSequenceParameterSets;
        for (i=0; i< numOfSequenceParameterSets; i++) {
            unsigned int(16) sequenceParameterSetLength ;
            bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
        }
        unsigned int(8) numOfPictureParameterSets;
        for (i=0; i< numOfPictureParameterSets; i++) {
            unsigned int(16) pictureParameterSetLength;
            bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
        }
    }
    */
    struct AVCDecoderConfigurationRecord {
        uint8_t configurationVersion;
        uint8_t AVCProfileIndication;
        uint8_t profile_compatibility;
        uint8_t AVCLevelIndication;
        uint8_t reserved1:6;
        uint8_t lengthSizeMinusOne:2;
        uint8_t reserved2:3;
        uint8_t numOfSequenceParameterSets:5;
        std::vector<AVCSequenceParameterSet> sps;
        uint8_t numOfPictureParameterSets;
        std::vector<AVCPictureParameterSet> pps;
    };
    
    std::pair<std::shared_ptr<AVCDecoderConfigurationRecord>, int> ParseAVCDecoderConfigurationRecord(uint8_t* pData, int nDataLen);
}

#endif
