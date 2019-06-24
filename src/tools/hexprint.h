
#ifndef __HEX_PRINT_H__
#define __HEX_PRINT_H__

#include <cstdint>

namespace AVD {
    int HexSprint(char * pData, int nDataLen, char * pBufferOut);
    int HexSprintLineCaclu(int nDataLen);
    int HexSprintBufferSize(int nDataLen);
}

#endif
