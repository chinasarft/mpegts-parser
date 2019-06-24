#include "hexprint.h"
#include <cstring>
#include <cstdio>
#include <cctype>

namespace AVD {
    // 38 38 38 38 38 38 38 38  38 38 38 38 71 77 65 65  88888888 8888qwee
    // 这样一个长度就是67
    static inline uint8_t getPrintChar(uint8_t c) {
        if (isprint(c))
            return c;
        return '.';
    }
    
    int HexSprint(char * pData, int nDataLen, char * pBufferOut)
    {
        int remain = nDataLen;
        int retLen = 0;
        uint8_t *p;
        while(remain >= 16) {
            p = (uint8_t*)(pData+nDataLen-remain);
            retLen += sprintf(pBufferOut+retLen, "%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x  %c%c%c%c%c%c%c%c %c%c%c%c%c%c%c%c\n",
                              p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                              p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15],
                              getPrintChar(p[0]), getPrintChar(p[1]), getPrintChar(p[2]), getPrintChar(p[3]), getPrintChar(p[4]),
                              getPrintChar(p[5]), getPrintChar(p[6]), getPrintChar(p[7]), getPrintChar(p[8]), getPrintChar(p[9]),
                              getPrintChar(p[10]), getPrintChar(p[11]), getPrintChar(p[12]), getPrintChar(p[13]), getPrintChar(p[14]),
                              getPrintChar(p[15]));
            remain -= 16;
        }
        if (remain > 0 ) {
            char *pTmp = pBufferOut+retLen;
            p = (uint8_t*)(pData+nDataLen-remain);
            memset(pBufferOut+retLen, ' ', 67);
            memset(pBufferOut+retLen+67, '\n', 1);
            for (int i = 0; i < remain; i++) {
                sprintf(pTmp+3*i + i/8, "%02X ", p[i]);
                pTmp[3*i + i/8 + 3] = 0x20;
                pTmp[50 + i + i/8] = getPrintChar(p[i]);
            }
            retLen += 68; // 多了个换行
        }
        pBufferOut[retLen] = 0;
        return retLen;
    }
    
    int HexSprintLineCaclu(int nDataLen) {
        return nDataLen/16 + nDataLen%16>0?1:0;
    }
    
    int HexSprintBufferSize(int nDataLen) {
        int line = HexSprintLineCaclu(nDataLen);
        return line*68+1;
    }
    
    int HexSnprint(char * pData, int nDataLen, int nLenLen, char * pBufferOut)
    {
        // 38 38 38 38 38 38 38 38  38 38 38 38 71 77 65 65  88888888 8888qwee
        // 这样一个长度就是67
        int i, j = 0, buf_len, logline_len = 67 + nLenLen;
        char s[100], temp[nLenLen + 1], format_l[10] = { 0 }, format_r[10] = {
            0};
        char *buf_tmp;
        buf_tmp = pBufferOut ;
        sprintf(format_l, "%%0%dd:", nLenLen);
        sprintf(format_r, ":%%0%dd", nLenLen);
        memset(s, 0, sizeof(s));
        memset(s, '-', logline_len);
        sprintf(buf_tmp, "%s\n", s);
        buf_len = logline_len + 1;
        for (i = 0; i < nDataLen; i++) {
            if (j == 0) {
                memset(s, ' ', logline_len);
                sprintf(s, format_l, i);
                sprintf(&s[69 + nLenLen - 1], format_r, i + 16);
            }
            sprintf(temp, "%02X ", (unsigned char) pData[i]);
            memcpy(&s[j * 3 + nLenLen + 1 + (j > 7)], temp, 3);
            if (isprint((unsigned char) pData[i])) {
                s[j + 51 + nLenLen + (j > 7)] = pData[i];
            } else {
                if (pData[i] != 0)
                    s[j + 51 + nLenLen + (j > 7)] = '.';
            }
            j++;
            if (j == 16) {
                s[logline_len] = 0;
                sprintf(buf_tmp + buf_len, "%s\n", s);
                buf_len += logline_len + 1;
                j = 0;
            }
        }
        if (j) {
            s[logline_len] = 0;
            sprintf(buf_tmp + buf_len, "%s\n", s);
            buf_len += logline_len + 1;
        }
        memset(s, 0, sizeof(s));
        
        /*memset(s, '-', logline_len); */
        memset(s, '=', logline_len);        /*为了perl好分析，改为=== */
        sprintf(buf_tmp + buf_len, "%s\n", s);
        buf_len += logline_len + 1;
        return buf_len;
    }
    
}
