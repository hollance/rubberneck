#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "JuceUtils.h"

AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p)
    : juce::AudioProcessorEditor(&p), audioProcessor(p)
{
    widthKnob.setLabel("Width");
    widthKnob.slider.setDescription("Amount of widening to add");
    widthKnob.slider.setHelpText(widthKnob.slider.getDescription());
    widthKnob.slider.setTooltip(widthKnob.slider.getHelpText());
    widthKnob.slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    widthKnob.setColor(Colors::Knob::label);
    widthKnob.setFont(Fonts::getFont());
    widthKnob.setExplicitFocusOrder(0);
    addAndMakeVisible(widthKnob);

    crossoverKnob.setLabel("Crossover");
    crossoverKnob.slider.setDescription("Sounds above crossover frequency are widened");
    crossoverKnob.slider.setHelpText(crossoverKnob.slider.getDescription());
    crossoverKnob.slider.setTooltip(crossoverKnob.slider.getHelpText());
    crossoverKnob.slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    crossoverKnob.setColor(Colors::Knob::label);
    crossoverKnob.setFont(Fonts::getFont());
    crossoverKnob.setExplicitFocusOrder(1);
    addAndMakeVisible(crossoverKnob);

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

    monoButton.setTitle("Mono");
    monoButton.setDescription("Audition the sound as mono");
    monoButton.setHelpText(monoButton.getDescription());
    monoButton.setTooltip(monoButton.getHelpText());
    monoButton.setButtonText("Mono");
    monoButton.setClickingTogglesState(true);
    monoButton.setBounds(0, 0, 50, 18);
    monoButton.setLookAndFeel(ButtonLookAndFeel::get());
    monoButton.setExplicitFocusOrder(3);
    addAndMakeVisible(monoButton);

    addAndMakeVisible(tooltips);

    setOpaque(true);
    setSize(320, 240);
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

    int x = 50;
    int y = 72;
    crossoverKnob.setTopLeftPosition(x, y);

    x += crossoverKnob.getWidth() + 60;
    widthKnob.setTopLeftPosition(x, y);

    x = bounds.getRight() - bypassButton.getWidth() - 10;
    y = 11;
    bypassButton.setTopLeftPosition(x, y);

    x -= monoButton.getWidth() + 10;
    monoButton.setTopLeftPosition(x, y);

    tooltips.setBounds(bounds.withY(bounds.getBottom() - 20).withHeight(20));
}
