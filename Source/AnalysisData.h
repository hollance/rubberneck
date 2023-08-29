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
        levelLeft = 0.0f;
        levelRight = 0.0f;
        levelMids = 0.0f;
        levelSides = 0.0f;
        dcSum = 0.0f;
        dcMax = 0.0f;
        rmsSum = 0.0f;
        rmsMax = 0.0f;

        // Note: This does not reset the RMS and DC offset history buffers.
        // Could tell the audio thread to do this using an atomic bool, but
        // it's not very important.
    }

    std::atomic<float> sampleRate;
    std::atomic<int> inChannels;
    std::atomic<int> outChannels;
    std::atomic<int> blockSize;

    // 0 = OK, 1 = clipping, 2 = way too loud, 3 = bad mmkay
    std::atomic<int> status = 0;

    // linear units; can be positive or negative
    std::atomic<float> peak;

    // VU meter
    std::atomic<float> levelLeft, levelRight;
    std::atomic<float> levelMids, levelSides;

    // RMS and DC offset
    std::atomic<int> historySize;
    std::atomic<float> dcSum;
    std::atomic<float> rmsSum;
    float dcMax;
    float rmsMax;
};
