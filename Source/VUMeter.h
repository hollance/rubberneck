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
    void timerCallback() override;
    void drawLevel(juce::Graphics& g, float level, int x);

    inline int positionForLevel(float dbLevel) const noexcept
    {
        return int(std::round(juce::jmap(dbLevel, maxdB, mindB, minPos, maxPos)));
    }

    std::atomic<float>& levelLeft;
    std::atomic<float>& levelRight;
    std::atomic<float>& levelMids;
    std::atomic<float>& levelSides;

    static constexpr float maxdB = 12.0f;
    static constexpr float mindB = -60.0f;
    static constexpr float stepdB = 6.0f;   // draw a tick every 6 dB

    float minPos;   // maxdB line
    float maxPos;   // mindB line

    float positionRed, positionYellow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeter)
};
