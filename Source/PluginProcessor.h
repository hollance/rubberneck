#pragma once

#include <JuceHeader.h>
#include "StateVariableFilter.h"

namespace ParameterID
{
    #define PARAMETER_ID(str) const juce::ParameterID str(#str, 1);

    PARAMETER_ID(bypass)
    PARAMETER_ID(gain)
    PARAMETER_ID(invertLeft)
    PARAMETER_ID(invertRight)
    PARAMETER_ID(swapChannels)
    PARAMETER_ID(channels)
    PARAMETER_ID(protectYourEars)
    PARAMETER_ID(lowCut)
    PARAMETER_ID(highCut)

    #undef PARAMETER_ID
}

class AudioProcessor : public juce::AudioProcessor
{
public:
    AudioProcessor();
    ~AudioProcessor() override;

    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override { }
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override { }

    juce::AudioProcessorParameter* getBypassParameter() const override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    juce::AudioProcessorEditor* createEditor() override;

    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterChoice* channelsParam;
    juce::AudioParameterFloat* lowCutParam;
    juce::AudioParameterFloat* highCutParam;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void update() noexcept;
    void smoothen() noexcept;

    juce::AudioParameterBool* bypassParam;
    juce::AudioParameterBool* invertLeftParam;
    juce::AudioParameterBool* invertRightParam;
    juce::AudioParameterBool* swapChannelsParam;
    juce::AudioParameterBool* protectYourEarsParam;

    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<double> lowCutSmoother;
    juce::LinearSmoothedValue<double> highCutSmoother;

    bool bypassed;
    bool invertLeft;
    bool invertRight;
    bool swapChannels;
    bool protectYourEars;
    float gain;
    double lowCut, lastLowCut;
    double highCut, lastHighCut;
    int channels;

    StateVariableFilter lowCutFilter, highCutFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
};
