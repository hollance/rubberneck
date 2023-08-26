#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "RotaryKnob.h"
#include "TooltipViewer.h"

class AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AudioProcessorEditor(AudioProcessor&);
    ~AudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    using APVTS = juce::AudioProcessorValueTreeState;
    using SliderAttachment = APVTS::SliderAttachment;
    using ButtonAttachment = APVTS::ButtonAttachment;

    AudioProcessor& audioProcessor;

    RotaryKnob gainKnob;
    juce::TextButton minus6Button;
    juce::TextButton minus12Button;
    juce::TextButton bypassButton;
    juce::TextButton invertLeftButton;
    juce::TextButton invertRightButton;
    juce::TextButton swapChannelsButton;
    TooltipViewer tooltips;

    SliderAttachment gainAttachment {
        audioProcessor.apvts, ParameterID::gain.getParamID(), gainKnob.slider
    };
    ButtonAttachment bypassAttachment {
        audioProcessor.apvts, ParameterID::bypass.getParamID(), bypassButton
    };
    ButtonAttachment invertLeftAttachment {
        audioProcessor.apvts, ParameterID::invertLeft.getParamID(), invertLeftButton
    };
    ButtonAttachment invertRightAttachment {
        audioProcessor.apvts, ParameterID::invertRight.getParamID(), invertRightButton
    };
    ButtonAttachment swapChannelsAttachment {
        audioProcessor.apvts, ParameterID::swapChannels.getParamID(), swapChannelsButton
    };
//    ComboBoxAttachment comboBoxAttachment {
//        audioProcessor.apvts, ParameterID::channels.getParamID(), channelsPicker
//    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
};
