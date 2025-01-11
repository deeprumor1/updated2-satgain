/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_dsp/juce_dsp.h>



//==============================================================================
GainKnobAudioProcessor::GainKnobAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    parameters(*this, nullptr, "PARAMETERS",
        {
            std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 10.0f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("eqBoost", "EQ Boost", 0.0f, 10.0f, 0.0f)
        })
#endif
{
}

GainKnobAudioProcessor::~GainKnobAudioProcessor()
{
}

//==============================================================================
const juce::String GainKnobAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GainKnobAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GainKnobAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GainKnobAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GainKnobAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GainKnobAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int GainKnobAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GainKnobAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GainKnobAudioProcessor::getProgramName(int index)
{
    return {};
}

void GainKnobAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void GainKnobAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialize EQ filter coefficients with a default gain of 0 dB
    eqCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, 400.0f, 0.707f, juce::Decibels::decibelsToGain(0.0f));

    // Resize filters to match the number of channels
    eqFilters.resize(getTotalNumInputChannels());
    for (auto& filter : eqFilters)
        filter.coefficients = eqCoefficients;
}

void GainKnobAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GainKnobAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void GainKnobAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear extra output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get the current gain and EQ boost values
    float gain = parameters.getRawParameterValue("gain")->load();
    float eqBoost = parameters.getRawParameterValue("eqBoost")->load();

    // Update EQ coefficients only if the knob value changes
    if (eqBoost != previousEqBoost)
    {
        float gainInDecibels = juce::Decibels::decibelsToGain(eqBoost);
        eqCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            getSampleRate(), 400.0f, 0.707f, gainInDecibels);

        for (auto& filter : eqFilters)
            filter.coefficients = eqCoefficients;

        previousEqBoost = eqBoost;
    }

    // Wrap the buffer in a DSP block
    juce::dsp::AudioBlock<float> audioBlock(buffer);

    // Variables to track peak levels for left and right channels
    float leftChannelLevel = 0.0f;
    float rightChannelLevel = 0.0f;

    // Process each channel
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto channelBlock = audioBlock.getSingleChannelBlock(channel);
        juce::dsp::ProcessContextReplacing<float> context(channelBlock);

        // Apply EQ filter for this channel
        eqFilters[channel].process(context);

        // Apply gain and saturation manually
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float processedSample = channelData[sample] * gain;

            // Apply soft saturation
            if (gain > 1.0f)
            {
                float threshold = 0.8f;
                if (processedSample > threshold)
                    processedSample = threshold + (processedSample - threshold) / (1.0f + std::pow(processedSample - threshold, 2));
                else if (processedSample < -threshold)
                    processedSample = -threshold + (processedSample + threshold) / (1.0f + std::pow(processedSample + threshold, 2));
            }

            channelData[sample] = processedSample;
        }

        // Calculate peak levels for each channel
        if (channel == 0) // Left channel
            leftChannelLevel = std::max(leftChannelLevel, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));
        else if (channel == 1) // Right channel
            rightChannelLevel = std::max(rightChannelLevel, buffer.getMagnitude(channel, 0, buffer.getNumSamples()));
    }

    // Send levels to the editor
    if (auto* editor = dynamic_cast<GainKnobAudioProcessorEditor*>(getActiveEditor()))
    {
        // Update level meters
        editor->levelMeters.setLevels(leftChannelLevel, rightChannelLevel);

        // Push a sample from the left channel to the visualizer
        if (totalNumInputChannels > 0)
        {
            float sampleToPush = buffer.getReadPointer(0)[0]; // First sample of the left channel
            editor->visualizer.pushSample(sampleToPush);
        }
    }
}

//==============================================================================
bool GainKnobAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GainKnobAudioProcessor::createEditor()
{
    return new GainKnobAudioProcessorEditor(*this);
}

//==============================================================================
void GainKnobAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GainKnobAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
    }

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GainKnobAudioProcessor();
}

