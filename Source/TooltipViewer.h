#pragma once

#include <JuceHeader.h>

/**
  Displays the tooltip of the component under the mouse.

  Loosely based on https://github.com/Chowdhury-DSP/chowdsp_utils (GPLv3)
  and juce::TooltipWindow.
 */
class TooltipViewer : public juce::Component, private juce::Timer
{
public:
    TooltipViewer();
    void paint(juce::Graphics&) override;

private:
    void timerCallback() override;
    juce::String getTipFor(juce::Component& component);

    juce::String tip;
    juce::Component* component = nullptr;
    std::atomic<bool> showTip { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TooltipViewer)
};
