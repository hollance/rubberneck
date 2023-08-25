#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP.h"
#include "JuceUtils.h"
#include "ProtectYourEars.h"

static juce::String stringFromPercent(float value, int)
{
    return juce::String(int(value)) + " %";
}

static juce::String stringFromHz(float value, int)
{
    if (value < 100.0f) {
        return juce::String(value, 1) + " Hz";
    } else if (value < 1000.0f) {
        return juce::String(int(value)) + " Hz";
    } else {
        return juce::String(value / 1000.0f, 2) + " k";
    }
}

static float hzFromString(const juce::String& str)
{
    float value = str.getFloatValue();
    if (value > 0.0f && value < 10.0f) {  // kHz
        value *= 1000.0f;
    }
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
    castParameter(apvts, ParameterID::crossover, crossoverParam);
    castParameter(apvts, ParameterID::width, widthParam);
    castParameter(apvts, ParameterID::audition, auditionParam);
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
        ParameterID::crossover,
        "Crossover",
        juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.3f),
        200.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromHz)
            .withValueFromStringFunction(hzFromString)));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::width,
        "Width",
        juce::NormalisableRange<float>(0.0f, 100.0f, 1.0f),
        50.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(stringFromPercent)));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::audition,
        "Audition Mono",
        false));

    return layout;
}

void AudioProcessor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock)
{
    crossoverFilter.prepareToPlay(sampleRate);

    crossoverSmoother.reset(sampleRate, 0.02);
    widthSmoother.reset(sampleRate, 0.02);

    reset();
}

void AudioProcessor::releaseResources()
{
}

void AudioProcessor::reset()
{
    crossoverFilter.reset();

    lastCrossover = -1.0f;  // force update
    crossover = crossoverParam->get();
    crossoverSmoother.setCurrentAndTargetValue(crossover);

    width = widthParam->get() * 0.01f;
    widthSmoother.setCurrentAndTargetValue(width);
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
    crossoverSmoother.setTargetValue(crossoverParam->get());
    widthSmoother.setTargetValue(widthParam->get() * 0.01f);
    audition = auditionParam->get();
}

void AudioProcessor::smoothen() noexcept
{
    crossover = crossoverSmoother.getNextValue();
    width = widthSmoother.getNextValue();
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

    float* channelL = buffer.getWritePointer(0);
    float* channelR = buffer.getWritePointer(1);

    float lowM, highM, lowS, highS;

    for (int sample = 0; sample < numSamples; ++sample) {
        smoothen();

        // Only recalc filter coefficients if parameter has changed.
        if (crossover != lastCrossover) {
            lastCrossover = crossover;
            crossoverFilter.setFrequency(crossover);
        }

        float sampleL = channelL[sample];
        float sampleR = channelR[sample];

        float M = (sampleL + sampleR) * 0.5f;
        float S = (sampleL - sampleR) * 0.5f;

        crossoverFilter.processSample(0, M, lowM, highM);
        crossoverFilter.processSample(1, S, lowS, highS);

        // Bypass still filters the sound, to avoid clicks.

        if (!bypassed) {
            // Only process the sides, keep the mids at the same volume.
            // These multipliers were picked because I liked how they sound.

            if (audition) {
                lowS = highS = 0.0f;  // only keep mids
            } else {
                lowS *= 1.0f - width * 0.9f;   // narrow lows
                highS *= 1.0f + width * 2.0f;  // widen highs
            }
        }

        M = lowM + highM;
        S = lowS + highS;

        channelL[sample] = M + S;
        channelR[sample] = M - S;
    }

    /*
    #ifdef JUCE_DEBUG
    protectYourEars(channelL, numSamples);
    protectYourEars(channelR, numSamples);
    #endif
    */
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
