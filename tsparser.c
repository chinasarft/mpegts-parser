#include "bitreader.h"
#include "tsparser.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

// CRC32 lookup table for polynomial 0x04c11db7
static uint32_t crc_table[256] = {
        0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
        0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
        0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
        0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
        0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
        0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
        0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
        0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
        0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
        0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
        0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
        0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
        0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
        0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
        0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
        0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
        0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
        0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
        0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
        0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
        0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
        0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
        0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
        0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
        0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
        0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
        0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
        0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
        0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
        0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
        0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
        0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
        0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
        0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
        0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
        0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
        0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
        0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
        0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
        0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
        0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
        0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
        0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4};

uint32_t crc32 (uint8_t *data, int len) {
        register int i;
        uint32_t crc = 0xffffffff;
        
        for (i=0; i<len; i++)
                crc = (crc << 8) ^ crc_table[((crc >> 24) ^ *data++) & 0xff];
        
        return crc;
}

int check_crc32(uint8_t *data, int len, uint8_t* pCrc) {
    int c32 = crc32(data, len);
    uint8_t *pTmp =  (uint8_t*)&c32;
    uint8_t buf[4];
    buf[0] = pTmp[3];
    buf[1] = pTmp[2];
    buf[2] = pTmp[1];
    buf[3] = pTmp[0];
    return memcmp(pCrc, buf, 4);
}

static inline int check_continuity_counter(uint8_t pre, uint8_t cur) {
    if (pre == 0x0F) {
        return 0 == cur;
    }
    return pre+1 == cur;
}

static void ts_parse_header(BitReader* pBitReader, TsHeader* pHdr) {
    pHdr->sync_byte = (uint8_t)GetBits(pBitReader, 8);
    assert(pHdr->sync_byte != 47);
    pHdr->transport_error_indicator = (uint8_t)GetBits(pBitReader, 1);
    pHdr->payload_unit_start_indicator = (uint8_t)GetBits(pBitReader, 1);
    pHdr->transport_priority = (uint8_t)GetBits(pBitReader, 1);
    pHdr->PID = (uint16_t)GetBits(pBitReader, 13);
    pHdr->transport_scrambling_control = (uint8_t)GetBits(pBitReader, 2);
    pHdr->adaptation_field_control = (uint8_t)GetBits(pBitReader, 2);
    pHdr->continuity_counter = (uint8_t)GetBits(pBitReader, 4);

    return;
}

static int is_pmt_pid(MpegTs *pTs, uint16_t pid) {
    PmtMap* pPmtMap = (PmtMap*)&pTs->pat.map;
    for (int i = 0; i < TS_MAX_PROGRAM_MAP; i++) {
        if (pPmtMap[0].program_map_PID > 0) {
            if (pid == pPmtMap[0].program_map_PID) {
                return 1;
            }
        } else {
            return 0;
        }
    }

    return 0;
}

static int is_pes_pid(MpegTs *pTs, uint16_t pid, const TsPMT** ppPmt) {
    TsPMT* pPmt = (TsPMT*)&pTs->pmt;
    int pmtLen = sizeof(pTs->pmt) / sizeof(TsPMT);
    for (int i = 0; i < pmtLen; i++) {
        for (int j = 0; j <= TS_MAX_STREAMS; j++) {
            if (pPmt[i].streamInfo[j].elementary_PID > 0) {
                if (pPmt[i].streamInfo[j].elementary_PID == pid) {
                    if (ppPmt)
                        *ppPmt = &pPmt[i];
                    return 1;
                }
            } else {
                break;
            }
        } 
    }

    return 0;
}

static uint8_t get_stream_type_by_pid(const TsPMT* pPmt, uint16_t pid) {

    for (int j = 0; j <= TS_MAX_STREAMS; j++) {
        if (pPmt->streamInfo[j].elementary_PID > 0) {
            if (pPmt->streamInfo[j].elementary_PID == pid) {
                return pPmt->streamInfo[j].stream_type;
            }
        } else {
            return 0;
        }
    } 
    return 0;
}

static const char * get_name_by_stream_type(uint8_t stype) {
    switch (stype) {
        case 0x1b:
            return "h264";
        case 0x0f:
            return "aac";
        case 0x06:
            return "private";
        case 0x24:
            return "h265";
    }
    return "unknown";
}

static void parse_adaptation_field(BitReader* pBitReader, TsAdaptationField* pAdaptField) {
    pAdaptField->adaptation_field_length = GetBits(pBitReader, 8);
    if (pAdaptField->adaptation_field_length == 0)
        return;

    int nParseLen = 0;
    SkipBits(pBitReader, 3);
    pAdaptField->PCR_flag = GetBits(pBitReader, 1);
    SkipBits(pBitReader, 4);
    nParseLen++;

    if (pAdaptField->PCR_flag == 0) {
        SkipBits(pBitReader, (pAdaptField->adaptation_field_length-nParseLen) * 8);
        return;
    } 

    uint32_t b32 = GetBits(pBitReader, 32);
    uint32_t tmp = GetBits(pBitReader, 1);

    pAdaptField->pcr = ((uint64_t)b32)<<1 | tmp;
    SkipBits(pBitReader, 6);

    tmp = GetBits(pBitReader, 9);

    pAdaptField->pcr = pAdaptField->pcr * 300 + tmp;
    printf("pcr:%"PRId64" millisec:%.3f\n", pAdaptField->pcr, ((pAdaptField->pcr/300) * 300)/27000000.0);

    return;
}

static int parse_pat(BitReader* pBitReader, TsPAT* pPat) {
    pPat->table_id = GetBits(pBitReader, 8);
    pPat->section_syntax_indicator  = GetBits(pBitReader, 1);
    pPat->must_zero =  GetBits(pBitReader, 1);
    pPat->reserved1  = GetBits(pBitReader, 2);
    pPat->section_length = GetBits(pBitReader, 12);
    pPat->transport_stream_id = GetBits(pBitReader, 16);
    pPat->reserved2 = GetBits(pBitReader, 2);
    pPat->version_number = GetBits(pBitReader, 5);
    pPat->current_next_indicator = GetBits(pBitReader, 1);
    pPat->section_number = GetBits(pBitReader, 8);
    pPat->last_section_number = GetBits(pBitReader, 8);

    int left = pPat->section_length - 9; // section_length 
    int idx = 0, i = 0;
    for (i = 0; i+4 <= left; i+=4, idx++) {
        pPat->map[idx].program_number =  GetBits(pBitReader, 16);
        SkipBits(pBitReader, 3);
        pPat->map[idx].program_map_PID =  GetBits(pBitReader, 13);
    }
    if (idx < TS_MAX_PROGRAM_MAP) {
        pPat->map[idx].program_map_PID = 0;
    }
    //SkipBits(pBitReader, left - i);

    return 0;
}

static int parse_pmt(BitReader* pBitReader, TsPMT* pPmt) {
    pPmt->table_id = GetBits(pBitReader, 8);
    pPmt->section_syntax_indicator  = GetBits(pBitReader, 1);
    pPmt->must_zero =  GetBits(pBitReader, 1);
    pPmt->reserved1  = GetBits(pBitReader, 2);
    pPmt->section_length = GetBits(pBitReader, 12);
    pPmt->program_number = GetBits(pBitReader, 16);
    pPmt->reserved2 = GetBits(pBitReader, 2);
    pPmt->version_number = GetBits(pBitReader, 5);
    pPmt->current_next_indicator = GetBits(pBitReader, 1);
    pPmt->section_number = GetBits(pBitReader, 8);
    pPmt->last_section_number = GetBits(pBitReader, 8);

    pPmt->reserved3 = GetBits(pBitReader, 3);
    pPmt->PCR_PID = GetBits(pBitReader, 13);
    pPmt->reserved4 = GetBits(pBitReader, 4);
    pPmt->program_info_length = GetBits(pBitReader, 12); //目前都为 0 不考虑。

    int left = pPmt->section_length - 9 - 4; // for crc32
    int count = 0;
    for (int i = 0; i < left && count < TS_MAX_STREAMS;) {
        pPmt->streamInfo[count].stream_type = GetBits(pBitReader, 8);
        pPmt->streamInfo[count].reserved1 = GetBits(pBitReader, 3);
        pPmt->streamInfo[count].elementary_PID = GetBits(pBitReader, 13);
        pPmt->streamInfo[count].reserved2 = GetBits(pBitReader, 4);
        pPmt->streamInfo[count].ES_info_length = GetBits(pBitReader, 12);
        if ((pPmt->streamInfo[count].ES_info_length & 0xC00) != 0 || pPmt->streamInfo[count].ES_info_length > TS_MAX_STREAM_DESCRIPTOR) {
            printf("pmt error ES_info_length:%d\n", pPmt->streamInfo[count].ES_info_length);
            SkipBits(pBitReader, (left - 5) * 8);
            return -1;
        }
        i += (5 + pPmt->streamInfo[count].ES_info_length);

        for (int j = 0; j < pPmt->streamInfo[count].ES_info_length && j < TS_MAX_STREAM_DESCRIPTOR; j++) {
            pPmt->streamInfo[count].descriptor[j] = GetBits(pBitReader, 8);
        }
        count ++;
    }
    pPmt->streamInfoCount = count;

    return 0;
}

static int parse_pes_header(BitReader* pBitReader, Pes* pPes) {
    pPes->pesHdr.packet_start_code_prefix = GetBits(pBitReader, 24);
    pPes->pesHdr.stream_id = GetBits(pBitReader, 8);
    pPes->pesHdr.PES_packet_length = GetBits(pBitReader, 16);
    SkipBits(pBitReader, 8);
    pPes->pesHdr.PTS_DTS_flags = GetBits(pBitReader, 2);
    SkipBits(pBitReader, 14);
    if(pPes->pesHdr.PTS_DTS_flags == 2) { // 只有pts
        SkipBits(pBitReader, 4);
        uint64_t a = (uint64_t)GetBits(pBitReader, 3);
        SkipBits(pBitReader, 1);
        uint64_t b = (uint64_t)GetBits(pBitReader, 15);
        SkipBits(pBitReader, 1);
        uint64_t c = (uint64_t)GetBits(pBitReader, 15);
        SkipBits(pBitReader, 1);
        pPes->pesHdr.pts = a<<30 | b<<15 | c;
    }
    pPes->isParseStart = 1;
    pPes->parsedLength = 8;
    pPes->pesHdrLen = 8;
    // TODO pts后面就是数据了，复杂格式的后面再说
    return 0;
}

static void fill_frame(Pes* pPes, TsParsedFrame* pFrame, uint8_t stype) {
    pFrame->pData = pPes->pData;
    pFrame->nDataLen = pPes->nDataLen;
    pFrame->stype = stype;
    pFrame->PID = pPes->hdr.PID;
    pFrame->nPts = pPes->pesHdr.pts;
}

static void get_pes(MpegTs *pTs, const TsPMT* pPmt, int idx, TsParsedFrame pFrames[2]) {
    Pes* pPes = &pTs->pes[idx];
    uint8_t stype;
    if (pPes->nDataLen > 0) {
        stype = get_stream_type_by_pid(pPmt, pPes->hdr.PID);
        const char *typeName = get_name_by_stream_type(stype);
        printf("%8s pts:%"PRId64" millisec:%.3f pesLen:%d\n", typeName, pPes->pesHdr.pts, 
            pPes->pesHdr.pts/90000.0, pPes->pesHdr.PES_packet_length);
    } else {
        return;
    }

    if (pFrames == NULL)
        return;
    for(int i = 0; i < 2; i ++) {
        if (pFrames[i].pData == NULL) {
           fill_frame(pPes, &pFrames[i], stype);
           break;
        }
    }
    
    return;
}

void ts_init(MpegTs *pTs) {
    if (pTs) {
        memset(pTs, 0, sizeof(MpegTs));
    }
    return;
}

int ts_parse_buffer(MpegTs *pTs, uint8_t* pData, int nDataLen, TsParsedFrame pFrames[2]) {
    assert(nDataLen == 188 && pData != NULL);
    if (pFrames) {
        if (pFrames != NULL)
            memset(pFrames, 0, sizeof(TsParsedFrame)*2);
    }
        

    BitReader bitReader;
    InitBitReader(&bitReader, pData, nDataLen);

    TsHeader hdr;
    ts_parse_header(&bitReader, &hdr);

    PointerField pointerField = {0, 0};
    if (hdr.PID == 0 || is_pmt_pid(pTs, hdr.PID )) {
        if (hdr.payload_unit_start_indicator) {
            pointerField.is_exist = 1;
            pointerField.len = GetBits(&bitReader, 8);
            SkipBits(&bitReader, pointerField.len * 8);
        }
    } 
    int left = numBitsLeft(&bitReader) / 8;
    int crcstart = nDataLen - left;

    TsAdaptationField adaptField = {0, 0};
    if(hdr.adaptation_field_control == 2 || hdr.adaptation_field_control == 3) {
		parse_adaptation_field(&bitReader, &adaptField);
	}

    if (!pTs->isParsedPat && hdr.PID != 0) {
        printf("not meet pat\n");
        return 1;
    }
    
    if (hdr.PID == 0) {
        if (pTs->isParsedPat) {
            if (!check_continuity_counter(pTs->pat.hdr.continuity_counter, hdr.continuity_counter)) {
                printf("PID(%d) continuity_counter error:%d %d\n", hdr.PID, pTs->pat.hdr.continuity_counter, hdr.continuity_counter);
            }
        }
        pTs->pat.hdr = hdr;
        pTs->pat.poiter_field = pointerField;
        pTs->pat.adpt_fld = adaptField;
        parse_pat(&bitReader, &pTs->pat);
        pTs->isParsedPat = 1;

        int crcend = nDataLen - numBitsLeft(&bitReader) / 8;
        // -1, section_length 包含4bitcrc32, 以及不包含包括section_length在内的3个bit, 所以-1
        if (check_crc32(pData+crcstart, pTs->pat.section_length-1, pData+crcend) != 0) {
            printf("pat crc32 error\n");
        }
        return 0;
    }

    if (is_pmt_pid(pTs, hdr.PID)) {

        TsPMT *pPmt = NULL;
        for (int i = 0; i < TS_MAX_PMT; i++) {
             if (pTs->pmt[i].table_id == 0) {
                pPmt = &pTs->pmt[i];
                break;
             }
            if (pTs->pmt[i].hdr.PID == hdr.PID)
                pPmt = &pTs->pmt[i];
        }

        if (pPmt->table_id != 0) {
            if (!check_continuity_counter(pPmt->hdr.continuity_counter, hdr.continuity_counter)) {
                printf("PID(%d) continuity_counter error:%d %d\n", hdr.PID, pTs->pat.hdr.continuity_counter, hdr.continuity_counter);
            }
        }
        parse_pmt(&bitReader, pPmt);
                
        pPmt->hdr = hdr;
        pPmt->poiter_field = pointerField;
        pPmt->adpt_fld = adaptField;
        int crcend = nDataLen - numBitsLeft(&bitReader) / 8;
        if (check_crc32(pData+crcstart, crcend - crcstart, pData+crcend) != 0) {
            printf("pmt crc32 error\n");
        }
        for (int i = 0; i < pPmt->streamInfoCount; i++) {
            if (pPmt->streamInfo[i].ES_info_length > 0) {
                const char *typeName = get_name_by_stream_type(pPmt->streamInfo[i].stream_type);
                if (pPmt->streamInfo[i].ES_info_length < TS_MAX_STREAM_DESCRIPTOR) {
                    printf("%s descriptor:", typeName);
                    for (int j = 0; j < pPmt->streamInfo[i].ES_info_length; j++) {
                        printf("%02X", pPmt->streamInfo[i].descriptor[j]);
                    }
                    printf("\n");
                }
            }
        }
        return 0;
    }

    const TsPMT *pPmt = NULL;
    if (is_pes_pid(pTs, hdr.PID, &pPmt)) {
        Pes *pPes = NULL;

        int currentIdx = 0;
        for (int i = 0; i < TS_MAX_STREAMS; i++) {
            if (pTs->pes[i].hdr.PID == 0) {
                pPes = &pTs->pes[i];
                currentIdx = i;
                break;
            }
            if (pTs->pes[i].hdr.PID == hdr.PID) {
                pPes = &pTs->pes[i];
                currentIdx = i;
                break;
            }
        }
        if (pTs->pes[pTs->lastParsedPesIdx].hdr.PID != 0 && pTs->lastParsedPesIdx != currentIdx) {
            get_pes(pTs, pPmt, pTs->lastParsedPesIdx, pFrames);
        }
        pTs->lastParsedPesIdx = currentIdx;

        if (pPes->hdr.PID != 0) {
            if (!check_continuity_counter(pPes->hdr.continuity_counter, hdr.continuity_counter)) {
                printf("PID(%d) continuity_counter error:%d %d\n", hdr.PID, pPes->hdr.continuity_counter, hdr.continuity_counter);
            }
        }
        pPes->hdr = hdr;
        pPes->adpt_fld = adaptField;

        // 新的pes开始了
        if (hdr.payload_unit_start_indicator) {
            if (pPes->isParseStart) { // 检查前一个pes
                if (pPes->pesHdr.PES_packet_length != 0 && pPes->pesHdr.PES_packet_length != pPes->parsedLength) {
                    printf("length error:%d %d", pPes->pesHdr.PES_packet_length, pPes->parsedLength);
                }        
            }
            
            pPes->isParseStart = 0;
            pPes->parsedLength = 0;
            parse_pes_header(&bitReader, pPes);

            int allocLen = pPes->pesHdr.PES_packet_length * 1.5;
            if (allocLen == 0) {
                allocLen = 200*1024; 
            } 
            if (pPes->nDataCap < allocLen) {
                if (pPes->pData == NULL)
                    free((char *)pPes->pData);
                pPes->pData = malloc(allocLen);
                pPes->nDataCap = allocLen;
                pPes->nDataLen = 0;
            }
        }

        int pesRemain = numBitsLeft(&bitReader) / 8;

        if (pesRemain + pPes->nDataLen > pPes->nDataCap) {
                int reAllocLen = pPes->nDataCap*1.3;
                pPes->pData = (uint8_t *)realloc((char *)pPes->pData, reAllocLen);
                pPes->nDataCap = reAllocLen;
        }
        memcpy(pPes->pData+pPes->nDataLen, pData+(188-pesRemain), pesRemain);
        pPes->nDataLen += pesRemain;

        pPes->parsedLength += pesRemain;
        if (pPes->parsedLength == pPes->pesHdr.PES_packet_length) {
            get_pes(pTs, pPmt, currentIdx, pFrames);
            pPes->nDataLen = 0;
        }
    }

    return 1;
}

int ts_flush(MpegTs *pTs, TsParsedFrame* pFrame) {
    if(pFrame == NULL)
        return -1;
    memset(pFrame, 0, sizeof(TsParsedFrame));
    const TsPMT *pPmt = NULL;
    int idx = pTs->lastParsedPesIdx;
    if (is_pes_pid(pTs, pTs->pes[idx].hdr.PID, &pPmt)) {
        if (pTs->pes[idx].nDataLen > 0) {
            uint8_t stype = get_stream_type_by_pid(pPmt, pTs->pes[idx].hdr.PID);
            fill_frame(&pTs->pes[idx], pFrame, stype);
            return 1;
        }
    }
    return 0;
}

void ts_clean(MpegTs *pTs) {
    for (int i = 0; i < TS_MAX_STREAMS; i++) {
        if (pTs->pes[i].pData == 0) {
            free((char *)pTs->pes[i].pData);
        }
    }
    memset(pTs, 0, sizeof(MpegTs));
    return;
}
