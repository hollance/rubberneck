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

    drawLevel(g, levelLeft.load(), 0);
    drawLevel(g, levelRight.load(), 16);
    drawLevel(g, levelMids.load(), 50);
    drawLevel(g, levelSides.load(), 66);

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

    int y = bounds.getHeight() - 5;
    g.setFont(Fonts::getFont(14.0f));
    g.setColour(juce::Colour(0, 0, 0));
    g.drawSingleLineText("L", 5, y);
    g.drawSingleLineText("R", 20, y);
    g.drawSingleLineText("M", 54, y);
    g.drawSingleLineText("S", 70, y);
}

void VUMeter::drawLevel(juce::Graphics& g, float level, int x)
{
    if (level > 0.0f) {
        level = std::clamp(gainToDecibels(level), -120.0f, maxdB);

        // This uses the same colors and ranges as Protect Your Ears,
        // which isn't really standard but makes sense for this plug-in.

        if (level > 6.0f) {
            int y1 = positionForLevel(level);
            int y2 = positionRed;
            g.setColour(juce::Colour(226, 74, 81));
            g.fillRect(x, y1, 14, y2 - y1);
            level = 6.0f;
        }
        if (level > 0.0f) {
            int y1 = positionForLevel(level);
            int y2 = positionYellow;
            g.setColour(juce::Colour(234, 224, 24));
            g.fillRect(x, y1, 14, y2 - y1);
            level = 0.0f;
        }

        int y = positionForLevel(level);
        if (y < getHeight()) {
            g.setColour(juce::Colour(65, 206, 88));
            g.fillRect(x, y, 14, getHeight() - y);
        }
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
    repaint();
}
