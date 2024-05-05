#include <JuceHeader.h>
#include "SegmentedPicker.h"
#include "LookAndFeel.h"

SegmentedPicker::SegmentedPicker(juce::AudioParameterChoice& parameter) :
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
    auto cornerSize = 4.0f;
    auto buttonRect = bounds.reduced(1.5f, 2.5f);

    g.setColour(Colors::Button::background);
    g.fillRoundedRectangle(buttonRect, cornerSize);

    g.setColour(Colors::Button::border);
    g.drawRoundedRectangle(buttonRect, cornerSize, 1.0f);

    g.setColour(juce::Colours::black.withAlpha(0.1f));
    for (int i = 1; i < getNumButtons(); ++i) {
        g.fillRect(float(buttons[size_t(i)]->getX()) - 1.0f,
                   buttonRect.getY() + 1.0f,
                   1.0f, buttonRect.getHeight() - 2.0f);
    }

    auto buttonBounds = buttons[size_t(selectedIndex)]->getBounds().toFloat();
    auto toggleRect = buttonBounds.reduced(0.0f, 3.0f).withTrimmedLeft(-1.0f);

    g.setColour(Colors::Button::backgroundToggled);
    g.fillRoundedRectangle(toggleRect, cornerSize);

    g.setColour(Colors::Button::borderToggled);
    g.drawRoundedRectangle(toggleRect, cornerSize, 2.0f);
}

void SegmentedPicker::resized()
{
    int numButtons = getNumButtons();

    float margin = 2.0f;
    float width = float(getWidth());
    float segmentWidth = (width - 2.0f * margin) / float(numButtons);

    int xa = int(margin);
    int xb;

    for (int i = 0; i < numButtons; ++i) {
        if (i == numButtons - 1) {
            xb = int(width + 1.0f - margin);
        } else {
            xb = int(margin + segmentWidth * float(i + 1));
        }
        buttons[size_t(i)]->setBounds(xa, 0, xb - xa, getHeight());
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
        if (button == buttons[size_t(i)].get()) {
            attachment.setValueAsCompleteGesture(float(i));
            break;
        }
    }
}
