#pragma once

#include <JuceHeader.h>

class AnalysisPanel : public juce::Component
{
public:
    AnalysisPanel();
    ~AnalysisPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalysisPanel)
};
