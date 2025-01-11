#include "VisualizerComponent.h"

VisualizerComponent::VisualizerComponent()
{
    startTimerHz(30); // Lower refresh rate to 30 Hz for better performance
}

void VisualizerComponent::pushSample(const float sample)
{
    // Use a circular buffer to manage samples efficiently
    if (writeIndex >= maxBufferSize)
        writeIndex = 0; // Wrap around when reaching the end

    audioBuffer.setSample(0, writeIndex, sample);
    ++writeIndex;
}

void VisualizerComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey); // Background color

    auto width = getWidth();
    auto height = getHeight();

    if (writeIndex > 0)
    {
        auto* buffer = audioBuffer.getReadPointer(0);
        juce::Path waveformPath;
        waveformPath.startNewSubPath(0, height / 2);

        for (int i = 0; i < maxBufferSize; ++i)
        {
            int bufferIndex = (writeIndex + i) % maxBufferSize; // Wrap around for circular buffer
            float x = (float)i / (float)maxBufferSize * width;
            float y = height / 2 - buffer[bufferIndex] * (height / 2);
            waveformPath.lineTo(x, y);
        }

        g.setColour(juce::Colours::silver); // Waveform color
        g.strokePath(waveformPath, juce::PathStrokeType(1.0f)); // Draw waveform
    }
}

void VisualizerComponent::timerCallback()
{
    repaint(); // Trigger visualizer repaint
}

void VisualizerComponent::resized()
{
    // Adjust buffer size or other parameters if needed
}





