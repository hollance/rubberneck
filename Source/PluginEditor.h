#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "RotaryKnob.h"
#include "SegmentedPicker.h"
#include "AnalysisPanel.h"
#include "VUMeter.h"
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
    RotaryKnob lowCutKnob;
    RotaryKnob highCutKnob;
    juce::TextButton minus6Button;
    juce::TextButton minus12Button;
    juce::TextButton bypassButton;
    juce::TextButton invertLeftButton;
    juce::TextButton invertRightButton;
    juce::TextButton swapChannelsButton;
    juce::TextButton protectYourEarsButton;
    SegmentedPicker channelsPicker;
    AnalysisPanel analysisPanel;
    VUMeter vuMeter;
    TooltipViewer tooltips;

    SliderAttachment gainAttachment {
        audioProcessor.apvts, ParameterID::gain.getParamID(), gainKnob.slider
    };
    SliderAttachment lowCutAttachment {
        audioProcessor.apvts, ParameterID::lowCut.getParamID(), lowCutKnob.slider
    };
    SliderAttachment highCutAttachment {
        audioProcessor.apvts, ParameterID::highCut.getParamID(), highCutKnob.slider
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
    ButtonAttachment protectYourEarsAttachment {
        audioProcessor.apvts, ParameterID::protectYourEars.getParamID(), protectYourEarsButton
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
};
