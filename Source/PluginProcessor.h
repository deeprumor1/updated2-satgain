/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <juce_audio_processors/juce_audio_processors.h>


//==============================================================================
/**
*/
class GainKnobAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    GainKnobAudioProcessor();
    ~GainKnobAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    std::atomic<float> gainValue{ 0.5f }; // Initial gain value

    juce::AudioProcessorValueTreeState parameters;



private:

    float previousEqBoost = 0.0f;

    std::vector<juce::dsp::IIR::Filter<float>> eqFilters; // Array of EQ filters for each channel
    juce::dsp::IIR::Coefficients<float>::Ptr eqCoefficients; // Shared coefficients

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GainKnobAudioProcessor)
};

