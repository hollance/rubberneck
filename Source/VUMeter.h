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

    std::atomic<float>& levelLeft;
    std::atomic<float>& levelRight;
    std::atomic<float>& levelMids;
    std::atomic<float>& levelSides;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VUMeter)
};
