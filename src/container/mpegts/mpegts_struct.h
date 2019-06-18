#ifndef __MPEGTS_STRUCT__
#define __MPEGTS_STRUCT__

#include <vector>
#include <memory>
#include <cstdint>
#include <map>

namespace AVD {
    #define TS_PACKET_SIZE 188
    class ErrorCode {
        public:
        static int const SUCCESS = 0;
        static int const UNKNOWN_SIZE = -2;
        static int const NOEOUNGH_DATA = -100;
        static int const WRONG_SYNC_BYTE = -101; // first byte not 47
    };

    class Reader {
    public:
        virtual size_t Read(uint8_t *pBuf, size_t nBufLen) = 0;
        virtual size_t GetSize() = 0; // may return UNKNOWN_SIZE
        virtual int ReadAll(uint8_t** p) = 0;
    };

    class Parser {
    public:
            /**
             * @return > 0: 解析的数据的长度
             *         <=0 错误
             **/
        virtual int Parse(uint8_t *pData, size_t nDataLen) = 0;
    };

    class TsHeader : public Parser {
    public:
            uint32_t sync_byte                    :8;      //同步字节，固定为0x47 ，表示后面的是一个TS分组，当然，后面包中的数据是不会出现0x47的
            uint32_t transport_error_indicator       :1;      //传输错误标志位，一般传输错误的话就不会处理这个包了
            uint32_t payload_unit_start_indicator    :1;      //有效负载的开始标志，根据后面有效负载的内容不同功能也不同， 0x01表示含有PSI或者PES头
            // payload_unit_start_indicator为1时，在前4个字节之后会有一个调整字节，它的数值决定了负载内容的具体开始位置。
            uint32_t transport_priority              :1;      //传输优先级位，1表示高优先级
            uint32_t PID                          :13;     //有效负载数据的类型，0x0表示后面负载内容为PAT，不同的PID表示不同的负载
            uint32_t transport_scrambling_control     :2;      //加密标志位,00表示未加密
            uint32_t adaptation_field_control          :2;      //调整字段控制,。01仅含有效负载，10仅含调整字段，11含有调整字段和有效负载。为00的话解码器不进行处理。
            uint32_t continuity_counter              :4;      //一个4bit的计数器，范围0-15，据计数器读数，接收端可判断是否有包丢失及包传送顺序错误。显然，包头对TS包具有同步、识别、检错及加密功能
    public:
            int Parse(uint8_t *pData, size_t nDataLen) override;
    };

    // 未遇到过pointer filed 有内容的，所以暂时先这样定义; 如果存在offset默认就是5
    class PointerField {
    public:
        int Parse(uint8_t *pData, size_t nDataLen) override;

    public:
        uint8_t is_exist;
        uint8_t len;
    };

    class TsAdaptationField : public Parser
    {
    public:
        uint64_t adaptation_field_length :8;
        //if(adaptation_field_length >0)
        uint64_t discontinuity_indicator :1;
        uint64_t random_access_indicator:1;
        uint64_t elementary_stream_priority_indicator:1;
        uint64_t PCR_flag:1;
        uint64_t OPCR_flag:1;
        uint64_t splicing_point_flag:1;
        uint64_t transport_private_data_flag:1;
        uint64_t adaptation_field_extension_flag:1;

        //if(PCR_flag == '1')
        uint64_t program_clock_reference_base:33;
        uint64_t reserved_1:6;
        uint64_t program_clock_reference_extension:9;


        //if(OPCR_flag == '1')
        uint64_t original_program_clock_reference_base:33;
        uint64_t reserved_2:6;
        uint64_t original_program_clock_reference_extension:9;
        //if (splicing_point_flag == '1')
        uint64_t splice_countdown:8;
        //if(transport_private_data_flag == '1')
        uint64_t transport_private_data_length:8; //立即跟随transport_private_data_length长度的data
        std::vector<char> private_data_byte;

        //if (adaptation_field_extension_flag == '1' )
        unsigned adaptation_field_extension_length:8;
        unsigned ltw_flag:1;
        unsigned piecewise_rate_flag:1;
        unsigned seamless_splice_flag:1;
        unsigned reserved_3:5;
        //if (ltw_flag == '1')
        unsigned ltw_valid_flag:1;
        unsigned ltw_offset:15;
        //if (piecewise_rate_flag == '1')
        unsigned reserved_4:2;
        unsigned piecewise_rate:22;
        //if (seamless_splice_flag == '1')
        unsigned splice_type:4;
        unsigned DTS_next_AU_1:3;
        unsigned marker_bit_1:1;
        unsigned DTS_next_AU_2:15;
        unsigned marker_bit_2:1;
        unsigned DTS_next_AU_3:15;
        unsigned marker_bit_3:1;
        unsigned reserved:8;
        unsigned stuffing_byte:8;
    };

    class TsPATMapEntry {
    public:
        unsigned program_number                  : 16;
        unsigned reserved_3                      : 3;
        unsigned network_or_program_map_PID      : 13;  // if(program_number == '0') network_PID
    };
    // Program Association Table
    class TsPAT : public Parser
    {
    public:
        unsigned table_id                        : 8; //固定为0x00 ，标志是该表是PAT
        unsigned section_syntax_indicator        : 1; //段语法标志位，固定为1
        unsigned zero                            : 1; //0
        unsigned reserved_1                      : 2; // 保留位
        unsigned section_length                  : 12;//表示这个字节后面有用的字节数，包括CRC32
        unsigned transport_stream_id             : 16;//该传输流的ID，区别于一个网络中其它多路复用的流
        unsigned reserved_2                      : 2; // 保留位
        unsigned version_number                  : 5; //范围0-31，表示PAT的版本号
        unsigned current_next_indicator          : 1; //发送的PAT是当前有效还是下一个PAT有效
        unsigned section_number                  : 8; //分段的号码。PAT可能分为多段传输，第一段为00，以后每个分段加1，最多可能有256个分段
        unsigned last_section_number             : 8; //最后一个分段的号码
        unsigned CRC_32                          : 32;
        std::vector<TsPATMapEntry> program_number_map_entries;

    public:
        /**
         * @pTSBuf: ts原始数据
         * @pPat:
         * return > 0 success. 表示解析的数据长度
         *        <= 0失败
         **/
        int Parse(unsigned char *pTSData);
    };

    class TsPMTMapEntry {
    public:
        unsigned stream_type                     : 8;
        unsigned reserved_5                      : 3;
        unsigned elementary_PID                  : 13;
        unsigned reserved_6                      : 4;
        unsigned ES_info_length                  : 12;
    };
    // Program Map Table
    class TsPMT : public Parser
    {
    public:
        unsigned table_id                        : 8;
        unsigned section_syntax_indicator        : 1;
        unsigned zero                            : 1;
        unsigned reserved_1                      : 2;
        unsigned section_length                  : 12;
        unsigned program_number                  : 16;
        unsigned reserved_2                      : 2;
        unsigned version_number                  : 5;
        unsigned current_next_indicator          : 1;
        unsigned section_number                  : 8;
        unsigned last_section_number             : 8;
        unsigned reserved_3                      : 3;
        unsigned PCR_PID                         : 13;
        unsigned reserved_4                      : 4;
        unsigned program_info_length             : 12;
        unsigned CRC_32                          : 32;
        std::vector<TsPMTMapEntry> stream_id_map_entries;
    public:
        int Parse(unsigned char *pTSData);
    } ;

    class TsPacket : public Parser
    {
    public:
        TsHeader header;
        std::shared_ptr<TsAdaptationField> adaptationField;
        std::shared_ptr<TsPMT> pmt;
        std::shared_ptr<TsPAT> pat;
    public:
        int Parse(unsigned char *pTSData);
    };


    class mpegts
    {
    public:
        mpegts();
        std::vector<TsPacket> tsPackets;
    };

    class PesHeader {
        uint32_t packet_start_code_prefix;
        uint8_t stream_id;
        uint16_t PES_packet_length;
        uint8_t PTS_DTS_flags;
        uint64_t pts;
    };

    class Pes {
        TsHeader hdr;
        PesHeader pesHdr;
        TsAdaptationField adpt_fld;
        uint8_t isParseStart;
        uint32_t parsedLength;
        int pesHdrLen;
        uint8_t* pData;
        int nDataLen;
        int nDataCap;
    };

    class MpegTs : public Parser {
    public:
        int Parse(Reader* pReader);
         ~MpegTs();
    private:
        int isParsedPat;
        TsPAT pat;
        std::map<uint16_t, TsPMT*> pmts; //TsPMT pmt[TS_MAX_PMT];
        std::map<uint16_t, Pes*> pes; //Pes pes[TS_MAX_STREAMS];
        std::vector<std::vector<uint8_t>> rawData;
        int lastParsedPesIdx;

       
    };

}

#endif // __MPEGTS_STRUCT__