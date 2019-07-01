
#ifndef __AudioInfo_H__
#define __AudioInfo_H__

#include <cstdint>
#include <memory>

namespace AVD {
    class AudioSpecificConfig {
    public:
        static const int OK = 0;
        static const int WRONG_DATA = -1300;
        
    public:
        const char* GetAudioTypeName();
        int GetSamplingFrequency();
        int GetChannels();
        
    public:
        uint32_t audioObjectType;
        uint32_t samplingFrequencyIndex;
        uint32_t samplingFrequency;
        uint32_t channelConfiguration;
    };
    
    std::pair<std::shared_ptr<AudioSpecificConfig>, int> ParseAudioSpecificConfig(uint8_t* pData, int nDataLen);
}

#endif
