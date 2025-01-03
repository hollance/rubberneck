#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioProcessorEditor::AudioProcessorEditor(AudioProcessor& p) :
    juce::AudioProcessorEditor(&p),
    audioProcessor(p),
    channelsPicker(*p.params.channelsParam),
    analysisPanel(p.analysis),
    vuMeter(p.analysis.levelLeft, p.analysis.levelRight, p.analysis.levelMids, p.analysis.levelSides)
{
    gainKnob.setLabel("Gain");
    gainKnob.slider.setDescription("Output level adjustment");
    gainKnob.slider.setHelpText(gainKnob.slider.getDescription());
    gainKnob.slider.setTooltip(gainKnob.slider.getHelpText());
    gainKnob.slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    addAndMakeVisible(gainKnob);

    invertLeftButton.setTitle("Phase invert left");
    invertLeftButton.setDescription("Invert the phase of the left output channel");
    invertLeftButton.setHelpText(invertLeftButton.getDescription());
    invertLeftButton.setTooltip(invertLeftButton.getHelpText());
    invertLeftButton.setButtonText(juce::CharPointer_UTF8("ΦL"));
    invertLeftButton.setClickingTogglesState(true);
    invertLeftButton.setBounds(0, 0, 40, 25);
    invertLeftButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(invertLeftButton);

    invertRightButton.setTitle("Phase invert right");
    invertRightButton.setDescription("Invert the phase of the right output channel");
    invertRightButton.setHelpText(invertRightButton.getDescription());
    invertRightButton.setTooltip(invertRightButton.getHelpText());
    invertRightButton.setButtonText(juce::CharPointer_UTF8("ΦR"));
    invertRightButton.setClickingTogglesState(true);
    invertRightButton.setBounds(0, 0, 40, 25);
    invertRightButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(invertRightButton);

    swapChannelsButton.setTitle("Swap channels");
    swapChannelsButton.setDescription("Swap the left and right output channels");
    swapChannelsButton.setHelpText(swapChannelsButton.getDescription());
    swapChannelsButton.setTooltip(swapChannelsButton.getHelpText());
    swapChannelsButton.setButtonText(juce::CharPointer_UTF8("L←→R"));
    swapChannelsButton.setClickingTogglesState(true);
    swapChannelsButton.setBounds(0, 0, 50, 25);
    swapChannelsButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(swapChannelsButton);

    minus6Button.setTitle("-6 dB");
    minus6Button.setDescription("Set the gain to -6 dB");
    minus6Button.setHelpText(minus6Button.getDescription());
    minus6Button.setTooltip(minus6Button.getHelpText());
    minus6Button.setButtonText("6");
    minus6Button.setBounds(0, 0, 20, 20);
    minus6Button.setLookAndFeel(CircleButtonLookAndFeel::get());
    minus6Button.onClick = [this](){
        audioProcessor.params.gainParam->setValueNotifyingHost(audioProcessor.params.gainParam->convertTo0to1(-6.0f));
    };
    addAndMakeVisible(minus6Button);

    minus12Button.setTitle("-12 dB");
    minus12Button.setDescription("Set the gain to -12 dB");
    minus12Button.setHelpText(minus12Button.getDescription());
    minus12Button.setTooltip(minus12Button.getHelpText());
    minus12Button.setButtonText("12");
    minus12Button.setBounds(0, 0, 20, 20);
    minus12Button.setLookAndFeel(CircleButtonLookAndFeel::get());
    minus12Button.onClick = [this](){
        audioProcessor.params.gainParam->setValueNotifyingHost(audioProcessor.params.gainParam->convertTo0to1(-12.0f));
    };
    addAndMakeVisible(minus12Button);

    juce::TextButton* button = channelsPicker.getButtonAt(0);
    button->setButtonText("All");
    button->setDescription("Output stereo sound");
    button->setHelpText(button->getDescription());
    button->setTooltip(button->getHelpText());

    button = channelsPicker.getButtonAt(1);
    button->setButtonText("L");
    button->setDescription("Output left channel only");
    button->setHelpText(button->getDescription());
    button->setTooltip(button->getHelpText());

    button = channelsPicker.getButtonAt(2);
    button->setButtonText("R");
    button->setDescription("Output right channel only");
    button->setHelpText(button->getDescription());
    button->setTooltip(button->getHelpText());

    button = channelsPicker.getButtonAt(3);
    button->setButtonText("M");
    button->setDescription("Output mids only (mono sound)");
    button->setHelpText(button->getDescription());
    button->setTooltip(button->getHelpText());

    button = channelsPicker.getButtonAt(4);
    button->setButtonText("S");
    button->setDescription("Output sides only");
    button->setHelpText(button->getDescription());
    button->setTooltip(button->getHelpText());

    channelsPicker.setBounds(0, 0, 160, 25);
    addAndMakeVisible(channelsPicker);

    lowCutKnob.setLabel("Low Cut");
    lowCutKnob.slider.setDescription("Low cut / high pass filter");
    lowCutKnob.slider.setHelpText(lowCutKnob.slider.getDescription());
    lowCutKnob.slider.setTooltip(lowCutKnob.slider.getHelpText());
    lowCutKnob.slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    lowCutKnob.setSizes(60);
    lowCutKnob.maxInputLength = 6;
    addAndMakeVisible(lowCutKnob);

    highCutKnob.setLabel("High Cut");
    highCutKnob.slider.setDescription("High cut / low pass filter");
    highCutKnob.slider.setHelpText(highCutKnob.slider.getDescription());
    highCutKnob.slider.setTooltip(highCutKnob.slider.getHelpText());
    highCutKnob.slider.setLookAndFeel(RotaryKnobLookAndFeel::get());
    highCutKnob.setSizes(60);
    highCutKnob.maxInputLength = 6;
    addAndMakeVisible(highCutKnob);

    protectYourEarsButton.setTitle("Protect your ears");
    protectYourEarsButton.setDescription("Clip audio when too loud, disable output when screaming feedback detected");
    protectYourEarsButton.setHelpText(protectYourEarsButton.getDescription());
    protectYourEarsButton.setTooltip(protectYourEarsButton.getHelpText());
    protectYourEarsButton.setButtonText("Protect Your Ears");
    protectYourEarsButton.setClickingTogglesState(true);
    protectYourEarsButton.setBounds(0, 0, 160, 25);
    protectYourEarsButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(protectYourEarsButton);

    muteButton.setTitle("Mute Output");
    muteButton.setDescription("Silence all audio output");
    muteButton.setHelpText(muteButton.getDescription());
    muteButton.setTooltip(muteButton.getHelpText());
    muteButton.setButtonText("Mute");
    muteButton.setClickingTogglesState(true);
    muteButton.setBounds(0, 0, 75, 25);
    muteButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(muteButton);

    bypassButton.setTitle("Bypass");
    bypassButton.setDescription("Bypass the effect");
    bypassButton.setHelpText(bypassButton.getDescription());
    bypassButton.setTooltip(bypassButton.getHelpText());
    bypassButton.setButtonText("Bypass");
    bypassButton.setClickingTogglesState(true);
    bypassButton.setBounds(0, 0, 75, 25);
    bypassButton.setLookAndFeel(ButtonLookAndFeel::get());
    addAndMakeVisible(bypassButton);

    addAndMakeVisible(analysisPanel);
    addAndMakeVisible(vuMeter);
    addAndMakeVisible(tooltips);

    setOpaque(true);
    setSize(640, 430);
}

void AudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(Colors::background);
}

void AudioProcessorEditor::resized()
{
    const auto bounds = getLocalBounds();

    int x = 60;
    int y = 10;
    gainKnob.setTopLeftPosition(x, y);

    int y1 = y + 40;
    x = 25;
    minus6Button.setTopLeftPosition(x, y1);

    y1 += minus6Button.getHeight() + 2;
    minus12Button.setTopLeftPosition(x, y1);

    x = 20;
    y += gainKnob.getHeight() + 20;
    invertLeftButton.setTopLeftPosition(x, y);

    x += invertLeftButton.getWidth() + 2;
    invertRightButton.setTopLeftPosition(x, y);

    x += invertRightButton.getWidth() + 30 - 2;
    swapChannelsButton.setTopLeftPosition(x, y);

    x = 20;
    y += swapChannelsButton.getHeight() + 5;
    channelsPicker.setTopLeftPosition(x, y);

    x = 30;
    y += channelsPicker.getHeight() + 20;
    lowCutKnob.setTopLeftPosition(x, y);

    x += lowCutKnob.getWidth() + 20;
    highCutKnob.setTopLeftPosition(x, y);

    x = 20;
    y += lowCutKnob.getHeight() + 20;
    protectYourEarsButton.setTopLeftPosition(x, y);

    y += protectYourEarsButton.getHeight() + 5;
    muteButton.setTopLeftPosition(x, y);

    x += muteButton.getWidth() + 10;
    bypassButton.setTopLeftPosition(x, y);

    analysisPanel.setBounds(200, 18, bounds.getWidth() - 200 - 120, muteButton.getBottom() - 20);
    vuMeter.setBounds(bounds.getWidth() - 100, analysisPanel.getY() - 8, 80, analysisPanel.getHeight() + 8);
    tooltips.setBounds(bounds.withY(bounds.getBottom() - 20).withHeight(20));
}
