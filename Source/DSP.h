#pragma once

inline float gainToDecibels(float value)
{
    return 8.6858896381f * std::log(value);  // 20 * log10(x)
}

inline float decibelsToGain(float value)
{
    return std::exp(0.1151292546f * value);  // 10^(x / 20)
}
