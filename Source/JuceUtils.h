#pragma once

#include <JuceHeader.h>

/**
  Returns a typed pointer to a juce::AudioParameterXXX object from the APVTS.
 */
template<typename T>
inline void castParameter(juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);  // parameter does not exist or wrong type
}

/**
  Draws a 2x image scaled down to 1x. Useful for Retina images.
 */
inline void drawImage(juce::Graphics& g, juce::Image& image, int x, int y, int scale)
{
    auto sW = image.getWidth();
    auto sH = image.getHeight();
    auto dW = sW / scale;
    auto dH = sH / scale;
    g.drawImage(image, x, y, dW, dH, 0, 0, sW, sH);
}
