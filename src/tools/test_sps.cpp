
#include <gtest/gtest.h>
#include "sps.h"

static int hexstr2bin( char *cp_hexstr, char *cp_bin,
                      int i_hexstr_len )
{
    int i, i_high, i_low;
    if(cp_hexstr == NULL || cp_bin == NULL)
        return -1;
    if(i_hexstr_len % 2 != 0)
        return -1;
    for( i = 0; i < i_hexstr_len; i += 2 ) {
        i_high = cp_hexstr[i];
        i_low  = cp_hexstr[i + 1];
        
        if( i_high > 0x39 )
            i_high -= 0x37;
        else
            i_high -= 0x30;
        
        if( i == i_hexstr_len - 1 )
            i_low = 0;
        else if( i_low > 0x39 )
            i_low -= 0x37;
        else
            i_low -= 0x30;
        
        cp_bin[i / 2] = (i_high << 4) | (i_low & 0x0f);
    }
    return i_hexstr_len/2;
}

TEST(TestSps,HandlerTrueReturn)
{
    const char *p = "42c015d901e096ffc0040003c4000003000400000300c83c58b920";
    int strLen = strlen(p);
    uint8_t bin[128];
    int binLen = hexstr2bin((char *)p, (char *)bin, strLen);
    ASSERT_EQ(binLen, strLen/2);
    
    auto pair = AVD::ParseSps(bin, binLen);
    ASSERT_EQ(pair.second, 0);
    
    AVD::SPS* sps = pair.first;
    ASSERT_EQ((sps->Pic_width_in_mbs_minus1+1)*16, 480);
    
    ASSERT_EQ((sps->Pic_height_in_map_units_minus1+1)*16, 288);
}
