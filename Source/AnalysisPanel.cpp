#include <JuceHeader.h>
#include "AnalysisPanel.h"
#include "LookAndFeel.h"
#include "JuceUtils.h"

static const juce::String statusMessages[] =
{
    "Sound levels OK",
    "Clipping",
    "Too loud (more than +6 dB)",
    "!!! Inf or NaN !!!",
};

static const juce::Colour statusColors[] =
{
    { 65, 206, 88 },
    { 234, 224, 24 },
    { 226, 74, 81 },
    { 226, 74, 81 },
};

AnalysisPanel::AnalysisPanel(AnalysisData& data) : data(data)
{
    clearButton.setTitle("Clear");
    clearButton.setDescription("Reset the analysis results");
    clearButton.setHelpText(clearButton.getDescription());
    clearButton.setTooltip(clearButton.getHelpText());
    clearButton.setButtonText("Clear");
    clearButton.setBounds(0, 0, 80, 30);
    clearButton.setLookAndFeel(ButtonLookAndFeel::get());
    clearButton.onClick = [&data](){
        data.reset();
    };
    addAndMakeVisible(clearButton);

    setOpaque(true);
    startTimerHz(30);
}

AnalysisPanel::~AnalysisPanel()
{
}

void AnalysisPanel::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    g.fillAll(Colors::background);

    g.setColour(Colors::Analysis::background);
    g.fillRoundedRectangle(bounds, 8.0f);

    juce::Path path;
    path.addRoundedRectangle(bounds, 8.0f);
    juce::DropShadow dropShadow(Colors::Analysis::dropShadow, 8, {0, 4});
    drawInnerShadow(g, path, dropShadow);

    int status = data.status.load();
    g.setColour(statusColors[status]);
    g.fillRoundedRectangle(10, 10, bounds.getWidth() - 20, 20, 3);

    g.setFont(Fonts::getFont());
    g.setColour(juce::Colours::black);
    g.drawSingleLineText(statusMessages[status], bounds.getWidth() / 2, 24, juce::Justification::horizontallyCentred);
}

void AnalysisPanel::resized()
{
    const auto bounds = getLocalBounds();

    clearButton.setTopRightPosition(bounds.getRight() - 8, bounds.getBottom() - clearButton.getHeight() - 5);
}

void AnalysisPanel::timerCallback()
{
    repaint();
}
