#include <JuceHeader.h>
#include "Parameters.h"

template<typename T>
static void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);  // parameter does not exist or wrong type
}

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
    if (value < 100.0f) {
        auto s = str.trimEnd().toLowerCase();
        if (s.endsWith("k") || s.endsWith("khz")) {
            value *= 1000.0f;
        }
    }
    return value;
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts_) : apvts(apvts_)
{
    castParameter(apvts, ParameterID::bypass, bypassParam);
    castParameter(apvts, ParameterID::gain, gainParam);
    castParameter(apvts, ParameterID::invertLeft, invertLeftParam);
    castParameter(apvts, ParameterID::invertRight, invertRightParam);
    castParameter(apvts, ParameterID::swapChannels, swapChannelsParam);
    castParameter(apvts, ParameterID::channels, channelsParam);
    castParameter(apvts, ParameterID::protectYourEars, protectYourEarsParam);
    castParameter(apvts, ParameterID::mute, muteParam);
    castParameter(apvts, ParameterID::lowCut, lowCutParam);
    castParameter(apvts, ParameterID::highCut, highCutParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
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
        juce::StringArray({ "All", "Left", "Right", "Mids", "Sides" }),
        0));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::protectYourEars,
        "Protect Your Ears",
        true));

    layout.add(std::make_unique<juce::AudioParameterBool>(
        ParameterID::mute,
        "Mute Output",
        false));

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

void Parameters::prepare(double sampleRate) noexcept
{
    double smoothTime = 0.02;
    gainSmoother.reset(sampleRate, smoothTime);
    lowCutSmoother.reset(sampleRate, smoothTime);
    highCutSmoother.reset(sampleRate, smoothTime);
}

void Parameters::reset() noexcept
{
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    lowCutSmoother.setCurrentAndTargetValue(lowCutParam->get());
    highCutSmoother.setCurrentAndTargetValue(highCutParam->get());
}

void Parameters::update() noexcept
{
    bypassed = bypassParam->get();
    invertLeft = invertLeftParam->get();
    invertRight = invertRightParam->get();
    swapChannels = swapChannelsParam->get();
    channels = channelsParam->getIndex();
    protectYourEars = protectYourEarsParam->get();
    mute = muteParam->get();

    gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(gainParam->get()));
    lowCutSmoother.setTargetValue(lowCutParam->get());
    highCutSmoother.setTargetValue(highCutParam->get());
}

void Parameters::smoothen() noexcept
{
    gain = gainSmoother.getNextValue();
    lowCut = lowCutSmoother.getNextValue();
    highCut = highCutSmoother.getNextValue();
}
