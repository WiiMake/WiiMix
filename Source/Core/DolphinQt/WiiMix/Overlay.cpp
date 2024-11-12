//
// Created by Владислав Павловский on 25.10.2024.
//
#pragma once
#include "Common/CommonTypes.h"
#include "Common/IOFile.h"
#include "VideoCommon/Assets/CustomTextureData.h"
#include "VideoCommon/OnScreenDisplay.h"
namespace WiiMixOverlay {
    std::vector<u8> getRow(VideoCommon::CustomTextureData::ArraySlice::Level * texture, int row) {
        // returns one buffer row from texture
        std::vector<u8> rowVector;
        int startingPoint = (row - 1) * texture->width * 4;
        rowVector.reserve(texture->width * 4);
        for (int i = 0; i < texture->width * 4; i++) {
            rowVector.push_back(texture->data[startingPoint + i]);
        }
//        for (int i : rowVector) {
//            printf("%d ", i);
//        }
//        printf("\n");
        return rowVector;
    }
    bool displayBingoBoard(std::vector<std::string> filenames) {
        double sqrtSize = sqrt(filenames.size());
        if (pow(floor(sqrtSize), 2) != filenames.size()) {
            std::printf("Files cannot be represented as a matrix, aborting drawing bingo board\n");
            return false;
        }
        std::vector<std::vector<u8>> filesBuffer = std::vector<std::vector<u8>>(filenames.size());
        unsigned long long i = 0;
        // read all files into separate fileBuffers
        for (const std::string &filename: filenames) {
            File::IOFile file;
            file.Open(filename, "rb");
            if (!file.IsGood()) {
                std::printf("File probably doesn't exist, aborting\n");
                return false;
            }
            filesBuffer[i] = std::vector<u8>(file.GetSize());
            file.ReadBytes(filesBuffer[i].data(), file.GetSize());
            i++;
        }
        i = 0;
        // load all textures as RGBA8
        u32 width = 0;
        u32 height = 0;
        std::vector<VideoCommon::CustomTextureData::ArraySlice::Level *> pngTextures = std::vector<VideoCommon::CustomTextureData::ArraySlice::Level *>(
                filenames.size());
        for (std::vector<u8> fileBuffer: filesBuffer) {
            pngTextures[i] = new VideoCommon::CustomTextureData::ArraySlice::Level();
            VideoCommon::LoadPNGTexture(pngTextures[i], filesBuffer[i]);
            if (height == 0) {
                height = pngTextures[i]->height;
                width = pngTextures[i]->width;
            } else if (height != pngTextures[i]->height || width != pngTextures[i]->width) {
                std::printf("Images had different dimensions, aborting drawing bingo board\n");
                return false;
            }
            i++;
        }
        VideoCommon::CustomTextureData::ArraySlice::Level *bingoBoard = new VideoCommon::CustomTextureData::ArraySlice::Level();
        bingoBoard->height = height * sqrtSize + 3 * (sqrtSize - 1);
        bingoBoard->width = width * sqrtSize + 3 * (sqrtSize - 1);
        bingoBoard->data = std::vector<u8>();
        bingoBoard->data.reserve(4 * bingoBoard->width * bingoBoard->height);
        printf("height: %d, width: %d\n", height, width);
        for (int row = 0; row < height * sqrtSize; row++) {
            // example for iconRow: assume size of bingo board is 9. iconRow initially will point to icons 0-2, then 3-5, and then 6-8
            int iconRow = row / pngTextures[0]->height;
            //printf("iconRow: %d, sqrtSize: %d\n", iconRow, (int) sqrtSize);
            for (int horizontalIconRow = 0; horizontalIconRow < sqrtSize; horizontalIconRow++) {
                for (auto value : getRow(pngTextures[iconRow * sqrtSize + horizontalIconRow], row - iconRow * height)) {
                    bingoBoard->data.push_back(value);
                }
                // spacer between columns
                if (horizontalIconRow != sqrtSize - 1) {
                    for (int _ = 0; _ < 4; _++) { bingoBoard->data.push_back(0); }
                    for (int _ = 0; _ < 4; _++) { bingoBoard->data.push_back(255); }
                    for (int _ = 0; _ < 4; _++) { bingoBoard->data.push_back(0); }
                }
            }
            // spacer between rows
            if (row != bingoBoard->height - 1 && row % pngTextures[0]->height == height - 1) {
                for (int _ = 0; _ < bingoBoard->width * 4; _++) { bingoBoard->data.push_back(0); }
                for (int _ = 0; _ < bingoBoard->width * 4; _++) { bingoBoard->data.push_back(255); }
                for (int _ = 0; _ < bingoBoard->width * 4; _++) { bingoBoard->data.push_back(0); }
            }
        }
        // todox: according to the print statements below, size of bingoBoard is incorrect
        //printf("size of bingoBoard: %ld\n", bingoBoard->data.size());
        //printf("size should be: %ld\n", (long) bingoBoard->height * 4 * bingoBoard->width);
        /*
         * uncomment this to print buffer
        for (int i = 0; i < bingoBoard->height; i++) {
            for (int j = 0; j < bingoBoard->width * 4; j++) {
                printf("%-3d ", bingoBoard->data[i * bingoBoard->width * 4 + j]);
            }
            printf("\n");
        }
        */
        OSD::AddMessage("", 100000, OSD::Color::YELLOW, bingoBoard);
        OSD::DrawMessages();
        return true;
    }
}