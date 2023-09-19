#include <JuceHeader.h>
#include "SegmentedPicker.h"
#include "LookAndFeel.h"

SegmentedPicker::SegmentedPicker(juce::AudioParameterChoice& parameter) :
    dropShadow(Colors::Knob::dropShadow, 4, { 0, 2 }),
    attachment(parameter, [this](float f){ setValue(f); })
{
    for (int i = 0; i < parameter.choices.size(); ++i) {
        int edges = 0;
        if (i > 0) {
            edges |= juce::Button::ConnectedOnLeft;
        }
        if (i < parameter.choices.size() - 1) {
            edges |= juce::Button::ConnectedOnRight;
        }

        auto button = std::make_unique<juce::TextButton>();
        button->setButtonText(parameter.choices[i]);
        button->setClickingTogglesState(false);
        button->setConnectedEdges(edges);
        button->setLookAndFeel(PickerButtonLookAndFeel::get());
        button->addListener(this);
        addAndMakeVisible(*button);

        buttons.emplace_back(std::move(button));
    }

    attachment.sendInitialUpdate();
}

SegmentedPicker::~SegmentedPicker()
{
    for (auto& button : buttons) {
        button->removeListener(this);
    }
}

void SegmentedPicker::setLookAndFeel(juce::LookAndFeel* newLookAndFeel)
{
    for (auto& button : buttons) {
        button->setLookAndFeel(newLookAndFeel);
    }
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
    for (int i = 1; i < getNumButtons(); ++i) {
        g.fillRect(float(buttons[i]->getX()) - 1.0f, innerRect.getY(), 1.0f, innerRect.getHeight());
    }

    g.setColour(Colors::Knob::outline);
    for (int i = 1; i < getNumButtons(); ++i) {
        g.fillRect(float(buttons[i]->getX()), innerRect.getY(), 1.0f, innerRect.getHeight());
    }

    auto buttonBounds = buttons[selectedIndex]->getBounds().toFloat();
    auto buttonInnerRect = buttonBounds.reduced(0.0f, 3.0f).withTrimmedBottom(4.0f);
    auto toggleRect = buttonInnerRect.withY(buttonInnerRect.getBottom() - 3.0f)
        .withHeight(2.0f).withTrimmedLeft(6.0f).withTrimmedRight(6.0f);
    g.setColour(Colors::Knob::trackActive);
    g.fillRoundedRectangle(toggleRect, 1.0f);
}

void SegmentedPicker::resized()
{
    int numButtons = getNumButtons();

    float margin = 5.0f;
    float width = float(getWidth());
    float segmentWidth = (width - 2.0f * margin) / float(numButtons);

    int xa = int(margin);
    int xb;

    for (int i = 0; i < numButtons; ++i) {
        if (i == numButtons - 1) {
            xb = int(width - margin);
        } else {
            xb = int(margin + segmentWidth * float(i + 1));
        }
        buttons[i]->setBounds(xa, 0, xb - xa, getHeight());
        xa = xb;
    }
}

void SegmentedPicker::setValue(float newValue)
{
    const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);

    selectedIndex = int(newValue);
    repaint();
}

void SegmentedPicker::buttonClicked(juce::Button* button)
{
    if (ignoreCallbacks) { return; }

    for (int i = 0; i < getNumButtons(); ++i) {
        if (button == buttons[i].get()) {
            attachment.setValueAsCompleteGesture(float(i));
            break;
        }
    }
}
