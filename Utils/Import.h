//
// Created by rubie on 2025-05-06.
//

#ifndef AUDIOPLAYER_IMPORT_H
#define AUDIOPLAYER_IMPORT_H

#include "spdlog/spdlog.h"
#include "minimp3_ex.h"
class Import {
public :
    mp3dec_ex_t dec;
    std::vector<float> left_wave;
    std::vector<float> right_wave;
    std::vector<float> time;
    int32_t sampleRate;


    bool GetMp3(const std::string filePath = "../TestAudio/Whiplash.mp3");
    void GetInfo();
    void CalculateTime(int total_read,int sampleRate,int channels,int step);
    ~Import();

private:
    const int32_t splitSize = 100000;

};


#endif //AUDIOPLAYER_IMPORT_H
