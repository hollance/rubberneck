#include <JuceHeader.h>
#include "VUMeter.h"
#include "LookAndFeel.h"
#include "DSP.h"

VUMeter::VUMeter(std::atomic<float>& levelLeft, std::atomic<float>& levelRight,
                 std::atomic<float>& levelMids, std::atomic<float>& levelSides)
    : levelLeft(levelLeft), levelRight(levelRight), levelMids(levelMids), levelSides(levelSides)
{
    startTimerHz(60);
}

VUMeter::~VUMeter()
{
}

void VUMeter::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.fillAll(Colors::background);

//    g.setColour(juce::Colours::white);
//    g.fillRect(0, 0, 30, getHeight());
//    g.fillRect(50, 0, 30, getHeight());

    g.setFont(Fonts::getFont(12.0f));

    constexpr float maxdB = 6.0f;
    constexpr float mindB = -60.0f;
    constexpr float stepdB = 6.0f;   // draw a tick every 6 dB

//TODO: put this stuff in resized()
    float minPos = 10.0f;                              // maxdB line
    float maxPos = float(bounds.getHeight()) - 30.0f;  // mindB line
    float pixelsPerDb = (maxPos - minPos) / (maxdB - mindB);

    //TODO: bunch of duplicated code here, could put in helper method

    float levelL = levelLeft.load();
    if (levelL > 0.0f) {
        levelL = std::clamp(gainToDecibels(levelL), -120.0f, maxdB);

        int y = int(std::round(juce::jmap(levelL, maxdB, mindB, minPos, maxPos)));
        if (y < getHeight()) {
            g.setColour(juce::Colour(65, 206, 88));
            g.fillRect(0, y, 14, getHeight() - y);
        }
    }

    float levelR = levelRight.load();
    if (levelR > 0.0f) {
        levelR = std::clamp(gainToDecibels(levelR), -120.0f, maxdB);

        int y = int(std::round(juce::jmap(levelR, maxdB, mindB, minPos, maxPos)));
        if (y < getHeight()) {
            g.setColour(juce::Colour(65, 206, 88));
            g.fillRect(16, y, 14, getHeight() - y);
        }
    }

    float levelM = levelMids.load();
    if (levelM > 0.0f) {
        levelM = std::clamp(gainToDecibels(levelM), -120.0f, maxdB);

        int y = int(std::round(juce::jmap(levelM, maxdB, mindB, minPos, maxPos)));
        if (y < getHeight()) {
            g.setColour(juce::Colour(65, 206, 88));
            g.fillRect(50, y, 14, getHeight() - y);
        }
    }

    float levelS = levelSides.load();
    if (levelS > 0.0f) {
        levelS = std::clamp(gainToDecibels(levelS), -120.0f, maxdB);

        int y = int(std::round(juce::jmap(levelS, maxdB, mindB, minPos, maxPos)));
        if (y < getHeight()) {
            g.setColour(juce::Colour(65, 206, 88));
            g.fillRect(66, y, 14, getHeight() - y);
        }
    }

    for (float db = maxdB; db >= mindB; db -= stepdB) {
//TODO: put this in a helper function (inline)
        int y = int(std::round(juce::jmap(db, maxdB, mindB, minPos, maxPos)));

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
        g.drawSingleLineText(juce::String(int(db)), bounds.getCentreX(), y + 5, juce::Justification::horizontallyCentred);
    }

    int y = bounds.getHeight() - 5;
    g.setFont(Fonts::getFont(14.0f));
    g.setColour(juce::Colour(0, 0, 0));
    g.drawSingleLineText("L", 5, y);
    g.drawSingleLineText("R", 20, y);
    g.drawSingleLineText("M", 55, y);
    g.drawSingleLineText("S", 70, y);

// actually detect these levels in the audio code
// also levelMids and levelSides
// green, yellow, red colors
// ballistics filter
}

void VUMeter::resized()
{
}

void VUMeter::timerCallback()
{
    repaint();
}
