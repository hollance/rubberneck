#pragma once

#include <cmath>

/**
  State variable filter (SVF), designed by Andrew Simper of Cytomic.

  http://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */
class StateVariableFilter
{
public:
    StateVariableFilter() : m0(0.0f), m1(0.0f), m2(0.0f) { }

    void lowpass(float sampleRate, float freq, float Q) noexcept
    {
        setCoefficients(sampleRate, freq, Q);
        m0 = 0.0f;
        m1 = 0.0f;
        m2 = 1.0f;
    }

    void highpass(float sampleRate, float freq, float Q) noexcept
    {
        setCoefficients(sampleRate, freq, Q);
        m0 = 1.0f;
        m1 = -k;
        m2 = -1.0f;
    }

    void reset() noexcept
    {
        ic1eq[0] = 0.0f;
        ic2eq[0] = 0.0f;
        ic1eq[1] = 0.0f;
        ic2eq[1] = 0.0f;
    }

    float processSample(int channel, float v0) noexcept
    {
        float v3 = v0 - ic2eq[channel];
        float v1 = a1 * ic1eq[channel] + a2 * v3;
        float v2 = ic2eq[channel] + a2 * ic1eq[channel] + a3 * v3;
        ic1eq[channel] = 2.0f * v1 - ic1eq[channel];
        ic2eq[channel] = 2.0f * v2 - ic2eq[channel];
        return m0 * v0 + m1 * v1 + m2 * v2;
    }

private:
    void setCoefficients(float sampleRate, float freq, float Q) noexcept
    {
        g = std::tan(3.141592653589793238f * freq / sampleRate);
        k = 1.0f / Q;
        a1 = 1.0f / (1.0f + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    float g, k, a1, a2, a3;    // filter coefficients
    float m0, m1, m2;          // mix coefficients
    float ic1eq[2], ic2eq[2];  // internal state
};
