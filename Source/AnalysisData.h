#pragma once

struct AnalysisData
{
    void reset() noexcept
    {
        status = 0;
    }

    // 0 = OK, 1 = clipping, 2 = way too loud, 3 = bad mmkay
    std::atomic<int> status = 0;
};
