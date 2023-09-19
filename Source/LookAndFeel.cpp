#include "LookAndFeel.h"
#include "RotaryKnob.h"

const juce::Typeface::Ptr Fonts::typeface = juce::Typeface::createSystemTypefaceFor(
    BinaryData::VictorMonoRegular_ttf, BinaryData::VictorMonoRegular_ttfSize);

juce::Font Fonts::getFont(float height)
{
    return juce::Font(typeface).withHeight(height);
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
        // The reason for our own Label subclass is so we can customize
        // the TextEditor as neither object is exposed by juce::Slider.

        auto* ed = new juce::TextEditor(getName());
        ed->applyFontToAllText(getLookAndFeel().getLabelFont(*this));
        copyAllExplicitColoursTo(*ed);

        // The TextEditor size is always the bounds of the Label.
        // Align it with the smaller round rect that we've drawn.

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

RotaryKnobLookAndFeel::RotaryKnobLookAndFeel(bool fromCenter) :
    fromCenter(fromCenter),
    dropShadow(Colors::Knob::dropShadow, 6, { 0, 3 })
{
    if (fromCenter) {
        setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackFromCenter);
    } else {
        setColour(juce::Slider::rotarySliderFillColourId, Colors::Knob::trackActive);
    }

    // See also lnf.createSliderTextBox where these color IDs are assigned
    setColour(juce::Slider::textBoxTextColourId, Colors::Knob::value);

    // Used by the TextEditor
    if (fromCenter) {
        setColour(juce::Slider::textBoxHighlightColourId, Colors::Knob::trackFromCenter);
    } else {
        setColour(juce::Slider::textBoxHighlightColourId, Colors::Knob::trackActive);
    }
    setColour(juce::CaretComponent::caretColourId, Colors::Knob::caret);
    setColour(juce::TextEditor::highlightedTextColourId, Colors::Knob::value);

    // Note: juce::Slider::textBoxBackgroundColourId is the background color
    // when not editing, but it's also TextEditor::backgroundColourId, so we
    // have to override fillTextEditorBackground() to paint a different color.
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
    auto lineW = 3.0f;
    auto arcRadius = radius - lineW / 2.0f;
    auto dialW = 2.0f;

    auto center = bounds.getCentre();
    auto strokeType = juce::PathStrokeType(
        lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

    auto knobRect = bounds.reduced(10.0f, 10.0f);
    auto path = juce::Path();
    path.addEllipse(knobRect);
    dropShadow.drawForPath(g, path);
    g.setColour(Colors::Knob::outline);
    g.fillEllipse(knobRect);

    auto innerRect = knobRect.reduced(dialW, dialW);
    auto gradient = juce::ColourGradient(
        Colors::Knob::gradientTop, 0.0f, innerRect.getY(),
        Colors::Knob::gradientBottom, 0.0f, innerRect.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillEllipse(innerRect);

    auto arg = toAngle - juce::MathConstants<float>::halfPi;
    auto dialRadius = arcRadius - 14.0f;
    juce::Point<float> thumbEnd(center.x + dialRadius * std::cos(arg),
                                center.y + dialRadius * std::sin(arg));
    juce::Point<float> thumbStart(center.x + 10.0f * std::cos(arg),
                                  center.y + 10.0f * std::sin(arg));

    auto outsideW = dialW * 2.0f;
    g.setColour(Colors::Knob::outline);
    g.drawLine(thumbEnd.x, thumbEnd.y, thumbStart.x, thumbStart.y, outsideW);
    g.fillEllipse(juce::Rectangle<float>(outsideW, outsideW).withCentre(thumbStart));
    g.fillEllipse(juce::Rectangle<float>(outsideW, outsideW).withCentre(thumbEnd));

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
        if (fromCenter) {
            fromAngle += (rotaryEndAngle - rotaryStartAngle) / 2.0f;
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
        const juce::Font font(getLabelFont(label));
        g.setFont(font);

        g.setColour(Colors::Knob::textBoxBackground);
        g.fillRoundedRectangle(label.getLocalBounds().reduced(8, 0).toFloat(), 4.0f);
        g.setColour(Colors::Knob::value);

        g.drawFittedText(label.getText(),
                         label.getLocalBounds(),
                         label.getJustificationType(),
                         1,
                         1.0f);
    }
}

void RotaryKnobLookAndFeel::fillTextEditorBackground(
    juce::Graphics& g, [[maybe_unused]] int width, [[maybe_unused]] int height,
    juce::TextEditor& textEditor)
{
    g.setColour(Colors::Knob::textBoxBackground);
    g.fillRoundedRectangle(textEditor.getLocalBounds().reduced(8, 0).toFloat(), 4.0f);
}

void RotaryKnobLookAndFeel::drawTextEditorOutline(
    [[maybe_unused]] juce::Graphics& g,
    [[maybe_unused]] int width, [[maybe_unused]] int height,
    [[maybe_unused]] juce::TextEditor& textEditor)
{
    // do nothing
}

// =============================================================================

BarButtonLookAndFeel::BarButtonLookAndFeel()
{
    setColour(juce::TextButton::buttonColourId, Colors::BarButton::background);
    setColour(juce::TextButton::buttonOnColourId, Colors::BarButton::backgroundToggled);
    setColour(juce::TextButton::textColourOffId, Colors::BarButton::text);
    setColour(juce::TextButton::textColourOnId, Colors::BarButton::textToggled);
}

void BarButtonLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 1.5f);
    auto cornerSize = bounds.getHeight() * 0.25f;

    if (shouldDrawButtonAsDown) {
        bounds.translate(0.0f, 1.0f);
    } else {
        g.setColour(juce::Colours::black.withAlpha(0.1f));
        g.fillRoundedRectangle(bounds.translated(0.0f, 1.0f), cornerSize);
    }

    g.setColour(backgroundColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    if (button.getToggleState()) {
        g.setColour(Colors::BarButton::borderToggled);
    } else {
        g.setColour(Colors::BarButton::border);
    }
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
}

void BarButtonLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 1.5f);
    if (shouldDrawButtonAsDown) {
        bounds.translate(0.0f, 1.0f);
    }

    g.setFont(Fonts::getFont(12.0f));

    if (button.getToggleState()) {
        g.setColour(button.findColour(juce::TextButton::textColourOnId));
    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
    }

    g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
}

// =============================================================================

ButtonLookAndFeel::ButtonLookAndFeel() : dropShadow(Colors::Knob::dropShadow, 4, { 0, 2 })
{
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
    auto cornerSize = bounds.getHeight() * 0.25f;
    auto buttonRect = bounds.reduced(4.0f, 2.0f).withTrimmedBottom(4.0f);

    auto path = juce::Path();
    path.addRoundedRectangle(buttonRect, cornerSize, cornerSize);
    dropShadow.drawForPath(g, path);

    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

//    if (button.getToggleState()) {
//        g.setColour(Colors::Knob::trackActive);
//        g.drawRoundedRectangle(buttonRect, cornerSize, 2.0f);
//        buttonRect.reduce(1.0f, 1.0f);
//        cornerSize -= 1.0f;
//    }

    g.setColour(Colors::Knob::outline);
    g.fillRoundedRectangle(buttonRect, cornerSize);

    auto innerRect = buttonRect.reduced(1.0f);
//    if (button.getToggleState()) {
//        g.setGradientFill(juce::ColourGradient(
//            Colors::Button::gradientTopToggled, 0.0f, innerRect.getY(),
//            Colors::Button::gradientBottomToggled, 0.0f, innerRect.getBottom(), false));
//    } else {
        g.setGradientFill(juce::ColourGradient(
            Colors::Knob::gradientTop, 0.0f, innerRect.getY(),
            Colors::Knob::gradientBottom, 0.0f, innerRect.getBottom(), false));
//    }
    g.fillRoundedRectangle(innerRect, cornerSize - 1.0f);

    if (button.getToggleState()) {
        auto toggleRect = innerRect.withY(innerRect.getBottom() - 3.0f)
            .withHeight(2.0f).withTrimmedLeft(6.0f).withTrimmedRight(6.0f);
        g.setColour(Colors::Knob::trackActive);
        g.fillRoundedRectangle(toggleRect, 1.0f);
    }
}

void ButtonLookAndFeel::drawButtonText(
    juce::Graphics& g, juce::TextButton& button,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    auto buttonRect = bounds.reduced(4.0f, 2.0f).withTrimmedBottom(4.0f);
    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    g.setFont(Fonts::getFont(15.0f));

//    if (button.getToggleState()) {
//        g.setColour(button.findColour(juce::TextButton::textColourOffId).withAlpha(0.2f));
//    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOnId).withAlpha(0.5f));
//    }
    g.drawText(button.getButtonText(), buttonRect.translated(0.0f, 1.0f), juce::Justification::centred);

//    if (button.getToggleState()) {
//        g.setColour(button.findColour(juce::TextButton::textColourOnId));
//    } else {
        g.setColour(button.findColour(juce::TextButton::textColourOffId));
//    }
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
    auto buttonRect = bounds.reduced(4.0f, 2.0f).withTrimmedBottom(4.0f);
    if (shouldDrawButtonAsDown) {
        buttonRect.translate(0.0f, 1.0f);
    }

    g.setFont(Fonts::getFont(15.0f));

    g.setColour(button.findColour(juce::TextButton::textColourOnId).withAlpha(0.5f));
    g.drawText(button.getButtonText(), buttonRect.translated(0.0f, 1.0f), juce::Justification::centred);

    g.setColour(button.findColour(juce::TextButton::textColourOffId));
    g.drawText(button.getButtonText(), buttonRect, juce::Justification::centred);
}

// =============================================================================

RoundButtonLookAndFeel::RoundButtonLookAndFeel()
{
    setColour(juce::TextButton::buttonColourId, Colors::RoundButton::background);
    setColour(juce::TextButton::textColourOffId, Colors::RoundButton::text);
}

void RoundButtonLookAndFeel::drawButtonBackground(
    juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
    [[maybe_unused]] bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    if (shouldDrawButtonAsDown) {
        g.setColour(Colors::RoundButton::backgroundPressed);
    } else {
        g.setColour(backgroundColour);
    }
    g.fillEllipse(bounds);
}

void RoundButtonLookAndFeel::drawButtonText(
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
