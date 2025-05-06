//
// Created by rubie on 2025-05-06.
//
#define MINIMP3_IMPLEMENTATION
#define NOMINMAX  // Windows 헤더의 max, min 매크로 정의 방지

#include "minimp3_ex.h"
#include "Import.h"
#include <algorithm>

bool Import::GetMp3(const std::string filePath) {
    left_wave.clear();
    right_wave.clear();
    if (mp3dec_ex_open(&dec, filePath.c_str(), MP3D_SEEK_TO_SAMPLE)) {
        spdlog::warn("{} => MP3 파일 열기 실패 \n", filePath);
        return false;
    }
    sampleRate = dec.info.hz;


    std::vector<int16_t> pcm_buffer;
    const int chunk_size = 4096;
    int16_t temp[chunk_size];

    int total_read = 0;
    while(true){
        auto read = mp3dec_ex_read(&dec,temp,chunk_size);
        if(read <= 0) break;

        pcm_buffer.insert(pcm_buffer.end(),temp,temp + read);
        total_read += read;
    };

    int step = std::max(1,total_read/splitSize);

    if(dec.info.channels == 2){
        for (int i = 0; i < total_read; i += step * 2) {
            left_wave.push_back(static_cast<float>((pcm_buffer[i])     / 32768.0f) + 2.0f);     // L
            right_wave.push_back(static_cast<float>(pcm_buffer[i + 1]) / 32768.0f);    // R
        }
    }
    CalculateTime(total_read,sampleRate,dec.info.channels,step);

    return true;
}

void Import::CalculateTime(int total_read,int sampleRate,int channels,int step) {
    time.clear();  // 기존 데이터 초기화
    int length = total_read / channels;
    time.reserve(length);
    for (size_t i = 0; i < length; i += step) {
        time.push_back(static_cast<float>(i) / sampleRate);
    }
}
void Import::GetInfo() {
    printf("샘플레이트: %d\n", dec.info.hz);
    printf("채널 수: %d\n", dec.info.channels);
    printf("전체 샘플 수: %lld\n", dec.samples);
}

Import::~Import() {
    // PCM 데이터는 dec.buffer에 있음 (int16_t*)
    mp3dec_ex_close(&dec);
}