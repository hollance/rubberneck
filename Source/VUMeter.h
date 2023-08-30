#pragma once

#include <JuceHeader.h>

class VUMeter : public juce::Component, private juce::Timer
{
public:
    VUMeter(std::atomic<float>& levelLeft, std::atomic<float>& levelRight,
            std::atomic<float>& levelMids, std::atomic<float>& levelSides);
    ~VUMeter() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Channel
    {
        float level;
        float leveldB;
        float peak;
        float peakdB;
        int peakHold;
        int x;
    };

    void timerCallback() override;
    void updateChannel(Channel& measurement, std::atomic<float>& value);
    void drawLevel(juce::Graphics& g, Channel& measurement);
    void drawPeak(juce::Graphics& g, Channel& measurement);

    inline int positionForLevel(float dbLevel) const noexcept
    {
        return int(std::round(juce::jmap(dbLevel, maxdB, mindB, minPos, maxPos)));
    }

    std::atomic<float>& levelLeft;
    std::atomic<float>& levelRight;
    std::atomic<float>& levelMids;
    std::atomic<float>& levelSides;

    Channel channels[4];  // L, R, M, S

    static constexpr float maxdB = 12.0f;      // highest dB shown
    static constexpr float mindB = -60.0f;     // lowest dB shown
    static constexpr float clampdB = -120.0f;  // clamp levels to this value
    static constexpr float stepdB = 6.0f;      // draw a tick every 6 dB

    static constexpr int refreshRate = 20;
    static constexpr int holdMax = refreshRate * 2;

    float minPos;          // maxdB line
    float maxPos;          // mindB line
    float positionRed;     // 0 dB line
    float positionYellow;  // -12 dB line

    float attack;          // filter coefficients
    float release;
    float peakDecay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeter)
};
