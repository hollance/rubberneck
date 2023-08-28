#include <JuceHeader.h>
#include "AnalysisPanel.h"
#include "LookAndFeel.h"
#include "JuceUtils.h"
#include "DSP.h"

static const juce::String statusMessages[] =
{
    "Sound levels OK",
    "Clipping",
    "Too loud (over +6 dB)",
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
    clearButton.setDescription("Reset analysis results");
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

    constexpr int lineHeight = 20;
    int y = 60;

    constexpr int bufSize = 100;
    char s[bufSize];

    if (data.sampleRate > 0.0) {
        std::snprintf(s, bufSize, "Sample rate   %d", int(data.sampleRate));
    } else {
        std::strcpy(s, "Sample rate   unknown");
    }
    g.drawSingleLineText(s, 10, y);

    y += lineHeight;
    if (data.inChannels != -1) {
        std::snprintf(s, bufSize, "Channels      in %d out %d", data.inChannels.load(), data.outChannels.load());
    } else {
        std::strcpy(s, "Channels      unknown");
    }
    g.drawSingleLineText(s, 10, y);

    y += lineHeight;
    if (data.blockSize != -1) {
        std::snprintf(s, bufSize, "Block size    %d", data.blockSize.load());
    } else {
        std::strcpy(s, "Block size    unknown");
    }
    g.drawSingleLineText(s, 10, y);

    float peak = data.peak.load();
    float peakDecibels = -std::numeric_limits<float>::infinity();
    if (std::abs(peak) > 0.0f) {
        peakDecibels = gainToDecibels(std::abs(peak));
    }

    y += lineHeight * 2;
    std::snprintf(s, bufSize, "Peak        % 7.2f dB (%+f)", peakDecibels, peak);
    g.drawSingleLineText(s, 10, y);

    float historySize = float(data.historySize.load());
    float dcOffset = data.dcSum.load() / historySize;
    if (dcOffset > data.dcMax) {
        data.dcMax = dcOffset;
    }
    float dcOffsetDecibels = -std::numeric_limits<float>::infinity();
    if (std::abs(dcOffset) > 0.0f) {
        dcOffsetDecibels = gainToDecibels(std::abs(dcOffset));
    }
    float dcMaxDecibels = -std::numeric_limits<float>::infinity();
    if (data.dcMax > 0.0f) {
        dcMaxDecibels = gainToDecibels(data.dcMax);
    }

    y += lineHeight;
    std::snprintf(s, bufSize, "DC offset   % 7.2f dB (%+f)", dcOffsetDecibels, dcOffset);
    g.drawSingleLineText(s, 10, y);

    y += lineHeight;
    std::snprintf(s, bufSize, "  max       % 7.2f dB (%+f)", dcMaxDecibels, data.dcMax);
    g.drawSingleLineText(s, 10, y);

    // Note: If we only needed decibels, could do `10 * log10(rmsSum / size)`
    // instead of `20 * log10(sqrt(rmsSum / size))`.

    float rms = std::sqrt(data.rmsSum.load() / historySize);
    if (rms > data.rmsMax) {
        data.rmsMax = rms;
    }
    float rmsDecibels = -std::numeric_limits<float>::infinity();
    if (rms > 0.0f) {
        rmsDecibels = gainToDecibels(rms);
    }
    float rmsMaxDecibels = -std::numeric_limits<float>::infinity();
    if (data.rmsMax > 0.0f) {
        rmsMaxDecibels = gainToDecibels(data.rmsMax);
    }

    y += lineHeight;
    std::snprintf(s, bufSize, "RMS         % 7.2f dB (%+f)", rmsDecibels, rms);
    g.drawSingleLineText(s, 10, y);

    y += lineHeight;
    std::snprintf(s, bufSize, "  max       % 7.2f dB (%+f)", rmsMaxDecibels, data.rmsMax);
    g.drawSingleLineText(s, 10, y);
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
