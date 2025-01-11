#pragma once

#include <JuceHeader.h>

class LevelMeterComponent : public juce::Component, private juce::Timer
{
public:
    LevelMeterComponent();
    void setLevels(float left, float right); // Set levels for the meters
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    float leftLevel = 0.0f;  // Normalized value for the left meter (0.0 to 1.0)
    float rightLevel = 0.0f; // Normalized value for the right meter (0.0 to 1.0)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterComponent)
};


