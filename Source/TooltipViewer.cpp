#include "TooltipViewer.h"
#include "LookAndFeel.h"

TooltipViewer::TooltipViewer()
{
    startTimer(200);
}

void TooltipViewer::paint(juce::Graphics& g)
{
    g.fillAll(Colors::background);

    if (showTip.load()) {
        g.setFont(Fonts::getFont(13.0f));
        g.setColour(Colors::tooltip);
        g.drawFittedText(tip, getLocalBounds(), juce::Justification::centredTop, 1, 1.0f);
    }
}

void TooltipViewer::timerCallback()
{
    const auto mouseSource = juce::Desktop::getInstance().getMainMouseSource();
    auto* newComponent = mouseSource.isTouch() ? nullptr : mouseSource.getComponentUnderMouse();

    if (component != newComponent) {
        component = newComponent;
        if (component != nullptr) {
            tip = getTipFor(*component);
            showTip.store(true);
            repaint();
        } else {
            if (showTip.load()) {
                showTip.store(false);
                repaint();
            }
        }
    }
}

juce::String TooltipViewer::getTipFor(juce::Component& component)
{
    auto* tooltipClient = dynamic_cast<juce::TooltipClient*>(&component);
    if (tooltipClient && !component.isCurrentlyBlockedByAnotherModalComponent()) {
        return tooltipClient->getTooltip();
    }
    return {};
}
