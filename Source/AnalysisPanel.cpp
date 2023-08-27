#include <JuceHeader.h>
#include "AnalysisPanel.h"
#include "LookAndFeel.h"
#include "JuceUtils.h"

AnalysisPanel::AnalysisPanel()
{
    setOpaque(true);
}

AnalysisPanel::~AnalysisPanel()
{
}

void AnalysisPanel::paint (juce::Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();

    g.fillAll(Colors::background);

    g.setColour(Colors::Analysis::background);
    g.fillRoundedRectangle(bounds, 8.0f);

    juce::Path path;
    path.addRoundedRectangle(bounds, 8.0f);
    juce::DropShadow dropShadow(Colors::Analysis::dropShadow, 8, {0, 4});
    drawInnerShadow(g, path, dropShadow);
}

void AnalysisPanel::resized()
{
}
