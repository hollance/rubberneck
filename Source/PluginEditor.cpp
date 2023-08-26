#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "JuceUtils.h"

AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    gainKnob.setLabel("Gain");
    gainKnob.slider.setDescription("Output level adjustment");
    gainKnob.slider.setHelpText(gainKnob.slider.getDescription());
    gainKnob.slider.setTooltip(gainKnob.slider.getHelpText());
    gainKnob.slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    gainKnob.setColor(Colors::Knob::label);
    gainKnob.setFont(Fonts::getFont());
    gainKnob.setExplicitFocusOrder(0);
    addAndMakeVisible(gainKnob);

    bypassButton.setTitle("Bypass");
    bypassButton.setDescription("Bypass the effect");
    bypassButton.setHelpText(bypassButton.getDescription());
    bypassButton.setTooltip(bypassButton.getHelpText());
    bypassButton.setButtonText("Bypass");
    bypassButton.setClickingTogglesState(true);
    bypassButton.setBounds(0, 0, 50, 19);
    bypassButton.setLookAndFeel(BarButtonLookAndFeel::get());
    addAndMakeVisible(bypassButton);

    invertLeftButton.setTitle("Phase invert left");
    invertLeftButton.setDescription("Invert the phase of the left output channel");
    invertLeftButton.setHelpText(invertLeftButton.getDescription());
    invertLeftButton.setTooltip(invertLeftButton.getHelpText());
    invertLeftButton.setButtonText(juce::CharPointer_UTF8("ΦL"));
    invertLeftButton.setClickingTogglesState(true);
    invertLeftButton.setBounds(0, 0, 40, 30);
    invertLeftButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(invertLeftButton);

    invertRightButton.setTitle("Phase invert right");
    invertRightButton.setDescription("Invert the phase of the right output channel");
    invertRightButton.setHelpText(invertRightButton.getDescription());
    invertRightButton.setTooltip(invertRightButton.getHelpText());
    invertRightButton.setButtonText(juce::CharPointer_UTF8("ΦR"));
    invertRightButton.setClickingTogglesState(true);
    invertRightButton.setBounds(0, 0, 40, 30);
    invertRightButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(invertRightButton);

    swapChannelsButton.setTitle("Swap channels");
    swapChannelsButton.setDescription("Swap the left and right output channels");
    swapChannelsButton.setHelpText(swapChannelsButton.getDescription());
    swapChannelsButton.setTooltip(swapChannelsButton.getHelpText());
    swapChannelsButton.setButtonText(juce::CharPointer_UTF8("L←→R"));
    swapChannelsButton.setClickingTogglesState(true);
    swapChannelsButton.setBounds(0, 0, 50, 30);
    swapChannelsButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(swapChannelsButton);

    addAndMakeVisible(tooltips);

    setOpaque(true);
    setSize(600, 400);
}

AudioProcessorEditor::~AudioProcessorEditor()
{
}

void AudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    auto rect = bounds.withHeight(40);

    g.fillAll(Colors::background);

    g.setColour(Colors::Knob::trackActive);
    g.fillRect(rect);

    auto image = juce::ImageCache::getFromMemory(BinaryData::Logo_png, BinaryData::Logo_pngSize);
    drawImage(g, image, 0, 0, 2);
}

void AudioProcessorEditor::resized()
{
    const auto bounds = getLocalBounds();

    int x = 20;
    int y = 50;
    gainKnob.setTopLeftPosition(x, y);

    y += gainKnob.getHeight() + 20;
    invertLeftButton.setTopLeftPosition(x, y);

    x += invertLeftButton.getWidth() + 2;
    invertRightButton.setTopLeftPosition(x, y);

    x += invertRightButton.getWidth() + 2;
    swapChannelsButton.setTopLeftPosition(x, y);

    x = bounds.getRight() - bypassButton.getWidth() - 10;
    y = 11;
    bypassButton.setTopLeftPosition(x, y);

    tooltips.setBounds(bounds.withY(bounds.getBottom() - 20).withHeight(20));
}
