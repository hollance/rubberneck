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
    if (value < 100.0f) {
        auto s = str.trimEnd().toLowerCase();
        if (s.endsWith("k") || s.endsWith("khz")) {
            value *= 1000.0f;
        }
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

    // Force recalculation of the coefficients.
    lastLowCut = -1.0;
    lastHighCut = -1.0;

    lowCutFilter.reset();
    highCutFilter.reset();

    historySize = int(std::ceil(getSampleRate() * 0.3));  // 300 ms
    dcHistory.resize(historySize);
    rmsHistory.resize(historySize);
    std::fill(dcHistory.begin(), dcHistory.end(), 0.0f);
    std::fill(rmsHistory.begin(), rmsHistory.end(), 0.0f);
    historyIndex = 0;
    historyRefresh = 0;
    dcSum = 0.0f;
    rmsSum = 0.0f;

    levelL = 0.0f;
    levelR = 0.0f;
    levelM = 0.0f;
    levelS = 0.0f;
    vuStep = 0;
    vuMax = int(std::ceil(getSampleRate() * 0.01));  // 10 ms

    analysis.reset();
    analysis.historySize = historySize;
}

juce::AudioProcessorParameter* AudioProcessor::getBypassParameter() const
{
    return bypassParam;
}

bool AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono();
}

void AudioProcessor::update() noexcept
{
    bypassed = bypassParam->get();
    invertLeft = invertLeftParam->get();
    invertRight = invertRightParam->get();
    swapChannels = swapChannelsParam->get();
    channels = channelsParam->getIndex();
    protectYourEars = protectYourEarsParam->get();
    mute = muteParam->get();

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

    analysis.sampleRate = getSampleRate();
    analysis.inChannels = numInputChannels;
    analysis.outChannels = numOutputChannels;
    analysis.blockSize = numSamples;

    // Clear any output channels that don't contain input data.
    for (auto i = numInputChannels; i < numOutputChannels; ++i) {
        buffer.clear(i, 0, numSamples);
    }

    update();

    if (bypassed) { return; }

    auto sampleRate = getSampleRate();
    auto nyquist = sampleRate * 0.5;

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

        if (lowCut > 0.0f) {
            lowCut = std::min(lowCut, nyquist - 1.0);
            if (lowCut != lastLowCut) {
                lowCutFilter.highpass(sampleRate, lowCut, 0.70710678);
                lastLowCut = lowCut;
            }
            sampleL = lowCutFilter.processSample(0, sampleL);
            sampleR = lowCutFilter.processSample(1, sampleR);
        }
        if (highCut < 24000.0f) {
            highCut = std::min(highCut, nyquist - 1.0);
            if (highCut != lastHighCut) {
                highCutFilter.lowpass(sampleRate, highCut, 0.70710678);
                lastHighCut = highCut;
            }
            sampleL = highCutFilter.processSample(0, sampleL);
            sampleR = highCutFilter.processSample(1, sampleR);
        }

        float sampleM = (sampleL + sampleR) * 0.5f;
        float sampleS = (sampleL - sampleR) * 0.5f;

        // Measuring the levels here makes most sense to me, so that you can
        // still see everything even if we're only outputting mids or sides.
        if (std::abs(sampleL) > levelL) { levelL = std::abs(sampleL); }
        if (std::abs(sampleR) > levelR) { levelR = std::abs(sampleR); }
        if (std::abs(sampleM) > levelM) { levelM = std::abs(sampleM); }
        if (std::abs(sampleS) > levelS) { levelS = std::abs(sampleS); }

        // Report the highest level every N ms. It's possible that a higher
        // value is overwritten by a lower value if the UI does not read fast
        // enough. I don't think that's a problem here, but could fix this by
        // only updating the atomic when the new level is higher (for example,
        // using compare_exchange_weak).
        vuStep += 1;
        if (vuStep == vuMax) {
            vuStep = 0;
            analysis.levelLeft = levelL;
            analysis.levelRight = levelR;
            analysis.levelMids = levelM;
            analysis.levelSides = levelS;
            levelL = 0.0f;
            levelR = 0.0f;
            levelM = 0.0f;
            levelS = 0.0f;
        }

        if (channels == 1) {         // left
            sampleR = sampleL;
        } else if (channels == 2) {  // right
            sampleL = sampleR;
        } else if (channels == 3) {  // mids
            sampleL = sampleR = sampleM;
        } else if (channels == 4) {  // sides
            sampleL = sampleR = sampleS;
        }

        if (mute) {
            sampleL = 0.0f;
            sampleR = 0.0f;
        }

        channelL[sample] = sampleL;
        channelR[sample] = sampleR;
    }

    /*
        I calculate the RMS / DC offset using a 300 ms circular buffer.

        Alternatively, use a first-order lowpass filter. Simply add the squared
        sample to the filter. When the UI wants to display the RMS, just take
        the square-root of the current value of the filter (since it already
        calculates a smoothed average). Pick a time constant that corresponds
        to 300 ms. Something like this:

            state += (x - state) * coeff;

        See also: https://www.kvraudio.com/forum/viewtopic.php?t=460756
    */

    // Gather statistics. Note that JUCE also has buffer.getMagnitude()
    // and buffer.getRMSLevel(), which may be vector optimized.
    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* data = buffer.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i) {
            float x = data[i];
            if (!std::isnan(x) && !std::isinf(x)) {
                // Measure peak
                float y = std::abs(x);
                if (y > std::abs(analysis.peak)) {
                    analysis.peak = x;
                }

                // Keep running sum for DC offset and RMS.
                dcSum += x;
                rmsSum += x * x;

                // Also store the samples in a circular buffer.
                dcHistory[historyIndex] = x;
                rmsHistory[historyIndex] = x * x;
                historyIndex += 1;
                if (historyIndex == historySize) {
                    historyIndex = 0;
                    historyRefresh += 1;
                }

                // Remove oldest sample from the running sum.
                dcSum -= dcHistory[historyIndex];
                rmsSum -= rmsHistory[historyIndex];
            }
        }
    }

    // To avoid floating point issues, such as an offset from building up,
    // periodically reset the running sum by adding up all the numbers in
    // the circular buffer.
    if (historyRefresh > 10) {
        historyRefresh = 0;
        dcSum = 0.0f;
        rmsSum = 0.0f;
        for (int i = 0; i < historySize; ++i) {
            dcSum += dcHistory[i];
            rmsSum += rmsHistory[i];
        }
    }

    // Defensive programming. Since we subtract values from the running sum,
    // due to floating point imprecision we can end up with a negative sum in
    // theory (as float operations are not commutative), which would be bad.
    if (dcSum < 0.0f) { dcSum = 0.0f; }
    if (rmsSum < 0.0f) { rmsSum = 0.0f; }

    // When the UI wants to display the current value, it reads the sum and
    // divides by N and takes the square root.
    analysis.dcSum = dcSum;
    analysis.rmsSum = rmsSum;

    // Clipping is temporary but we want to be notified without a doubt when
    // there are inf or nan values, so don't overwrite the "holy shit" state.
    if (analysis.status < 3) {
        analysis.status = 0;
    }

    // We still show the current state, even if "Protect Your Ears" is off.
    // The main reason to turn it off is to analyze the actual sound levels
    // using a spectrum analyzer or other tool, so we don't want to clip there.
    bool firstWarning = true;
    for (int channel = 0; channel < numInputChannels; ++channel) {
        float* data = buffer.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i) {
            float x = data[i];
            bool silence = false;
            if (std::isnan(x)) {
                DBG("!!! WARNING: nan detected in audio buffer, silencing !!!");
                silence = true;
                analysis.status = 3;
            } else if (std::isinf(x)) {
                DBG("!!! WARNING: inf detected in audio buffer, silencing !!!");
                silence = true;
                analysis.status = 3;
            } else if (x < -2.0f || x > 2.0f) {
                if (protectYourEars) {
                    DBG("!!! WARNING: sample out of range, silencing !!!");
                    silence = true;
                }
                if (analysis.status < 3) {
                    analysis.status = 2;
                }
            } else if (x < -1.0f) {
                if (firstWarning) {
                    if (protectYourEars) {
                        DBG("!!! WARNING: sample out of range (" << x << ") !!!");
                        firstWarning = false;
                    }
                    if (analysis.status < 3) {
                        analysis.status = 1;
                    }
                }
                if (protectYourEars) {
                    data[i] = -1.0f;
                }
            } else if (x > 1.0f) {
                if (firstWarning) {
                    if (protectYourEars) {
                        DBG("!!! WARNING: sample out of range (" << x << ") !!!");
                        firstWarning = false;
                    }
                    if (analysis.status < 3) {
                        analysis.status = 1;
                    }
                }
                if (protectYourEars) {
                    data[i] = 1.0f;
                }
            }
            if (silence) {
                std::memset(data, 0, numSamples * sizeof(float));
                break;
            }
        }
    }
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
