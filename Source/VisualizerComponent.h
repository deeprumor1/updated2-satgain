#pragma once

#include <JuceHeader.h>

class VisualizerComponent : public juce::Component, private juce::Timer
{
public:
    VisualizerComponent();
    void pushSample(const float sample); // Push a single audio sample
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    juce::AudioBuffer<float> audioBuffer{ 1, 1024 }; // 1 channel, max size 1024
    const int maxBufferSize = 1024;                  // Circular buffer size
    int writeIndex = 0;                              // Write position in circular buffer

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VisualizerComponent)
};










