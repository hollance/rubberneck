#pragma once

#include <JuceHeader.h>
#include "AnalysisData.h"

class AnalysisPanel : public juce::Component, private juce::Timer
{
public:
    AnalysisPanel(AnalysisData& data);

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    juce::TextButton clearButton;
    AnalysisData& data;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisPanel)
};
