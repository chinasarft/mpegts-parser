
#include <gtest/gtest.h>
#include "sps.h"
#include "AVCDecoderConfigurationRecord.h"
#include "HEVCDecoderConfigurationRecord.h"

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

TEST(TestSps, pasreSps)
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

TEST(TestAVCseqhdr, parse264seqhdr)
{
    const char *p = "0142c015ffe1001c6742c015d901e096ffc0040003c4000003000400000300c83c58b92001000568cb83cb20";
    int strLen = strlen(p);
    uint8_t bin[128];
    int binLen = hexstr2bin((char *)p, (char *)bin, strLen);
    ASSERT_EQ(binLen, strLen/2);
    
    auto pair = AVD::ParseAVCDecoderConfigurationRecord(bin, binLen);
    ASSERT_EQ(pair.second, 0);
    
    auto c = pair.first;
    ASSERT_EQ(c->numOfSequenceParameterSets, 1);
    
    ASSERT_EQ(c->numOfPictureParameterSets, 1);
}

TEST(TestHEVCseqhdr, parse265seqhdr)
{
    const char *p = "01016000000300900000030000f000fcfdf8f800000303200001001840010c01ffff01600000030090000003000003003f959809210001002e42010101600000030090000003000003003fa00f08048596566924cafff0010000f010000003001000000301908022000100074401c172b46240";
    int strLen = strlen(p);
    uint8_t bin[256];
    int binLen = hexstr2bin((char *)p, (char *)bin, strLen);
    ASSERT_EQ(binLen, strLen/2);
    
    auto pair = AVD::ParseHEVCDecoderConfigurationRecord(bin, binLen);
    ASSERT_EQ(pair.second, 0);
    
    for (int i = 0; i <pair.first->arrays.size(); i++) {
        if (pair.first->arrays[i].NAL_unit_type == 33) { // SPS
            
            auto spsret = AVD::ParseSps(pair.first->arrays[i].unit.nalUnit, pair.first->arrays[i].unit.nalUnitLength);
            ASSERT_EQ(spsret.second, 0);
            
            AVD::SPS* sps = spsret.first;
            printf("%d x %d\n", (sps->Pic_width_in_mbs_minus1+1)*16, (sps->Pic_height_in_map_units_minus1+1)*16);
        }
    }
}
