#pragma once

#include <cmath>

/**
  State variable filter (SVF), designed by Andrew Simper of Cytomic.

  http://cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
 */
class StateVariableFilter
{
public:
    StateVariableFilter() : m0(0.0), m1(0.0), m2(0.0) { }

    void lowpass(double sampleRate, double freq, double Q) noexcept
    {
        setCoefficients(sampleRate, freq, Q);
        m0 = 0.0;
        m1 = 0.0;
        m2 = 1.0;
    }

    void highpass(double sampleRate, double freq, double Q) noexcept
    {
        setCoefficients(sampleRate, freq, Q);
        m0 = 1.0;
        m1 = -k;
        m2 = -1.0;
    }

    void reset() noexcept
    {
        ic1eq[0] = 0.0;
        ic2eq[0] = 0.0;
        ic1eq[1] = 0.0;
        ic2eq[1] = 0.0;
    }

    double processSample(int channel, double v0) noexcept
    {
        double v3 = v0 - ic2eq[channel];
        double v1 = a1 * ic1eq[channel] + a2 * v3;
        double v2 = ic2eq[channel] + a2 * ic1eq[channel] + a3 * v3;
        ic1eq[channel] = 2.0 * v1 - ic1eq[channel];
        ic2eq[channel] = 2.0 * v2 - ic2eq[channel];
        return m0 * v0 + m1 * v1 + m2 * v2;
    }

private:
    void setCoefficients(double sampleRate, double freq, double Q) noexcept
    {
        g = std::tan(M_PI * freq / sampleRate);
        k = 1.0 / Q;
        a1 = 1.0 / (1.0 + g * (g + k));
        a2 = g * a1;
        a3 = g * a2;
    }

    double g, k, a1, a2, a3;    // filter coefficients
    double m0, m1, m2;          // mix coefficients
    double ic1eq[2], ic2eq[2];  // internal state
};
