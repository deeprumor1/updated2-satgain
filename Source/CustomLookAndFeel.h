#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // Override the rotary slider drawing
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        // Adjust knob position
        int offsetY = 10; // Move knobs up by reducing the offset
        y += offsetY;

        // Base dimensions and center
        auto radius = juce::jmin(width / 2.0f, height / 2.0f) - 10.0f; // Adjust radius for spacing
        auto centerX = x + width * 0.5f;
        auto centerY = y + height * 0.5f;
        auto rx = centerX - radius;
        auto ry = centerY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // --- Outer Shadow for Depth ---
        g.setColour(juce::Colours::black.withAlpha(0.3f)); // Soft shadow
        g.fillEllipse(rx - 4, ry - 4, rw + 8, rw + 8);

        // --- Metallic Gradient for the Knob Base ---
        juce::ColourGradient metallicGradient(juce::Colours::lightgrey, centerX, centerY - radius, // Light at the top
                                              juce::Colours::darkgrey, centerX, centerY + radius,  // Dark at the bottom
                                              false);
        metallicGradient.addColour(0.3, juce::Colours::silver); // Add a brighter metallic shine
        g.setGradientFill(metallicGradient);
        g.fillEllipse(rx, ry, rw, rw); // Base fill

        // --- Border for the Knob ---
        g.setColour(juce::Colours::black.withAlpha(0.8f)); // Dark border
        g.drawEllipse(rx, ry, rw, rw, 1.5f);

        // --- 3D Highlight for the Knob ---
        g.setColour(juce::Colours::white.withAlpha(0.2f)); // Subtle highlight
        g.fillEllipse(rx + 5, ry + 5, rw - 10, rw * 0.5f); // Smaller ellipse on the top half

        // --- White Pointer Line ---
        juce::Path pointerPath;
        auto pointerLength = radius * 0.7f;  // Pointer length
        auto pointerThickness = 2.0f;       // Pointer thickness
        pointerPath.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        pointerPath.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
        g.setColour(juce::Colours::white);
        g.fillPath(pointerPath);

        if (slider.isMouseOverOrDragging())
        {
            g.setColour(juce::Colours::white.withAlpha(0.1f + sliderPos * 0.1f)); // Glow intensity
            g.fillEllipse(rx - 8, ry - 8, rw + 16, rw + 16); // Glow extends slightly beyond the knob
        }
    }

    // Override the label drawing
    void drawLabel(juce::Graphics& g, juce::Label& label) override
    {
        // Get the label's text
        const auto text = label.getText();

        // Set font and text properties
        g.setFont(juce::Font(14.0f)); // Remove bold styling, keep size 14
        g.setColour(juce::Colours::white);    // Set text color to white

        // Add glow effect
        g.setColour(juce::Colours::white.withAlpha(0.6f)); // Faint white glow
        g.drawFittedText(text, label.getLocalBounds().translated(0, 1), juce::Justification::centred, 1); // Move glow closer

        // Add tighter shadow effect
        g.setColour(juce::Colours::black.withAlpha(0.4f)); // Subtle, tighter shadow
        g.drawFittedText(text, label.getLocalBounds().translated(0, 2), juce::Justification::centred, 1); // Closer shadow offset

        // Draw the final text
        g.setColour(juce::Colours::white); // Final clean white text
        g.drawFittedText(text, label.getLocalBounds().translated(0, 0), juce::Justification::centred, 1); // Centered text
    }
};
