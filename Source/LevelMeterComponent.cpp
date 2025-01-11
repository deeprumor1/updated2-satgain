#include "LevelMeterComponent.h"

LevelMeterComponent::LevelMeterComponent()
{
    startTimerHz(30); // Update rate: 30 times per second
}

void LevelMeterComponent::setLevels(float left, float right)
{
    leftLevel = juce::jlimit(0.0f, 1.0f, left);  // Clamp to [0.0, 1.0]
    rightLevel = juce::jlimit(0.0f, 1.0f, right); // Clamp to [0.0, 1.0]
}

void LevelMeterComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    // Define parameters for the meter boxes
    auto boxWidth = bounds.getWidth() / 5;   // Narrower enclosing boxes
    auto boxHeight = bounds.getHeight();    // Full height for the boxes
    auto gap = bounds.getWidth() / 10;      // Space between the boxes
    auto leftBoxX = (bounds.getWidth() / 2) - boxWidth - (gap / 2); // Left box position
    auto rightBoxX = (bounds.getWidth() / 2) + (gap / 2);           // Right box position

    // --- Draw Left Box ---
    juce::Rectangle<float> leftBox(leftBoxX, 0, boxWidth, boxHeight);
    g.setColour(juce::Colours::darkgrey.darker(0.3f)); // Slightly darker background for the box
    g.fillRect(leftBox);

    // Add shadow effect for the left box
    juce::Rectangle<float> leftShadow(leftBox.getX(), leftBox.getBottom() - 5, leftBox.getWidth(), 5);
    g.setColour(juce::Colours::black.withAlpha(0.2f)); // Subtle shadow
    g.fillRect(leftShadow);

    // Draw left meter fill
    auto leftMeterHeight = boxHeight * leftLevel;
    juce::Rectangle<float> leftMeter(leftBox.getX() + 5, boxHeight - leftMeterHeight, boxWidth - 10, leftMeterHeight);
    g.setColour(juce::Colours::silver.withAlpha(leftLevel * 0.8f + 0.2f)); // Glow based on intensity
    g.fillRect(leftMeter);

    // Add border around the left meter
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRect(leftMeter, 1.0f);

    // --- Draw Right Box ---
    juce::Rectangle<float> rightBox(rightBoxX, 0, boxWidth, boxHeight);
    g.setColour(juce::Colours::darkgrey.darker(0.3f)); // Slightly darker background for the box
    g.fillRect(rightBox);

    // Add shadow effect for the right box
    juce::Rectangle<float> rightShadow(rightBox.getX(), rightBox.getBottom() - 5, rightBox.getWidth(), 5);
    g.setColour(juce::Colours::black.withAlpha(0.2f)); // Subtle shadow
    g.fillRect(rightShadow);

    // Draw right meter fill
    auto rightMeterHeight = boxHeight * rightLevel;
    juce::Rectangle<float> rightMeter(rightBox.getX() + 5, boxHeight - rightMeterHeight, boxWidth - 10, rightMeterHeight);
    g.setColour(juce::Colours::silver.withAlpha(rightLevel * 0.8f + 0.2f)); // Glow based on intensity
    g.fillRect(rightMeter);

    // Add border around the right meter
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.drawRect(rightMeter, 1.0f);
}


void LevelMeterComponent::resized()
{
    // Adjust layout if needed
}

void LevelMeterComponent::timerCallback()
{
    // Smooth decay logic could be added here if levels should "fall" over time
    repaint(); // Repaint the component regularly
}

