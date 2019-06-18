#include <stdint.h>
#define TS_MAX_PROGRAM_MAP 10
#define TS_MAX_STREAM_DESCRIPTOR 16
#define TS_MAX_STREAMS 8
#define TS_MAX_PMT 4
#define TS_PACKET_SIZE 188

#if defined(__cplusplus) || defined(c_plusplus)
extern "C"{
#endif

typedef enum {
    TsTypeH264 = 0x1b,
    TsTypeH265 = 0x24,
    TsTypeAAC = 0x0f,
    TsTypePrivate = 0x06
}TsStreamType;

typedef struct {
    int64_t nPts;
    uint16_t PID;
    const uint8_t* pData;
    int nDataLen;
    TsStreamType stype;
}TsParsedFrame;


typedef struct TsHeader {
    uint8_t sync_byte;
    uint8_t transport_error_indicator;
    uint8_t payload_unit_start_indicator;
    uint8_t transport_priority;
    uint16_t PID;
    uint8_t transport_scrambling_control;
    uint8_t adaptation_field_control;
    uint8_t continuity_counter;
}TsHeader;

// 暂时只需要这两个值
typedef struct TsAdaptationField {
    uint8_t adaptation_field_length;
    uint8_t PCR_flag;
    uint64_t pcr;
}TsAdaptationField;

// 未遇到过pointer filed 有内容的，所以暂时先这样定义
typedef struct PointerField {
    uint8_t is_exist;
    uint8_t len;
}PointerField;

typedef struct PesHeader {
    uint32_t packet_start_code_prefix;
    uint8_t stream_id;
    uint16_t PES_packet_length;
    uint8_t PTS_DTS_flags;
    uint64_t pts;
}PesHeader;

typedef struct Pes {
    TsHeader hdr;
    PesHeader pesHdr;
    TsAdaptationField adpt_fld;
    uint8_t isParseStart;
    uint32_t parsedLength;
    int pesHdrLen;
    uint8_t* pData;
    int nDataLen;
    int nDataCap;
}Pes;


typedef struct PmtMap {
    uint16_t program_number;
    uint16_t program_map_PID;
}PmtMap;

typedef struct TsPMTStreamInfo {
    uint8_t stream_type;
    uint8_t reserved1;
    uint16_t elementary_PID;
    uint8_t reserved2;
    uint16_t ES_info_length;
    uint8_t descriptor[TS_MAX_STREAM_DESCRIPTOR]; //实际长度ES_info_length，可能比这个大，现在只支持16个字节
}TsPMTStreamInfo;

typedef struct TsPMT {
    TsHeader hdr;
    PointerField poiter_field;
    TsAdaptationField adpt_fld;
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint8_t must_zero;
    uint8_t reserved1;
    uint16_t section_length;
    uint16_t program_number;
    uint8_t reserved2;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    uint8_t reserved3;
    uint16_t PCR_PID;
    uint8_t reserved4;
    uint16_t program_info_length;
    TsPMTStreamInfo streamInfo[TS_MAX_STREAMS];
    int streamInfoCount;
}TsPMT;

typedef struct TsPAT {
    TsHeader hdr;
    PointerField poiter_field;
    TsAdaptationField adpt_fld;
    uint8_t table_id;
    uint8_t section_syntax_indicator;
    uint8_t must_zero;
    uint8_t reserved1;
    uint16_t section_length;
    uint16_t transport_stream_id;
    uint8_t reserved2;
    uint8_t version_number;
    uint8_t current_next_indicator;
    uint8_t section_number;
    uint8_t last_section_number;
    PmtMap map[TS_MAX_PROGRAM_MAP];
}TsPAT;

typedef struct MpegTs {
    int isParsedPat;
    TsPAT pat;
    TsPMT pmt[TS_MAX_PMT];
    Pes pes[TS_MAX_STREAMS];
    int lastParsedPesIdx;
}MpegTs;

void ts_init(MpegTs *pTs);
int ts_parse_buffer(MpegTs *pTs, uint8_t* pData, int nDataLen, TsParsedFrame pFrames[2]);

// 最后一个pes可能由于没有长度信息，需要调用该函数才能获取
int ts_flush(MpegTs *pTs, TsParsedFrame* pFrame);

void ts_clean(MpegTs *pTs);
    
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
