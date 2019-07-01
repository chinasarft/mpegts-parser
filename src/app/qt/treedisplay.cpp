#include "treedisplay.h"

namespace AVD {
    
    static void addTreeViewOfSPS_profile_idc (QStandardItem *root, ProfileIdcExt* p) {
        
        std::vector<const char*> names = {"Chroma_format_idc", "Separate_colour_plane_flag", "Bit_depth_luma_minus8",
            "Bit_depth_chroma_minus8", "Qpprime_y_zero_transform_bypass_flag",
            "Seq_scaling_matrix_present_flag", "Matrix",
        };
        std::vector<uint32_t> vs = {
            p->Chroma_format_idc, p->Separate_colour_plane_flag, p->Bit_depth_luma_minus8,
            p->Bit_depth_chroma_minus8, p->Qpprime_y_zero_transform_bypass_flag,
            p->Seq_scaling_matrix_present_flag
        };
        
        showTreeViewHelper(root, names, vs);
        // TODO show matrix
        //for (int i = 0; i < p->Matrix.size(); i++) {}
        return;
    }
    
    void ShowTreeViewOfSPS(QStandardItem *root, SPS* sps) {
        
        QStandardItem* item1 = nullptr;
        QStandardItem* item2 = nullptr;
        QList<QStandardItem*> row;
        
        std::vector<const char*> names =  {"Profile_idc", "Constraint_set0_flag", "Constraint_set1_flag",
            "Constraint_set2_flag", "Constraint_set3_flag", "Constraint_set4_flag",
            "Constraint_set5_flag", "Reserved_zero_2bits", "Level_idc", "Seq_parameter_set_id"
        };
        std::vector<uint32_t> vs = {
            sps->Profile_idc, sps->Constraint_set0_flag, sps->Constraint_set1_flag, sps->Constraint_set2_flag,
            sps->Constraint_set3_flag, sps->Constraint_set4_flag, sps->Constraint_set5_flag,
            sps->Reserved_zero_2bits, sps->Level_idc, sps->Seq_parameter_set_id
        };
        
        showTreeViewHelper(root, names, vs);
        
        uint8_t profile_idc = sps->Profile_idc;
        
        item1 = new QStandardItem("profile_idc");
        item2 = new QStandardItem(QString::number(profile_idc));
        
        if (profile_idc == 100 || profile_idc == 110 || profile_idc == 122 ||
            profile_idc == 244 || profile_idc == 44 || profile_idc == 83 ||
            profile_idc == 86 || profile_idc == 118 || profile_idc == 128 ||
            profile_idc == 138 || profile_idc == 139 || profile_idc == 134) {
            
            addTreeViewOfSPS_profile_idc(item1, &sps->Profile_idc_ext);
        } else {
            row.append(item1);
            row.append(item2);
            root->appendRow(row);
        }
        
        
        names = {
            "Log2_max_frame_num_minus4", "Pic_order_cnt_type", "Log2_max_pic_order_cnt_lsb_minus4",
            "Delta_pic_order_always_zero_flag"
        };
        vs = {
            sps->Log2_max_frame_num_minus4, sps->Pic_order_cnt_type, sps->Log2_max_pic_order_cnt_lsb_minus4,
            sps->Delta_pic_order_always_zero_flag
        };
        showTreeViewHelper(root, names, vs);
        
        names = { "Offset_for_non_ref_pic", "Offset_for_top_to_bottom_field"};
        std::vector<int32_t> intvs = {sps->Offset_for_non_ref_pic, sps->Offset_for_top_to_bottom_field};
        showTreeViewHelper(root, names, intvs);
        
        row.clear();
        item1 = new QStandardItem("Num_ref_frames_in_pic_order_cnt_cycle");
        item2 = new QStandardItem(QString::number(sps->Num_ref_frames_in_pic_order_cnt_cycle));
        row.append(item1);
        row.append(item2);
        root->appendRow(row);
        //TODO Offset_for_ref_frame
        
        names = {
            "Max_num_ref_frames", "Gaps_in_frame_num_value_allowed_flag", "Pic_width_in_mbs_minus1",
            "Pic_height_in_map_units_minus1", "Frame_mbs_only_flag", "Mb_adaptive_frame_field_flag",
            "Direct_8x8_inference_flag", "Frame_cropping_flag", "Frame_crop_left_offset", "Frame_crop_right_offset",
            "Frame_crop_top_offset", "Frame_crop_bottom_offset", "Vui_parameters_present_flag"
        };
        vs = {
            sps->Max_num_ref_frames, sps->Gaps_in_frame_num_value_allowed_flag, sps->Pic_width_in_mbs_minus1,
            sps->Pic_height_in_map_units_minus1, sps->Frame_mbs_only_flag, sps->Mb_adaptive_frame_field_flag,
            sps->Direct_8x8_inference_flag, sps->Frame_cropping_flag, sps->Frame_crop_left_offset,
            sps->Frame_crop_right_offset, sps->Frame_crop_top_offset, sps->Frame_crop_bottom_offset,
            sps->Vui_parameters_present_flag
        };
        showTreeViewHelper(root, names, vs);
        
        return;
    }
 
    void ShowTreeViewOfAVCDecoderConfigurationRecord(QStandardItem *root, AVCDecoderConfigurationRecord* r) {
        std::vector<uint32_t> vs = {
            r->configurationVersion, r->AVCProfileIndication, r->profile_compatibility, r->AVCLevelIndication,
            r->reserved1, r->lengthSizeMinusOne, r->reserved2
        };
        std::vector<const char*> names =  {
            "configurationVersion", "AVCProfileIndication", "profile_compatibility", "AVCLevelIndication",
            "reserved1", "lengthSizeMinusOne", "reserved2"
        };
        showTreeViewHelper(root, names, vs);
        
        QStandardItem* item1 = nullptr;
        QStandardItem* item2 = nullptr;
        QList<QStandardItem*> row;
        item1 = new QStandardItem("SequenceParameterSets");
        item2 = new QStandardItem(QString::number(r->numOfSequenceParameterSets));
        row.append(item1);
        row.append(item2);
        root->appendRow(row);
        
        for (int i = 0; i < r->numOfSequenceParameterSets; i++) {
            row.clear();
            auto spsret = ParseSps(r->sps[i].sequenceParameterSetNALUnit+1, r->sps[i].sequenceParameterSetLength);
            if (spsret.second == 0) {
                
                item2 = new QStandardItem("SequenceParameterSet");
                row.append(item2);
                item1->appendRow(row);
                
                ShowTreeViewOfSPS(item2, spsret.first);
                
                
            }
        }
        return;
    }
}
