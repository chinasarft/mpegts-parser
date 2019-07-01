#include "audioinfo.h"
#include "bitreader.h"

namespace AVD {
    static int freqs[] = {
        96000, 88200, 64000, 48000, 44100, 32000,
        24000, 22050, 16000, 12000, 11025, 8000,
        7350, 0, 0, 0
    };
    static const char *audioTypes[] = {
        "AAC MAIN", "AAC LC", "AAC SSR", "AAC LTP", "SBR", "AAC scalable",
        "TwinVQ", "CELP", "HVXC", "reserved", "reserved", "TTSI", "Main synthetic",
        "Wavetabel synthetic", "General MIDI", "Algorithmic Synthesis and Audio FX"
        //TODO 补充完整
    };
    std::pair<std::shared_ptr<AudioSpecificConfig>, int> ParseAudioSpecificConfig(uint8_t* pData, int nDataLen) {
        std::shared_ptr<AudioSpecificConfig> c = std::make_shared<AudioSpecificConfig>();
        
        BitReader br;
        InitBitReader(&br, pData, nDataLen);
        c->audioObjectType = GetBits(&br, 5);
        
        c->samplingFrequencyIndex = GetBits(&br, 4);
        if (c->samplingFrequencyIndex == 0x0f) {
            c->samplingFrequency =  GetBits(&br, 24);
        }
        c->channelConfiguration = GetBits(&br, 4);
        
        if (IsOverFlow(&br)) {
            return std::make_pair<std::shared_ptr<AudioSpecificConfig>, int>(nullptr, AudioSpecificConfig::WRONG_DATA);
        }
        return std::make_pair<std::shared_ptr<AudioSpecificConfig>, int>(c, AudioSpecificConfig::OK);
    }
    
    const char* AudioSpecificConfig::GetAudioTypeName() {
        return audioTypes[audioObjectType];
    }
    
    int AudioSpecificConfig::GetSamplingFrequency() {
        if (samplingFrequencyIndex == 0xf) {
            return (int)samplingFrequency;
        }
        return freqs[samplingFrequencyIndex];
    }
    
    int AudioSpecificConfig::GetChannels() {
        return (int)channelConfiguration;
    }
}
