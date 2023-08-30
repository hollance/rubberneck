#include <JuceHeader.h>
#include "VUMeter.h"
#include "LookAndFeel.h"
#include "DSP.h"

VUMeter::VUMeter(std::atomic<float>& levelLeft, std::atomic<float>& levelRight,
                 std::atomic<float>& levelMids, std::atomic<float>& levelSides)
    : levelLeft(levelLeft),
      levelRight(levelRight),
      levelMids(levelMids),
      levelSides(levelSides)
{
    attack = 1.0f - std::exp(-1.0f / (float(refreshRate) * 0.01f));
    release = 1.0f - std::exp(-1.0f / (float(refreshRate) * 0.2f));
    peakDecay = 1.0f - std::exp(-1.0f / (float(refreshRate) * 0.5f));

    int xs[] = { 0, 16, 50, 66 };
    for (int i = 0; i < 4; ++i) {
        channels[i].level = 0.0f;
        channels[i].leveldB = clampdB;
        channels[i].peakdB = clampdB;
        channels[i].peakHold = 0;
        channels[i].x = xs[i];
    }

    setOpaque(true);
    startTimerHz(refreshRate);
}

VUMeter::~VUMeter()
{
}

void VUMeter::paint(juce::Graphics& g)
{
    // This can be optimized by:
    // Not repainting the text every time (Component::setBufferedToImage(true)
    // for the text portions).
    // Only call repaint() to redraw portions that have changed (i.e. if the
    // level didn't change, or not significantly, no need to repaint that bar).
    // Calculate the pixel positions in timerCallback(), not in paint(); the
    // painting code should be as simple as possible.
    // Also we should cache stuff that can be calculated in resized() already.

    const auto bounds = getLocalBounds();

    g.fillAll(Colors::background);

    for (int i = 0; i < 4; ++i) {
        drawLevel(g, channels[i]);
    }

    g.setFont(Fonts::getFont(12.0f));
    for (float db = maxdB; db >= mindB; db -= stepdB) {
        int y = positionForLevel(db);
        if (db == 0.0f) {
            g.setColour(juce::Colour(120, 120, 120));
            g.fillRect(0, y, 30, 1);
            g.fillRect(50, y, 30, 1);
        } else {
            g.setColour(juce::Colour(200, 200, 200));
            g.fillRect(0, y, 30, 1);
            g.fillRect(50, y, 30, 1);
        }
        g.setColour(juce::Colour(80, 80, 80));
        g.drawSingleLineText(juce::String(int(db)), bounds.getCentreX(), y + 4,
                             juce::Justification::horizontallyCentred);
    }

    for (int i = 0; i < 4; ++i) {
        drawPeak(g, channels[i]);
    }

    int y = bounds.getHeight() - 5;
    g.setFont(Fonts::getFont(14.0f));
    g.setColour(juce::Colour(0, 0, 0));
    g.drawSingleLineText("L", 5, y);
    g.drawSingleLineText("R", 20, y);
    g.drawSingleLineText("M", 54, y);
    g.drawSingleLineText("S", 70, y);
}

void VUMeter::drawLevel(juce::Graphics& g, Channel& channel)
{
    if (channel.level > 0.0f) {
        // This uses the same colors and ranges as Protect Your Ears,
        // which isn't really standard but makes sense for this plug-in.

        float leveldB = channel.leveldB;
        int x = channel.x;

        if (leveldB > 6.0f) {
            int y1 = positionForLevel(leveldB);
            int y2 = positionRed;
            g.setColour(juce::Colour(226, 74, 81));
            g.fillRect(x, y1, 14, y2 - y1);
            leveldB = 6.0f;
        }
        if (leveldB > 0.0f) {
            int y1 = positionForLevel(leveldB);
            int y2 = positionYellow;
            g.setColour(juce::Colour(234, 224, 24));
            g.fillRect(x, y1, 14, y2 - y1);
            leveldB = 0.0f;
        }

        int y = positionForLevel(leveldB);
        if (y < getHeight()) {
            g.setColour(juce::Colour(65, 206, 88));
            g.fillRect(x, y, 14, getHeight() - y);
        }
    }
}

void VUMeter::drawPeak(juce::Graphics& g, Channel& channel)
{
    if (channel.peakdB > clampdB) {
        int x = channel.x;
        int y = positionForLevel(channel.peakdB);
        g.setColour(juce::Colour(0, 0, 0));
        g.fillRect(x, y - 1, 14, 3);
    }
}

void VUMeter::resized()
{
    const auto bounds = getLocalBounds();

    minPos = 10.0f;                              // maxdB line
    maxPos = float(bounds.getHeight()) - 30.0f;  // mindB line

    positionRed = positionForLevel(6.0f);
    positionYellow = positionForLevel(0.0f);
}

void VUMeter::timerCallback()
{
    updateChannel(channels[0], levelLeft);
    updateChannel(channels[1], levelRight);
    updateChannel(channels[2], levelMids);
    updateChannel(channels[3], levelSides);

    repaint();
}

void VUMeter::updateChannel(Channel &channel, std::atomic<float>& value)
{
    // Read the current level and immediately set it back to 0. This will
    // cause the animation to decay even when the audio processing stops.
    float newLevel = value.exchange(0.0f);

    // Sometimes attack isn't used and if new level > current level, the level
    // immediately jumps to the new value. (Same as setting attack to 1.0.)
    float coeff = newLevel > channel.level ? attack : release;
    channel.level += coeff * (newLevel - channel.level);

    // Since we applied the filter before conversion to dB, the animation
    // becomes linear in dB space instead of exponential.
    if (channel.level > 0.0f) {
        channel.leveldB = std::clamp(gainToDecibels(channel.level), clampdB, maxdB);
    } else {
        channel.leveldB = clampdB;
    }

    // Peak level. This holds in place for a short while before decaying.
    if (channel.level > channel.peak) {
        channel.peak = channel.level;
        channel.peakdB = channel.leveldB;
        channel.peakHold = holdMax;
    } else if (channel.peakHold > 0) {
        channel.peakHold -= 1;
    } else if (channel.peakdB > clampdB) {
        channel.peak += peakDecay * (channel.level - channel.peak);
        if (channel.peak > 0.0f) {
            channel.peakdB = std::clamp(gainToDecibels(channel.peak), clampdB, maxdB);
        } else {
            channel.peakdB = clampdB;
        }
    }
}
