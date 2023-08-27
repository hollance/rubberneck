#include <JuceHeader.h>
#include "SegmentedPicker.h"
#include "LookAndFeel.h"

/*
    TODO: make generic

    - make N buttons (based on the parameter)
    - put these into a std::vector<unique_ptr<juce::TextButton>>
        - do we need that unique_ptr? (note that we should be the owner,
            Component only stores a raw pointer to the child component)
    - caller can do getButton(idx) to get a reference to the button and configure it
    - resized: automatically position the buttons
*/

SegmentedPicker::SegmentedPicker(juce::AudioParameterChoice& parameter) :
    dropShadow(Colors::Knob::dropShadow, 4, { 0, 2 }),
    attachment(parameter, [this](float f){ setValue(f); })
{
    allButton.setButtonText("All");
    allButton.setDescription("Output stereo sound");
    allButton.setHelpText(allButton.getDescription());
    allButton.setTooltip(allButton.getHelpText());
    allButton.setClickingTogglesState(true);
    allButton.setConnectedEdges(juce::Button::ConnectedOnRight);
    allButton.setLookAndFeel(PickerButtonLookAndFeel::get());
    addAndMakeVisible(allButton);

    midsButton.setButtonText("Mids");
    midsButton.setDescription("Output mids only (mono sound)");
    midsButton.setHelpText(midsButton.getDescription());
    midsButton.setTooltip(midsButton.getHelpText());
    midsButton.setClickingTogglesState(true);
    midsButton.setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    midsButton.setLookAndFeel(PickerButtonLookAndFeel::get());
    addAndMakeVisible(midsButton);

    sidesButton.setButtonText("Sides");
    sidesButton.setDescription("Output sides only");
    sidesButton.setHelpText(sidesButton.getDescription());
    sidesButton.setTooltip(sidesButton.getHelpText());
    sidesButton.setClickingTogglesState(true);
    sidesButton.setConnectedEdges(juce::Button::ConnectedOnLeft);
    sidesButton.setLookAndFeel(PickerButtonLookAndFeel::get());
    addAndMakeVisible(sidesButton);

    attachment.sendInitialUpdate();

    allButton.addListener(this);
    midsButton.addListener(this);
    sidesButton.addListener(this);
}

SegmentedPicker::~SegmentedPicker()
{
    allButton.removeListener(this);
    midsButton.removeListener(this);
    sidesButton.removeListener(this);
}

void SegmentedPicker::setLookAndFeel(juce::LookAndFeel* newLookAndFeel)
{
    allButton.setLookAndFeel(newLookAndFeel);
    midsButton.setLookAndFeel(newLookAndFeel);
    sidesButton.setLookAndFeel(newLookAndFeel);
}

void SegmentedPicker::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto cornerSize = bounds.getHeight() * 0.25f;
    auto buttonRect = bounds.reduced(4.0f, 2.0f).withTrimmedBottom(4.0f);

    auto path = juce::Path();
    path.addRoundedRectangle(buttonRect, cornerSize, cornerSize);
    dropShadow.drawForPath(g, path);

    g.setColour(Colors::Knob::outline);
    g.fillRoundedRectangle(buttonRect, cornerSize);

    auto innerRect = buttonRect.reduced(1.0f);
    g.setGradientFill(juce::ColourGradient(
        Colors::Knob::gradientTop, 0.0f, innerRect.getY(),
        Colors::Knob::gradientBottom, 0.0f, innerRect.getBottom(), false));
    g.fillRoundedRectangle(innerRect, cornerSize - 1.0f);

    g.setColour(juce::Colours::black.withAlpha(0.1f));
    g.fillRect(float(midsButton.getX()) - 1.0f, innerRect.getY(), 1.0f, innerRect.getHeight());
    g.fillRect(float(sidesButton.getX()) - 1.0f, innerRect.getY(), 1.0f, innerRect.getHeight());

    g.setColour(Colors::Knob::outline);
    g.fillRect(float(midsButton.getX()), innerRect.getY(), 1.0f, innerRect.getHeight());
    g.fillRect(float(sidesButton.getX()), innerRect.getY(), 1.0f, innerRect.getHeight());

    juce::Rectangle<float> buttonBounds;
    if (allButton.getToggleState()) {
        buttonBounds = allButton.getBounds().toFloat();
    } else if (midsButton.getToggleState()) {
        buttonBounds = midsButton.getBounds().toFloat();
    } else {
        buttonBounds = sidesButton.getBounds().toFloat();
    }
    auto buttonInnerRect = buttonBounds.reduced(0.0f, 3.0f).withTrimmedBottom(4.0f);
    auto toggleRect = buttonInnerRect.withY(buttonInnerRect.getBottom() - 3.0f)
        .withHeight(2.0f).withTrimmedLeft(6.0f).withTrimmedRight(6.0f);
    g.setColour(Colors::Knob::trackActive);
    g.fillRoundedRectangle(toggleRect, 1.0f);
}

void SegmentedPicker::resized()
{
    auto margin = 5.0f;
    auto width = float(getWidth());
    auto segmentWidth = (width - 2.0f * margin) / 3.0f;

    int x1 = int(margin);
    int x2 = int(margin + segmentWidth);
    int x3 = int(margin + segmentWidth * 2.0f);
    int x4 = int(width - margin);

    allButton.setBounds(x1, 0, x2 - x1, getHeight());
    midsButton.setBounds(x2, 0, x3 - x2, getHeight());
    sidesButton.setBounds(x3, 0, x4 - x3, getHeight());
}

void SegmentedPicker::setValue(float newValue)
{
    const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);
    allButton.setToggleState(newValue == 0.0f, juce::sendNotificationSync);
    midsButton.setToggleState(newValue == 1.0f, juce::sendNotificationSync);
    sidesButton.setToggleState(newValue == 2.0f, juce::sendNotificationSync);
}

void SegmentedPicker::buttonClicked(juce::Button* button)
{
    if (ignoreCallbacks) { return; }

    float newValue = 0.0f;
    if (button == &midsButton && midsButton.getToggleState()) {
        newValue = 1.0f;
    }
    if (button == &sidesButton && sidesButton.getToggleState()) {
        newValue = 2.0f;
    }
    attachment.setValueAsCompleteGesture(newValue);
}
