#include "LookAndFeel.h"
#include "RotaryKnob.h"

const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::VictorMonoRegular_ttf, BinaryData::VictorMonoRegular_ttfSize);

juce::Font Fonts::getFont(float height)
{
    return juce::Font(juce::FontOptions(typeface).withHeight(height));
}

// =============================================================================

class RotaryKnobLabel : public juce::Label
{
public:
    RotaryKnobLabel() : juce::Label() {}

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails&) override {}

    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return createIgnoredAccessibilityHandler(*this);
    }

    juce::TextEditor* createEditorComponent() override
    {
        auto* ed = new juce::TextEditor(getName());
        ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);

        ed->setBorder(juce::BorderSize<int>());
        ed->setIndents(2, 1);
        ed->setJustification(juce::Justification::centredTop);
        ed->setPopupMenuEnabled(false);

        // Kind of hacky!
        if (auto* knob = dynamic_cast<RotaryKnob*>(getParentComponent()->getParentComponent())) {
            ed->setInputRestrictions(knob->maxInputLength);
        }
        return ed;
    }
};

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::value);
    setColour(juce::Slider::textBoxHighlightColourId, Colors::Knob::trackActive);
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
    setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);
}

void RotaryKnobLookAndFeel::drawRotarySlider(
     juce::Graphics& g,
     int x, int y, int width, [[maybe_unused]] int height,
     float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
     juce::Slider& slider)
{
    auto fillColor = slider.findColour(juce::Slider::rotarySliderFillColourId);

    const float marginLeft = 0.0f;
    const float marginRight = 0.0f;
    const float marginTop = 0.0f;
    const float marginBottom = 0.0f;

    auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat()
        .withTrimmedLeft(marginLeft).withTrimmedRight(marginRight)
        .withTrimmedTop(marginTop).withTrimmedBottom(marginBottom);

    auto radius = bounds.getWidth() / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 4.0f;
    auto arcRadius = radius - lineW / 2.0f;
    auto dialW = 2.0f;

    auto center = bounds.getCentre();
    auto strokeType = juce::PathStrokeType(
        lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    auto arg = toAngle - juce::MathConstants<float>::halfPi;
    auto dialRadius = arcRadius - 6.0f;
    juce::Point<float> thumbEnd(center.x + dialRadius * std::cos(arg),
                                center.y + dialRadius * std::sin(arg));
    juce::Point<float> thumbStart(center.x, center.y);

    g.setColour(Colors::Knob::dial);
    g.drawLine(thumbStart.x, thumbStart.y, thumbEnd.x, thumbEnd.y, dialW);
    g.fillEllipse(juce::Rectangle<float>(dialW, dialW).withCentre(thumbStart));
    g.fillEllipse(juce::Rectangle<float>(dialW, dialW).withCentre(thumbEnd));

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x,
                                center.y,
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                rotaryEndAngle,
                                true);
    g.setColour(Colors::Knob::trackBackground);
    g.strokePath(backgroundArc, strokeType);

    if (slider.isEnabled()) {
        float fromAngle = rotaryStartAngle;
        int direction = int(slider.getProperties()["direction"]);
        if (direction == 1) {  // center
            fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.0f;
        } else if (direction == 2) {  // right
            fromAngle = rotaryEndAngle;
        }

        juce::Path valueArc;
        valueArc.addCentredArc(center.x,
                               center.y,
                               arcRadius,
                               arcRadius,
                               0.0f,
                               fromAngle,
                               toAngle,
                               true);
        g.setColour(fillColor);
        g.strokePath(valueArc, strokeType);
    }
}

juce::Label* RotaryKnobLookAndFeel::createSliderTextBox(juce::Slider& slider)
{
    auto l = new RotaryKnobLabel();
    l->setJustificationType(juce::Justification::centred);
    l->setKeyboardType(juce::TextInputTarget::decimalKeyboard);
    l->setColour(juce::Label::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    l->setColour(juce::TextEditor::textColourId, slider.findColour(juce::Slider::textBoxTextColourId));
    l->setColour(juce::TextEditor::highlightColourId, slider.findColour(juce::Slider::textBoxHighlightColourId));
    return l;
}

juce::Font RotaryKnobLookAndFeel::getLabelFont([[maybe_unused]] juce::Label& label)
{
    return Fonts::getFont(14.0f);
}

void RotaryKnobLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));

    if (!label.isBeingEdited()) {
        g.setFont(getLabelFont(label));
        g.setColour(Colors::Knob::value);
        g.drawFittedText(label.getText(),
                         label.getLocalBounds(),
                         label.getJustificationType(),
                         1,
                         1.0f);
    }
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(
    [[maybe_unused]] juce::Graphics& g,
    [[maybe_unused]] int width,
    [[maybe_unused]] int height,
    [[maybe_unused]] juce::TextEditor& textEditor)
{
    // do nothing
}

void RotaryKnobLookAndFeel::drawTextEditorOutline(
    [[maybe_unused]] juce::Graphics& g,
    [[maybe_unused]] int width, [[maybe_unused]] int height,
    [[maybe_unused]] juce::TextEditor& textEditor)
{
    // do nothing
}

// =============================================================================

ButtonLookAndFeel::ButtonLookAndFeel()
{
    setColour(juce::TextButton::buttonColourId, Colors::Button::background);
    setColour(juce::TextButton::buttonOnColourId, Colors::Button::backgroundToggled);
    setColour(juce::TextButton::textColourOffId, Colors::Button::text);
    setColour(juce::TextButton::textColourOnId, Colors::Button::textToggled);
}

void ButtonLookAndFeel::drawButtonBackground(
    juce::Graphics& g,
    juce::Button& button,
    [[maybe_unused]] const juce::Colour& backgroundColour,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto cornerSize = 4.0f;
    auto buttonRect = bounds.reduced(1.5f, 2.5f);

    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    g.setColour(backgroundColour);
    g.fillRoundedRectangle(buttonRect, cornerSize);

    if (button.getToggleState()) {
        g.setColour(Colors::Button::borderToggled);
        g.drawRoundedRectangle(buttonRect, cornerSize, 2.0f);
    } else {
        g.setColour(Colors::Button::border);
        g.drawRoundedRectangle(buttonRect, cornerSize, 1.0f);
    }
}

void ButtonLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto buttonRect = bounds.reduced(0.5f, 1.5f);
    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    if (button.getToggleState()) {
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
    }

    g.setFont(Fonts::getFont(15.0f));
    g.drawText(button.getButtonText(), buttonRect, juce::Justification::centred);
}

// =============================================================================

PickerButtonLookAndFeel::PickerButtonLookAndFeel()
{
    setColour(juce::TextButton::textColourOffId, Colors::Button::text);
    setColour(juce::TextButton::textColourOnId, Colors::Button::textToggled);
}

void PickerButtonLookAndFeel::drawButtonBackground(
    juce::Graphics&, juce::Button&, const juce::Colour&, bool, bool)
{
    // do nothing
}

void PickerButtonLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto buttonRect = bounds.reduced(0.5f, 1.5f);
//    auto buttonRect = bounds.reduced(4.0f, 2.0f).withTrimmedBottom(4.0f);
    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    if (button.getToggleState()) {
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
    }

//    g.setColour(button.findColour(juce::TextButton::textColourOffId));

    g.setFont(Fonts::getFont(15.0f));
    g.drawText(button.getButtonText(), buttonRect, juce::Justification::centred);
}

// =============================================================================

CircleButtonLookAndFeel::CircleButtonLookAndFeel()
{
    setColour(juce::TextButton::buttonColourId, Colors::CircleButton::background);
    setColour(juce::TextButton::textColourOffId, Colors::CircleButton::text);
}

void CircleButtonLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    if (shouldDrawButtonAsDown) {
        g.setColour(Colors::CircleButton::backgroundPressed);
    } else {
        g.setColour(backgroundColour);
    }
    g.fillEllipse(bounds);
}

void CircleButtonLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted,
    [[maybe_unused]] bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    g.setFont(Fonts::getFont(12.0f));
    g.setColour(button.findColour(juce::TextButton::textColourOffId));
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
}

// =============================================================================
