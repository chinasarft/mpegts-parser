#ifndef __SPS_H__
#define __SPS_H__

#include <vector>
#include <memory>
#include <cstdint>

/*
ue -> unsigned Golomb
14496-10 7.3.2.1
seq_parameter_set_data( ) {
    profile_idc 0 u(8)
    constraint_set0_flag 0 u(1)
    constraint_set1_flag 0 u(1)
    constraint_set2_flag 0 u(1)
    constraint_set3_flag 0 u(1)
    constraint_set4_flag 0 u(1)
    constraint_set5_flag 0 u(1)
    reserved_zero_2bits  0 u(2) // equal to 0
    level_idc 0 u(8)
    seq_parameter_set_id 0 ue(v)
    if( profile_idc = = 100 | profile_idc = = 110 | profile_idc = = 122 |
       profile_idc = = 244 | profile_idc = = 44 | profile_idc = = 83 |
       profile_idc = = 86 | | profile_idc = = 118 |profile_idc = = 128 |
       profile_idc = = 138 | | profile_idc = = 139 | | profile_idc = = 134 ) {
        chroma_format_idc 0 ue(v)
        if( chroma_format_idc == 3 )
            separate_colour_plane_flag 0 u(1)
        bit_depth_luma_minus8 0 ue(v)
        bit_depth_chroma_minus8 0 ue(v)
        qpprime_y_zero_transform_bypass_flag 0 u(1)
        seq_scaling_matrix_present_flag 0 u(1)
        if( seq_scaling_matrix_present_flag )
            for(i=0;i<((chroma_format_idc != 3)?8:12);i++) {
                seq_scaling_list_present_flag[ i ] 0 u(1)
                if( seq_scaling_list_present_flag[ i ] )
                    if( i < 6 )
                        scaling_list( ScalingList4x4[ i ], 16, UseDefaultScalingMatrix4x4Flag[ i ]) 0
                    else
                        scaling_list( ScalingList8x8[ i − 6 ], 64, UseDefaultScalingMatrix8x8Flag[ i − 6 ] ) 0
                }
    }
    log2_max_frame_num_minus4 0 ue(v)
    pic_order_cnt_type 0 ue(v)
    if( pic_order_cnt_type == 0 )
        log2_max_pic_order_cnt_lsb_minus4 0 ue(v)
    else if( pic_order_cnt_type = = 1 ) {
        delta_pic_order_always_zero_flag 0 u(1)
        offset_for_non_ref_pic 0 se(v)
        offset_for_top_to_bottom_field 0 se(v)
        num_ref_frames_in_pic_order_cnt_cycle 0 ue(v)
        for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
            offset_for_ref_frame[ i ] 0 se(v)
    }
    
    max_num_ref_frames 0 ue(v)
    gaps_in_frame_num_value_allowed_flag 0 u(1)
    pic_width_in_mbs_minus1 0 ue(v)
    pic_height_in_map_units_minus1 0 ue(v)
    frame_mbs_only_flag 0 u(1)
    if( !frame_mbs_only_flag )
        mb_adaptive_frame_field_flag 0 u(1)
    direct_8x8_inference_flag 0 u(1)
    frame_cropping_flag 0 u(1)
    if( frame_cropping_flag ) {
        frame_crop_left_offset 0 ue(v)
        frame_crop_right_offset 0 ue(v)
        frame_crop_top_offset 0 ue(v)
        frame_crop_bottom_offset 0 ue(v)
    }
    vui_parameters_present_flag 0 u(1)
    if( vui_parameters_present_flag )
        vui_parameters( ) 0
}
*/
namespace  AVD {
    
    struct ScalingList {
        int8_t Delta_scale[64];
    };
    
    struct SeqScalingMatrix {
        uint8_t Seq_scaling_list_present_flag;
        std::vector<ScalingList> ScalingList4x4;
        std::vector<ScalingList> ScalingList8x8;
    };
    
    struct ProfileIdcExt {
        uint8_t Chroma_format_idc                   ;
        uint8_t Separate_colour_plane_flag          ;
        uint8_t Bit_depth_luma_minus8               ;
        uint8_t Bit_depth_chroma_minus8             ;
        uint8_t Qpprime_y_zero_transform_bypass_flag;
        uint8_t Seq_scaling_matrix_present_flag     ;
        std::vector<SeqScalingMatrix> Matrix ;
    };
    
    struct RawHRD {
        uint8_t Cpb_cnt_minus1;
        uint8_t Bit_rate_scale;
        uint8_t Cpb_size_scale;
        
        uint32_t Bit_rate_value_minus1[32];  //H264_MAX_CPB_CNT =;
        uint32_t Cpb_size_value_minus1[32];
        uint8_t Cbr_flag[32]             ;
        
        uint8_t Initial_cpb_removal_delay_length_minus1;
        uint8_t Cpb_removal_delay_length_minus1        ;
        uint8_t Dpb_output_delay_length_minus1         ;
        uint8_t Time_offset_length                     ;
    };
    
    struct RawVUI {
        uint8_t Aspect_ratio_info_present_flag;
        uint8_t Aspect_ratio_idc              ;
        uint16_t Sar_width                     ;
        uint16_t Sar_height                    ;
        
        uint8_t Overscan_info_present_flag;
        uint8_t Overscan_appropriate_flag ;
        
        uint8_t Video_signal_type_present_flag ;
        uint8_t Video_format                   ;
        uint8_t Video_full_range_flag          ;
        uint8_t Colour_description_present_flag;
        uint8_t Colour_primaries               ;
        uint8_t Transfer_characteristics       ;
        uint8_t Matrix_coefficients            ;
        
        uint8_t Chroma_loc_info_present_flag       ;
        uint8_t Chroma_sample_loc_type_top_field   ;
        uint8_t Chroma_sample_loc_type_bottom_field;
        
        uint8_t Timing_info_present_flag;
        uint32_t Num_units_in_tick       ;
        uint32_t Time_scale              ;
        uint8_t Fixed_frame_rate_flag   ;
        
        uint8_t Nal_hrd_parameters_present_flag;
        RawHRD Nal_hrd_parameters             ;
        uint8_t Vcl_hrd_parameters_present_flag;
        RawHRD Vcl_hrd_parameters             ;
        uint8_t Low_delay_hrd_flag             ;
        
        uint8_t Pic_struct_present_flag;
        
        uint8_t Bitstream_restriction_flag             ;
        uint8_t Motion_vectors_over_pic_boundaries_flag;
        uint8_t Max_bytes_per_pic_denom                ;
        uint8_t Max_bits_per_mb_denom                  ;
        uint8_t Log2_max_mv_length_horizontal          ;
        uint8_t Log2_max_mv_length_vertical            ;
        uint8_t Max_num_reorder_frames                 ;
        uint8_t Max_dec_frame_buffering                ;
    };
    
    class SPS {
    public:
        static const int GolombError = -1000;
        uint8_t Profile_idc         ;
        uint8_t Constraint_set0_flag;
        uint8_t Constraint_set1_flag;
        uint8_t Constraint_set2_flag;
        uint8_t Constraint_set3_flag;
        uint8_t Constraint_set4_flag;
        uint8_t Constraint_set5_flag;
        uint8_t Reserved_zero_2bits ;
        uint8_t Level_idc           ;
        uint8_t Seq_parameter_set_id;
        
        ProfileIdcExt Profile_idc_ext;
        
        uint8_t Log2_max_frame_num_minus4            ;
        uint8_t Pic_order_cnt_type                   ;
        uint8_t Log2_max_pic_order_cnt_lsb_minus4    ;
        uint8_t Delta_pic_order_always_zero_flag     ;
        int32_t Offset_for_non_ref_pic               ;
        int32_t Offset_for_top_to_bottom_field       ;
        uint8_t Num_ref_frames_in_pic_order_cnt_cycle;
        int32_t Offset_for_ref_frame[256]                 ;
        
        uint8_t Max_num_ref_frames                  ;
        uint8_t Gaps_in_frame_num_value_allowed_flag;
        
        uint16_t Pic_width_in_mbs_minus1       ;
        uint16_t Pic_height_in_map_units_minus1;
        
        uint8_t Frame_mbs_only_flag         ;
        uint8_t Mb_adaptive_frame_field_flag;
        uint8_t Direct_8x8_inference_flag   ;
        
        uint8_t Frame_cropping_flag     ;
        uint16_t Frame_crop_left_offset  ;
        uint16_t Frame_crop_right_offset ;
        uint16_t Frame_crop_top_offset   ;
        uint16_t Frame_crop_bottom_offset;
        
        uint8_t Vui_parameters_present_flag;
        RawVUI Vui                        ;
    };
    
    std::pair<SPS*, int> ParseSps(uint8_t* pData, int nDataLen);
}

#endif
