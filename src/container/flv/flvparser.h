#ifndef __FLV_PARSE_H__
#define __FLV_PARSE_H__

#include "flv_struct.h"
#include <vector>
#include <memory>
#include <fstream>

namespace AVD {
    class Flv {
    public:
        
        class Reader {
        public:
            virtual int Read(uint8_t *pBuf, int nBufLen) = 0;
        };
        
        class FileReader : public Reader {
        public:
            int Read(uint8_t *pBuf, int nBufLen) override;
            int Seek(int nPos);
            bool IsOk() {return isOpenFileOk;}
            int64_t GetSize();
            static Flv::FileReader* NewFileReader(std::string fname);
            
        private:
            FileReader();
            bool isOpenFileOk = false;
            std::ifstream file;
            int64_t size_;
        };
        
        static const int OK = 0;
        static const int MAX_HANDLE_SIZE = 1;
        static const int NO_MORE_DATA = -100;
        static const int WRONG_SIG = -101;
        static const int NO_SUCH_TYPE = -102;
        static const int READ_EOF = -103;
        static const int NOT_VIDEO_SEQ = -104;
        
        static const int FlvHeaderLength = 9;
        static const int FlvTagHeaderLength = 11;
        
        static const int TagTypeVideo = 9;
        static const int TagTypeAudio = 8;
        static const int TagTypeScript = 18;
        
        static const int SoundFormatAlaw = 7;
        static const int SoundFormatMulaw = 8;
        static const int SoundFormatAac = 10;
        static const int SoundFormatSpeex = 11;
        static const int SoundFormatOpus = 15; // flv有定义了(Device-specific sound), 但是自定义一下，后续可能用到
        
        static const int VideoCodecIDAVC = 7;
        static const int VideoCodecIDHEVC = 12; // hevc扩展
        static const int VideoCodecIDAV1 = 13; // 自定义扩展
        static const int VideoCodecIDVP8 = 14; // 自定义扩展
        static const int VideoCodecIDVP9 = 15; // 自定义扩展
        
    public:
        Flv();
        
        
        int Parse(Reader* pReader, int nMaxHandleSize=30*1024*1024);
        void Print();
        std::pair<const FlvHeader*, const std::vector<FlvTag> > GetFlv();
        std::pair<const uint8_t*, int>GetSps(FlvTag *pTag);
        
    private:
        std::pair<uint8_t*, int> read(Reader* r, int nLen);
        int parseFlvHeader(Reader* pReader);
        int parsePreviousTagSize(Reader* pReader, uint32_t& tagSize);
        int parseTag(Reader* pReader);
        int parseAudio(Reader* pReader, FlvTag* t, uint8_t* buf);
        int parseVideo(Reader* pReader, FlvTag* t, uint8_t* buf);
        int parseScript(Reader* pReader, FlvTag* t, uint8_t* buf);
        
    private:
        std::vector<uint8_t> rawData_;
        int nRawOffset_;
        int nMaxHandleSize_;
        int nLastStep_;
        
        //static Position GetTag
        FlvHeader header_;
        std::vector<FlvTag> tags_;
    };

}

#endif
