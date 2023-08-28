#pragma once

struct AnalysisData
{
    void reset() noexcept
    {
        sampleRate = 0.0;
        inChannels = -1;
        outChannels = -1;
        blockSize = -1;
        status = 0;
        peak = 0.0f;
        dcOffset = 0.0f;
        dcOffsetMax = 0.0f;
        rms = 0.0f;
        rmsMax = 0.0f;
    }

    std::atomic<float> sampleRate;
    std::atomic<int> inChannels;
    std::atomic<int> outChannels;
    std::atomic<int> blockSize;

    // 0 = OK, 1 = clipping, 2 = way too loud, 3 = bad mmkay
    std::atomic<int> status = 0;

    // linear units; can be positive or negative
    std::atomic<float> peak;

    std::atomic<float> dcOffset;
    std::atomic<float> dcOffsetMax;
    std::atomic<float> rms;
    std::atomic<float> rmsMax;
};