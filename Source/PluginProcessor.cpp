#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP.h"
#include "JuceUtils.h"
//#include "ProtectYourEars.h"  //TODO: remove this file from project

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

AudioProcessor::AudioProcessor() :
    juce::AudioProcessor(
        BusesProperties()
            .withInput("Input", juce::AudioChannelSet::stereo(), true)
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        )
{
    castParameter(apvts, ParameterID::bypass, bypassParam);
    castParameter(apvts, ParameterID::gain, gainParam);
    castParameter(apvts, ParameterID::invertLeft, invertLeftParam);
    castParameter(apvts, ParameterID::invertRight, invertRightParam);
    castParameter(apvts, ParameterID::swapChannels, swapChannelsParam);
    castParameter(apvts, ParameterID::channels, channelsParam);
    castParameter(apvts, ParameterID::protectYourEars, protectYourEarsParam);
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

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::invertLeft,
        "Phase Invert Left",
        false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::invertRight,
        "Phase Invert Right",
        false));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::swapChannels,
        "Swap Left and Right",
        false));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterID::channels,
        "Output Channels",
        juce::StringArray({ "All", "Mids", "Sides" }),
        0));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::protectYourEars,
        "Protect Your Ears",
        true));

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
    gainSmoother.setTargetValue(decibelsToGain(gainParam->get()));
    invertLeft = invertLeftParam->get();
    invertRight = invertRightParam->get();
    swapChannels = swapChannelsParam->get();
    channels = channelsParam->getIndex();
    protectYourEars = protectYourEarsParam->get();
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

        if (invertLeft) {
            sampleL = -sampleL;
        }
        if (invertRight) {
            sampleR = -sampleR;
        }
        if (swapChannels) {
            std::swap(sampleL, sampleR);
        }

        if (channels == 1) {
            float M = (sampleL + sampleR) * 0.5f;
            sampleL = sampleR = M;
        } else if (channels == 2) {
            float S = (sampleL - sampleR) * 0.5f;
            sampleL = sampleR = S;
        }

//TODO protectyourears

        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
    }

//TODO: remove
//    protectYourEars(channelL, numSamples);
//    protectYourEars(channelR, numSamples);
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
