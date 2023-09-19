#pragma once

#include <JuceHeader.h>
#include <vector>

class SegmentedPicker : public juce::Component, private juce::Button::Listener
{
public:
    SegmentedPicker(juce::AudioParameterChoice& parameter);
    ~SegmentedPicker() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void setLookAndFeel(juce::LookAndFeel* newLookAndFeel);

    int getNumButtons() const noexcept
    {
        return int(buttons.size());
    }

    juce::TextButton* getButtonAt(int index) const noexcept
    {
        return buttons[index].get();
    }

private:
    juce::DropShadow dropShadow;

    void setValue(float newValue);
    void buttonClicked(juce::Button* button) override;

    std::vector<std::unique_ptr<juce::TextButton>> buttons;

    juce::ParameterAttachment attachment;
    bool ignoreCallbacks = false;
    int selectedIndex = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SegmentedPicker)
};
