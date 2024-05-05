#pragma once

#include <JuceHeader.h>
#include "AnalysisData.h"
#include "Parameters.h"
#include "StateVariableFilter.h"

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

    juce::AudioProcessorValueTreeState apvts;
    Parameters params;
    AnalysisData analysis;

private:
    double lastLowCut;
    double lastHighCut;

    int historyIndex;
    int historySize;
    int historyRefresh;
    float dcSum;
    float rmsSum;
    std::vector<float> dcHistory;
    std::vector<float> rmsHistory;

    float levelL;
    float levelR;
    float levelM;
    float levelS;
    int vuStep, vuMax;

    StateVariableFilter lowCutFilter, highCutFilter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessor)
};
