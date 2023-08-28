#pragma once

struct AnalysisData
{
    void reset() noexcept
    {
        status = 0;
        peak = 0.0f;
    }

    // 0 = OK, 1 = clipping, 2 = way too loud, 3 = bad mmkay
    std::atomic<int> status = 0;

    std::atomic<float> peak;
};
