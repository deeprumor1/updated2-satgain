/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VisualizerComponent.h"
#include "CustomLookAndFeel.h"
#include "LevelMeterComponent.h" // Include the new class

//==============================================================================
/**
*/
class GainKnobAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    GainKnobAudioProcessorEditor(GainKnobAudioProcessor&);
    ~GainKnobAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;


    VisualizerComponent visualizer; // Add the visualizer here
    CustomLookAndFeel customLookAndFeel;

    LevelMeterComponent levelMeters; // Add level meters

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GainKnobAudioProcessor& audioProcessor;


    juce::Slider gainSlider;
    juce::Slider eqKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainKnobAudioProcessorEditor)
};
