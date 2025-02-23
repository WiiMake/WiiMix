//
// Created by Владислав Павловский on 25.10.2024.
//

#pragma once

namespace WiiMixOverlay {
    std::vector<u8> getRow(VideoCommon::CustomTextureData::ArraySlice::Level * texture, int row);
    bool displayBingoBoard(std::vector<std::string> filenames);
}