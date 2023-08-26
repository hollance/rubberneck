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
    bypassButton.setBounds(0, 0, 50, 18);
    bypassButton.setLookAndFeel(ButtonLookAndFeel::get());
    bypassButton.setExplicitFocusOrder(2);
    addAndMakeVisible(bypassButton);

//    monoButton.setTitle("Mono");
//    monoButton.setDescription("Audition the sound as mono");
//    monoButton.setHelpText(monoButton.getDescription());
//    monoButton.setTooltip(monoButton.getHelpText());
//    monoButton.setButtonText("Mono");
//    monoButton.setClickingTogglesState(true);
//    monoButton.setBounds(0, 0, 50, 18);
//    monoButton.setLookAndFeel(ButtonLookAndFeel::get());
//    monoButton.setExplicitFocusOrder(3);
//    addAndMakeVisible(monoButton);

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

//    x += crossoverKnob.getWidth() + 60;
//    widthKnob.setTopLeftPosition(x, y);

    x = bounds.getRight() - bypassButton.getWidth() - 10;
    y = 11;
    bypassButton.setTopLeftPosition(x, y);

//    x -= monoButton.getWidth() + 10;
//    monoButton.setTopLeftPosition(x, y);

    tooltips.setBounds(bounds.withY(bounds.getBottom() - 20).withHeight(20));
}
