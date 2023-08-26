#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP.h"
#include "JuceUtils.h"
#include "ProtectYourEars.h"

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

//static juce::String stringFromPercent(float value, int)
//{
//    return juce::String(int(value)) + " %";
//}
//
//static juce::String stringFromHz(float value, int)
//{
//    if (value < 100.0f) {
//        return juce::String(value, 1) + " Hz";
//    } else if (value < 1000.0f) {
//        return juce::String(int(value)) + " Hz";
//    } else {
//        return juce::String(value / 1000.0f, 2) + " k";
//    }
//}
//
//static float hzFromString(const juce::String& str)
//{
//    float value = str.getFloatValue();
//    if (value > 0.0f && value < 10.0f) {  // kHz
//        value *= 1000.0f;
//    }
//    return value;
//}

AudioProcessor::AudioProcessor() :
    juce::AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        )
{
    castParameter(apvts, ParameterID::bypass, bypassParam);
    castParameter(apvts, ParameterID::gain, gainParam);
}

AudioProcessor::~AudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::bypass,
        "Bypass",
        false));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::gain,
        "Gain",
        juce::NormalisableRange<float>(-60.0f, 60.0f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromDecibels)));

    return layout;
}

void AudioProcessor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
    gainSmoother.reset(sampleRate, 0.02);
    reset();
}

void AudioProcessor::releaseResources()
{
}

void AudioProcessor::reset()
{
    gain = decibelsToGain(gainParam->get());
    gainSmoother.setCurrentAndTargetValue(gain);
}

juce::AudioProcessorParameter* AudioProcessor::getBypassParameter() const
{
    return bypassParam;
}

bool AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioProcessor::update() noexcept
{
    bypassed = bypassParam->get();
    gainSmoother.setCurrentAndTargetValue(decibelsToGain(gainParam->get()));
}

void AudioProcessor::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
}

void AudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto numInputChannels = getTotalNumInputChannels();
    auto numOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    // Clear any output channels that don't contain input data.
    for (auto i = numInputChannels; i < numOutputChannels; ++i) {
        buffer.clear(i, 0, numSamples);
    }

    update();

    if (bypassed) { return; }

    bool stereo = numInputChannels > 1;
    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(stereo ? 1 : 0);

    for (int sample = 0; sample < numSamples; ++sample) {
        smoothen();

        float sampleL = channelL[sample];
        float sampleR = channelR[sample];

        sampleL *= gain;
        sampleR *= gain;

        float M = (sampleL + sampleR) * 0.5f;
        float S = (sampleL - sampleR) * 0.5f;

        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
    }

    /**/
    #ifdef JUCE_DEBUG
    protectYourEars(channelL, numSamples);
    protectYourEars(channelR, numSamples);
    #endif
    /**/
}

void AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessorEditor* AudioProcessor::createEditor()
{
    return new AudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioProcessor();
}
