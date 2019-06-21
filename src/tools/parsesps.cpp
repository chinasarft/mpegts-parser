#include "bitreader.h"
#include "sps.h"



namespace AVD {
    static int ParseProfileIdcExt(BitReader *r, ProfileIdcExt *pie);
    static int ParseVuiParameters(BitReader *r, RawVUI *vui);
    
    static int unsignedGolomb(BitReader *r, uint32_t *res) {
        int leadingZeroCount = 0;
        uint32_t b1 = 0;
        while(true) {
            b1 =  GetBits(r, 1);
            if (IsOverFlow(r)) {
                return SPS::GolombError;
            }
            if (b1) {
                break;
            } else {
                leadingZeroCount++;
            }
        }
        
        if (leadingZeroCount == 0) {
            *res = 0;
            return 0;
        }
        uint32_t v = 1;
        
        for (int i = 0; i < leadingZeroCount; i++) {
            v = v << 1;
            b1 =  GetBits(r, 1);
            if (IsOverFlow(r)) {
                return SPS::GolombError;
            }
            if (b1) {
                v |= 0x01;
            }
        }
        v -= 1;
        *res = v;
        return 0;
    }
    
    static int signedGolomb(BitReader *r, int *res) {
        uint32_t u32 = 0;
        int ret = 0;
        if ((ret = unsignedGolomb(r, &u32)) != 0) {
            return ret;
        }
        if ((u32%2) == 0) { // negative value
            *res = (-(int(u32) - 1)) / 2;
        } else { // positive value
            *res = (int(u32) + 1) / 2;
        }
        return 0;
    }
    
    std::pair<SPS*, int> ParseSps(uint8_t* pData, int nDataLen) {
        BitReader br;
        InitBitReader(&br, pData, nDataLen);
        
        SPS* sps = new SPS();
        sps->Profile_idc = GetBits(&br, 8);
        
        sps->Constraint_set0_flag = GetBits(&br, 1);
        sps->Constraint_set1_flag = GetBits(&br, 1);
        sps->Constraint_set2_flag = GetBits(&br, 1);
        sps->Constraint_set3_flag = GetBits(&br, 1);
        sps->Constraint_set4_flag = GetBits(&br, 1);
        sps->Reserved_zero_2bits = GetBits(&br, 2);
        sps->Level_idc = GetBits(&br, 8);
        
        int se = 0;
        uint32_t ue = 0;
        int ret = unsignedGolomb(&br, &ue);
        if (ret) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        sps->Seq_parameter_set_id = ue;
        
        uint8_t profile_idc = sps->Profile_idc;
        if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 ||
            profile_idc == 244 || profile_idc == 44 || profile_idc == 83 ||
            profile_idc == 86 || profile_idc == 118 || profile_idc == 128 ||
            profile_idc == 138 || profile_idc == 139 || profile_idc == 134) {
            
            if (ParseProfileIdcExt(&br, &sps->Profile_idc_ext) != 0) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            
        }
        
        ret = unsignedGolomb(&br, &ue);
        if (ret) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        sps->Log2_max_frame_num_minus4 = ue;
        
        ret = unsignedGolomb(&br, &ue);
        if (ret) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        sps->Pic_order_cnt_type = ue;
        if (sps->Pic_order_cnt_type == 0) {
            ret = unsignedGolomb(&br, &ue);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Log2_max_pic_order_cnt_lsb_minus4 = ue;
        } else if (sps->Pic_order_cnt_type == 1) {

            sps->Delta_pic_order_always_zero_flag = GetBits(&br, 1);
            ret = signedGolomb(&br, &se);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Offset_for_non_ref_pic = se;
            
            ret = signedGolomb(&br, &se);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Offset_for_top_to_bottom_field = se;
            
            ret = unsignedGolomb(&br, &ue);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Num_ref_frames_in_pic_order_cnt_cycle = ue;
            
            for(int i = 0; i < sps->Num_ref_frames_in_pic_order_cnt_cycle; i++ ) {
                ret = signedGolomb(&br, &se);
                if (ret) {
                    delete sps;
                    return std::make_pair<SPS*, int>(nullptr, ret);
                }
                sps->Offset_for_ref_frame[i] = se;
            }
        }
        
        ret = unsignedGolomb(&br, &ue);
        if (ret) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        sps->Max_num_ref_frames = ue;
        
        sps->Gaps_in_frame_num_value_allowed_flag = GetBits(&br, 1);
        ret = unsignedGolomb(&br, &ue);
        if (ret) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        sps->Pic_width_in_mbs_minus1 = ue;
        
        ret = unsignedGolomb(&br, &ue);
        if (ret) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        sps->Pic_height_in_map_units_minus1 = ue;
        
        sps->Frame_mbs_only_flag = GetBits(&br, 1);
        
        if( !sps->Frame_mbs_only_flag )
            sps->Mb_adaptive_frame_field_flag = GetBits(&br, 1);
        sps->Direct_8x8_inference_flag = GetBits(&br, 1);
        sps->Frame_cropping_flag = GetBits(&br, 1);
        if(sps->Frame_cropping_flag) {
            ret = unsignedGolomb(&br, &ue);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Frame_crop_left_offset = ue;
            
            ret = unsignedGolomb(&br, &ue);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Frame_crop_right_offset = ue;
            
            ret = unsignedGolomb(&br, &ue);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Frame_crop_top_offset = ue;
            
            ret = unsignedGolomb(&br, &ue);
            if (ret) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
            sps->Frame_crop_bottom_offset = ue;
        }
        
        sps->Vui_parameters_present_flag = GetBits(&br, 1);
        
        if (sps->Vui_parameters_present_flag) {
            if (ParseVuiParameters(&br, &sps->Vui) != 0) {
                delete sps;
                return std::make_pair<SPS*, int>(nullptr, ret);
            }
        }
        if (IsOverFlow(&br)) {
            delete sps;
            return std::make_pair<SPS*, int>(nullptr, ret);
        }
        return std::make_pair<SPS*, int>(sps, 0);
    }
    
    static int ParseProfileIdcExt(BitReader *r, ProfileIdcExt *pie) {
        uint32_t ue = 0;
        int ret = unsignedGolomb(r, &ue);
        if (ret != 0) {
            return ret;
        }
        pie->Chroma_format_idc = ue;
        if (pie->Chroma_format_idc == 3) {
            pie->Separate_colour_plane_flag = GetBits(r, 1);
            
            ret = unsignedGolomb(r, &ue);
            if (ret != 0) {
                return ret;
            }
            pie->Bit_depth_luma_minus8 = ue;
            
            ret = unsignedGolomb(r, &ue);
            if (ret != 0) {
                return ret;
            }
            pie->Bit_depth_chroma_minus8 = ue;
            
            pie->Qpprime_y_zero_transform_bypass_flag = GetBits(r, 1);
            pie->Seq_scaling_matrix_present_flag = GetBits(r, 1);
            if (pie->Seq_scaling_matrix_present_flag) {
                for (int i = 0; (pie->Chroma_format_idc != 3)?8:12; i++) {
                    SeqScalingMatrix matrix;
                    matrix.Seq_scaling_list_present_flag = GetBits(r, 1);
                    
                    if (matrix.Seq_scaling_list_present_flag) {
                        if (i < 6) {
                            for (int j = 0; j < 16; j++) {
                                matrix.ScalingList4x4[i].Delta_scale[j] = GetBits(r, 8);
                            }
                        } else {
                            for (int j = 0; j < 64; j++) {
                                matrix.ScalingList8x8[i-6].Delta_scale[j] = GetBits(r, 8);
                            }
                        }
                    }
                    pie->Matrix.push_back(matrix);
                }
            }
        }
        if (IsOverFlow(r)) {
            return SPS::GolombError;
        }
        return 0;
        
    }
    
    static int ParseVuiParameters(BitReader *r, RawVUI *vui) {
        return 0;
    }
    
}
