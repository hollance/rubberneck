#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP.h"
#include "JuceUtils.h"

static juce::String stringFromDecibels(float value, int)
{
    return juce::String(value, 1) + " dB";
}

static juce::String stringFromHz(float value)
{
    if (value < 1000.0f) {
        return juce::String(int(value)) + " Hz";
    } else if (value < 10000.0f) {
        return juce::String(value / 1000.0f, 2) + " k";
    } else {
        return juce::String(value / 1000.0f, 1) + " k";
    }
}

static float hzFromString(const juce::String& str)
{
    float value = str.getFloatValue();
    // TODO: maybe if 0, then figure out if actually zero or input error

    // TODO: if the string ends in "k" or "kHz", then value *= 1000.0f;
    return value;
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
    castParameter(apvts, ParameterID::lowCut, lowCutParam);
    castParameter(apvts, ParameterID::highCut, highCutParam);
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

    auto lowCutStringFromValue = [](float value, int)
    {
        if (value <= 0.0f) { return juce::String("off"); }
        return stringFromHz(value);
    };

    auto lowCutValueFromString = [](const juce::String& str)
    {
        if (str.isEmpty() || str.toLowerCase() == "off") { return 0.0f; }
        return hzFromString(str);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::lowCut,
        "Low Cut",
        juce::NormalisableRange<float>(0.0f, 24000.0f, 1.0f, 0.3f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(lowCutStringFromValue)
            .withValueFromStringFunction(lowCutValueFromString)));

    auto highCutStringFromValue = [](float value, int)
    {
        if (value >= 24000.0f) { return juce::String("off"); }
        return stringFromHz(value);
    };

    auto highCutValueFromString = [](const juce::String& str)
    {
//ptr = str.getCharPointer();
//float value = float(CharacterFunctions::readDoubleValue(ptr));
//  this advances ptr, so maybe I can use this to figure out if it was a proper number

//TODO: really should be anything that's not a number
        if (str.isEmpty() || str.toLowerCase() == "off") { return 24000.0f; }
        return hzFromString(str);
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::highCut,
        "High Cut",
        juce::NormalisableRange<float>(0.0f, 24000.0f, 1.0f, 0.3f),
        24000.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(highCutStringFromValue)
            .withValueFromStringFunction(highCutValueFromString)));

    return layout;
}

void AudioProcessor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
    gainSmoother.reset(sampleRate, 0.02);
    lowCutSmoother.reset(sampleRate, 0.02);
    highCutSmoother.reset(sampleRate, 0.02);
    reset();
}

void AudioProcessor::releaseResources()
{
}

void AudioProcessor::reset()
{
    gainSmoother.setCurrentAndTargetValue(decibelsToGain(gainParam->get()));
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
    highCutSmoother.setCurrentAndTargetValue(highCutParam->get());
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
    invertLeft = invertLeftParam->get();
    invertRight = invertRightParam->get();
    swapChannels = swapChannelsParam->get();
    channels = channelsParam->getIndex();
    protectYourEars = protectYourEarsParam->get();

    gainSmoother.setTargetValue(decibelsToGain(gainParam->get()));
    lowCutSmoother.setTargetValue(lowCutParam->get());
    highCutSmoother.setTargetValue(highCutParam->get());
}

void AudioProcessor::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    lowCut = lowCutSmoother.getNextValue();
    highCut = highCutSmoother.getNextValue();
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

        // TODO: recompute filter coefficients if needed
        // TODO: low cut + high cut

        if (channels == 1) {
            float M = (sampleL + sampleR) * 0.5f;
            sampleL = sampleR = M;
        } else if (channels == 2) {
            float S = (sampleL - sampleR) * 0.5f;
            sampleL = sampleR = S;
        }

        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
    }

    // TODO: this also measures the highest peak (but ignore nan and inf)
//TODO protectyourears (do this in a loop on both channels)
/*
    bool firstWarning = true;
    for (int i = 0; i < sampleCount; ++i) {
        float x = buffer[i];
        bool silence = false;
        if (std::isnan(x)) {
            DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
            silence = true;
        } else if (std::isinf(x)) {
            DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
            silence = true;
        } else if (x < -2.0f || x > 2.0f) {  // screaming feedback
            DBG("!!! WARNING: sample out of range, silencing !!!");
            silence = true;
        } else if (x < -1.0f) {
            if (firstWarning) {
                DBG("!!! WARNING: sample out of range !!!");
                firstWarning = false;
            }
            if (clip) {
                buffer[i] = -1.0f;
            }
        } else if (x > 1.0f) {
            if (firstWarning) {
                DBG("!!! WARNING: sample out of range !!!");
                firstWarning = false;
            }
            if (clip) {
                buffer[i] = 1.0f;
            }
        }
        if (silence) {
            memset(buffer, 0, sampleCount * sizeof(float));
            return;
        }
    }
*/

    // TODO: measure DC offset here
    // TODO: measure RMS here (see KVR real-time RMS calculation best practices)
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
