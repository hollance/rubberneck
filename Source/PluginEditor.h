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

    RotaryKnob crossoverKnob;
    RotaryKnob widthKnob;
    juce::TextButton bypassButton;
    juce::TextButton monoButton;
    TooltipViewer tooltips;

    SliderAttachment crossoverAttachment {
        audioProcessor.apvts, ParameterID::crossover.getParamID(), crossoverKnob.slider
    };
    SliderAttachment widthAttachment {
        audioProcessor.apvts, ParameterID::width.getParamID(), widthKnob.slider
    };
    ButtonAttachment bypassAttachment {
        audioProcessor.apvts, ParameterID::bypass.getParamID(), bypassButton
    };
    ButtonAttachment monoAttachment {
        audioProcessor.apvts, ParameterID::audition.getParamID(), monoButton
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorEditor)
};
