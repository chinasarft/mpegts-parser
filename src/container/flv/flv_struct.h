#ifndef __FLV_STRUCT_H__
#define __FLV_STRUCT_H__

#include <cstdint>

namespace AVD {
    
    struct FlvPosition {
        int nOffset;
        int nSize;
    };

    struct FlvHeader {
        uint8_t Signature[3]; // must be ['F', 'L', 'V']
        uint8_t Version; // File version (for example, 0x01 for FLV version 1)
        uint8_t TypeFlagsReserved1:5; // Shall be 0
        uint8_t TypeFlagsAudio:1; // 1 = Audio tags are present
        uint8_t TypeFlagsReserved2:1; // Shall be 0
        uint8_t TypeFlagsVideo:1; // 1 = Video tags are present
        uint32_t DataOffset; //The length of this header in bytes
        uint32_t PreviousTagSize;
        uint8_t* pData;
        FlvPosition Pos;
    };

    struct AudioTagHeader {
        /*
        Format of SoundData. The following values are defined: 0 = Linear PCM, platform endian
        1 = ADPCM
        2 = MP3
        3 = Linear PCM, little endian 4 = Nellymoser 16 kHz mono 5 = Nellymoser 8 kHz mono 6 = Nellymoser
        7 = G.711 A-law logarithmic PCM
        8 = G.711 mu-law logarithmic PCM
        9 = reserved
        10 = AAC
        11 = Speex
        14 = MP3 8 kHz
        15 = Device-specific sound
        Formats 7, 8, 14, and 15 are reserved.
        AAC is supported in Flash Player 9,0,115,0 and higher. Speex is supported in Flash Player 10 and higher.
        */
        uint8_t SoundFormat:4;
        uint8_t SoundRate:2; // 0 = 5.5 kHz 1 = 11 kHz 2 = 22 kHz 3 = 44 kHz
                             // for aac alwats equal to 3
        /*
        Size of each audio sample. This parameter only pertains to uncompressed formats. Compressed formats always decode to 16 bits internally.
        0 = 8-bit samples
        1 = 16-bit samples
        */
        uint8_t SoundSize:1;
        uint8_t SoundType:1; //0 = Mono sound 1 = Stereo sound
    };

    struct VideoTagHeader {
        /*
        Type of video frame. The following values are defined: 1 = key frame (for AVC, a seekable frame)
        2 = inter frame (for AVC, a non-seekable frame)
        3 = disposable inter frame (H.263 only)
        4 = generated key frame (reserved for server use only) 5 = video info/command frame
        */
        uint8_t FrameType:4;
        /*
        Codec Identifier. The following values are defined: 2 = Sorenson H.263
        3 = Screen video
        4 = On2 VP6
        5 = On2 VP6 with alpha channel 6 = Screen video version 2
        7 = AVC
        0x0c(12) == HEVC 非标准
        */
        uint8_t CodecID:4;
    };

    struct FlvTag {
        uint8_t TagType; //8 = audio 9 = video 18 = script data
        uint32_t DataSize; // Length of the message. Number of bytes after StreamID to end of tag (Equal to length of the tag – 11)
        uint32_t Timestamp; // include TimestampExtended
                            // Time in milliseconds at which the data in this tag applies.
                            // This value is relative to the first tag in the FLV file, which always has a timestamp of 0
        //uint32_t TimestampExtended:8; // Extension of the Timestamp field to form a SI32 value.
                                   // This field represents the upper 8 bits, 
                                   // while the previous Timestamp field represents the lower 24 bits of the time in milliseconds
        int32_t CompositionTime;
        uint32_t Dts;
        uint32_t StreamID; // Always 0.
        // EncryptionHeader // if  Filter == 1
        // FilterParams  // if  Filter == 1
        union {
            AudioTagHeader a;
            VideoTagHeader v;
        }avHhr;
        uint8_t* pData; // if is a/v data according to AACPacketType and AVCPacketType, maybe data, or sequence config
                        // 包括了一个字节的AudioTagHeader或者VideoTagHeader
                        // aac avc等格式还有一些固定头没有固定解析, 打算是选中后在解析
        uint32_t PreviousTagSize;
        FlvPosition Pos;
    };

}

#endif
