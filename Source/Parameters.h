#pragma once

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
    PARAMETER_ID(mute)
    PARAMETER_ID(lowCut)
    PARAMETER_ID(highCut)

    #undef PARAMETER_ID
}

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepare(double sampleRate) noexcept;
    void reset() noexcept;
    void update() noexcept;
    void smoothen() noexcept;

    bool bypassed;
    bool invertLeft;
    bool invertRight;
    bool swapChannels;
    bool protectYourEars;
    bool mute;
    int channels;
    float gain;
    double lowCut;
    double highCut;

    juce::AudioParameterBool* bypassParam;
    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterBool* invertLeftParam;
    juce::AudioParameterBool* invertRightParam;
    juce::AudioParameterBool* swapChannelsParam;
    juce::AudioParameterChoice* channelsParam;
    juce::AudioParameterBool* protectYourEarsParam;
    juce::AudioParameterBool* muteParam;
    juce::AudioParameterFloat* lowCutParam;
    juce::AudioParameterFloat* highCutParam;

private:
    juce::AudioProcessorValueTreeState& apvts;

    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<double> lowCutSmoother;
    juce::LinearSmoothedValue<double> highCutSmoother;
};
