#include "flvparser.h"
#include <bitreader.h>

static uint32_t beBuf2Uint32(uint8_t* p) {
    return ((uint32_t)(p[0])<<24) |  ((uint32_t)(p[1])<<16) |  ((uint32_t)(p[2])<<8) | p[3];
}
static uint32_t beBuf2Uint24(uint8_t* p) {
    return ((uint32_t)(p[0])<<16) | ((uint32_t)(p[1])<<8) | p[2];
}

namespace AVD {
    
    Flv::FileReader::FileReader() { }
    
    Flv::FileReader*  Flv::FileReader::NewFileReader(std::string fname) {
        auto pReader = new FileReader();
        pReader->file.open(fname, std::ios_base::binary);
        pReader->isOpenFileOk = !pReader->file.fail();
        
        pReader->file.seekg(0, std::ios::end);
        pReader->size_ = pReader->file.tellg();
        pReader->file.seekg(0, std::ios::beg);
        fprintf(stderr, "size = %lld\n", pReader->size_);
        return pReader;
    }

    int Flv::FileReader::Read(uint8_t *pBuf, int nBufLen) {
        file.read(reinterpret_cast<char*>(pBuf), nBufLen);
        if (file.eof()) {
            return READ_EOF;
        }
        return file.gcount();
    }
    
    int Flv::FileReader::Seek(int nPos) {
        file.seekg(nPos);
        return OK;
    }
    
    int64_t Flv::FileReader::GetSize() {
        return size_;
    }
    
    Flv::Flv() :
        nRawOffset_(0),
        nMaxHandleSize_(0),
        nLastStep_(0) {
            
    }
    
    std::pair<uint8_t*, int>  Flv::read(Reader* r, int nLen) {
        uint8_t* buf = rawData_.data();
        buf += nRawOffset_;
        if (nRawOffset_ + nLen >= nMaxHandleSize_) {
            return std::make_pair<uint8_t*, int>(nullptr, MAX_HANDLE_SIZE);
        }
        
        auto readLen = r->Read(buf, nLen);
        if (readLen == READ_EOF) {
            return std::make_pair<uint8_t*, int>(nullptr, READ_EOF);
        }
        
        if (readLen != nLen) {
            return std::make_pair<uint8_t*, int>(nullptr, NO_MORE_DATA);
        }
        nRawOffset_ += nLen;
        nLastStep_ = nLen;
        return std::make_pair<uint8_t*, int>(buf, OK);
    }
    
    int Flv::Parse(Reader* r, int nMaxHandleSize) {

        rawData_.resize(nMaxHandleSize);
        nMaxHandleSize_ = nMaxHandleSize;
        
        int ret = parseFlvHeader(r);
        if (ret != OK) {
            return ret;
        }
        ret = parsePreviousTagSize(r, header_.PreviousTagSize);
        if (ret != OK) {
            return ret;
        }
        
        while(ret == OK) {
            ret = parseTag(r);
            if (ret != OK) {
                continue;
            }
        }
        if (ret == READ_EOF || ret == OK) {
            return OK;
        }
        return ret;
    }
    
    int Flv::parseFlvHeader(Reader* r) {
        header_.Pos.nOffset = nRawOffset_;
        auto result = read(r, FlvHeaderLength);
        if (result.second != OK) {
            return result.second;
        }
        uint8_t* buf = result.first;
        
        memcpy(header_.Signature, buf, 3);
        if (memcmp(header_.Signature, "FLV", 3) != 0) {
            return WRONG_SIG;
        }
        header_.Version = buf[3];
        
        header_.TypeFlagsReserved1 = buf[4] & 0xF8;
        header_.TypeFlagsAudio = (buf[4] & 0x04)>2;
        header_.TypeFlagsReserved2 = (buf[4] & 0x02)>1;
        header_.TypeFlagsVideo = buf[4] & 0x01;
        header_.DataOffset = beBuf2Uint32(&buf[5]);

        header_.Pos.nSize = FlvHeaderLength;
        header_.pData = buf;
        
        return OK;
    }
    
    int Flv::parsePreviousTagSize(Reader* r, uint32_t& tagSize) {
        auto result = read(r, 4);
        if (result.second != OK) {
            return result.second;
        }
        uint8_t* lenBuf = result.first;
        
        tagSize = beBuf2Uint32(lenBuf);
        return OK;
    }

    int Flv::parseTag(Reader* r) {
        int bakRawOffset = nRawOffset_;
        
        auto result = read(r, FlvTagHeaderLength);
        if (result.second != OK) {
            return result.second;
        }
        uint8_t* buf = result.first;
        
        FlvTag tag;
        tag.Pos.nOffset = bakRawOffset;
        

        tag.TagType = buf[0];
        tag.DataSize = beBuf2Uint24(&buf[1]);
        tag.Timestamp = ((uint32_t)buf[7])<<24 | beBuf2Uint24(&buf[4]);
        //tag.TimestampExtended = buf[7];
        tag.StreamID = beBuf2Uint24(&buf[8]);
        
        if (tag.DataSize + nRawOffset_ > nMaxHandleSize_) {
            nRawOffset_ -= nLastStep_;
            return MAX_HANDLE_SIZE;
        }
        
        tag.Pos.nSize = FlvTagHeaderLength + tag.DataSize;
        
        result = read(r, tag.DataSize);
        if (result.second != OK) {
            return result.second;
        }
        tag.pData = result.first;
        
        int ret = OK;
        switch(tag.TagType) {
            case TagTypeAudio:
                ret = parseAudio(r, &tag, tag.pData);
                break;
            case TagTypeVideo:
                ret = parseVideo(r, &tag, tag.pData);
                break;
            case TagTypeScript:
                ret = parseScript(r, &tag, tag.pData);
                break;
            default:
                ret = NO_SUCH_TYPE;
        }
        if (ret == OK) {
            ret = parsePreviousTagSize(r, tag.PreviousTagSize);
            if (ret != OK) {
                return ret;
            }
            tags_.push_back(tag);
        }
        return ret;
    }
    
    int Flv::parseAudio(Reader* r, FlvTag* t, uint8_t* buf) {
        AudioTagHeader* a = &t->avHhr.a;
        a->SoundFormat = (buf[0] & 0xF0)>4;
        a->SoundRate = (buf[0] & 0x04)>2;
        a->SoundSize = (buf[0] & 0x02)>1;
        a->SoundType = buf[0] & 0x01;
        return OK;
    }
    
    int Flv::parseVideo(Reader* r, FlvTag* t, uint8_t* buf) {
        VideoTagHeader* v = &t->avHhr.v;
        v->FrameType = (buf[0] & 0xF0)>4;
        v->CodecID = buf[0] & 0x0F;
        
        t->CompositionTime = (beBuf2Uint24(buf+2) + 0xff800000) ^ 0xff800000;
        int64_t dts = t->Timestamp;
        int64_t pts = dts + t->CompositionTime;
        t->Pts = (uint32_t)pts;
        return OK;
    }
    
    int Flv::parseScript(Reader* r, FlvTag* t, uint8_t* buf) {
        t->Pts = 0;
        fprintf(stderr, "not support scirpt now\n");
        return OK;
    }
    
    std::pair<const FlvHeader*, const std::vector<FlvTag> > Flv::GetFlv() {
        return std::make_pair(&header_, tags_);
    }
    
    std::pair<const uint8_t*, int> Flv::GetSps(FlvTag *pTag) {
        if (pTag == nullptr || pTag->TagType != 9 || pTag->pData == nullptr) { //video
            return std::make_pair(nullptr, NOT_VIDEO_SEQ);
        }
        if (pTag->pData[1] != 0) {
            return std::make_pair(nullptr, NOT_VIDEO_SEQ);
        }
        return std::make_pair(nullptr, NOT_VIDEO_SEQ);
        
    }
    
    void Flv::Print() {
        printf("HEADER: sig:%c%c%c versoin:%u vflag:%u aflag:%u offset:%u first plen=%d\n", header_.Signature[0], header_.Signature[1], header_.Signature[2],
               header_.Version, header_.TypeFlagsVideo, header_.TypeFlagsAudio, header_.DataOffset, header_.PreviousTagSize);
        for (int i = 0; i < tags_.size(); i++) {
            printf("type:%-2d pts:%-11d plen:%-8u dsize:%-8u\n", tags_[i].TagType, tags_[i].Timestamp, tags_[i].PreviousTagSize, tags_[i].DataSize);
        }
    }
}


