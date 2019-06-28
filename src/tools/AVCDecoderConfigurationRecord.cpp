#include <AVCDecoderConfigurationRecord.h>

namespace AVD {
    std::pair<std::shared_ptr<AVCDecoderConfigurationRecord>, int> ParseAVCDecoderConfigurationRecord(uint8_t* pData, int nDataLen) {
        
        std::shared_ptr<AVCDecoderConfigurationRecord> r = std::make_shared<AVCDecoderConfigurationRecord>();
        
        r->configurationVersion = pData[0];
        r->AVCProfileIndication = pData[1];
        r->profile_compatibility = pData[2];
        r->AVCLevelIndication = pData[3];
        r->reserved1 = pData[4]>>2;
        r->lengthSizeMinusOne = pData[4]&0xFC;
        r->reserved2 = pData[5]>>5;
        r->numOfSequenceParameterSets = pData[5]&0x1F;
        int offset = 6;
        for (int i = 0; i < r->numOfSequenceParameterSets; i++) {
            AVCSequenceParameterSet sps;
            sps.sequenceParameterSetLength =  ((uint16_t)pData[offset]) * 256 + pData[offset+1];
            offset += 2;
            sps.sequenceParameterSetNALUnit = pData+offset;
            offset += sps.sequenceParameterSetLength;
            r->sps.push_back(sps);
        }
        
        r->numOfPictureParameterSets = pData[offset++];
        for (int i = 0; i < r->numOfSequenceParameterSets; i++) {
            AVCPictureParameterSet pps;
            pps.pictureParameterSetLength =  ((uint16_t)pData[offset]) * 256 + pData[offset+1];
            offset += 2;
            pps.pictureParameterSetNALUnit = pData+offset;
            offset += pps.pictureParameterSetLength;
            r->pps.push_back(pps);
        }
        
        if (offset != nDataLen) {
            r.reset();
            return std::make_pair<std::shared_ptr<AVCDecoderConfigurationRecord>, int>(r, -1);
        }
        return std::make_pair<std::shared_ptr<AVCDecoderConfigurationRecord>, int>(r, 0);
    }
    
}
