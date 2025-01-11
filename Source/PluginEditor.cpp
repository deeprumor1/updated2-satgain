/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "VisualizerComponent.h"



//==============================================================================
GainKnobAudioProcessorEditor::GainKnobAudioProcessorEditor(GainKnobAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    // Gain Knob
    gainSlider.setSliderStyle(juce::Slider::Rotary);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 20);
    gainSlider.setRange(0.0, 10.0, 0.1);
    gainSlider.setValue(1.0);
    gainSlider.setLookAndFeel(&customLookAndFeel); // Apply custom LookAndFeel
    gainSlider.onValueChange = [this]()
        {
            // Convert dB value to linear scale
            float gainInLinear = std::pow(10.0f, static_cast<float>(gainSlider.getValue()) / 20.0f);
            audioProcessor.gainValue.store(gainInLinear);
        };
    addAndMakeVisible(gainSlider);

    // Attach gain parameter to the slider
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "gain", gainSlider);

    // EQ Knob
    eqKnob.setSliderStyle(juce::Slider::Rotary);
    eqKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 30, 20);
    eqKnob.setRange(0.0, 10.0, 0.1);
    eqKnob.setValue(0.0);
    eqKnob.setLookAndFeel(&customLookAndFeel); // Apply custom LookAndFeel
    addAndMakeVisible(eqKnob);

    // Attach EQ boost parameter to the slider
    eqAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "eqBoost", eqKnob);

    // Visualizer Component
    addAndMakeVisible(visualizer);

    addAndMakeVisible(levelMeters);

    gainSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite); // Remove focus outline
    eqKnob.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite); // Remove focus outline

    // Set the size of the plugin editor window
    setSize(400, 300);
}

GainKnobAudioProcessorEditor::~GainKnobAudioProcessorEditor()
{

    gainSlider.setLookAndFeel(nullptr);
    eqKnob.setLookAndFeel(nullptr);

}

//==============================================================================
void GainKnobAudioProcessorEditor::paint(juce::Graphics& g)
{
    // --- Enhanced Metallic Gradient Background ---
    juce::ColourGradient backgroundGradient(juce::Colours::darkgrey, 0, 0,         // Start color at top-left
        juce::Colours::lightgrey, getWidth(), // End color at bottom-right
        getHeight(), false);                  // Linear gradient
    backgroundGradient.addColour(0.3, juce::Colours::silver);    // Stronger metallic shine
    backgroundGradient.addColour(0.7, juce::Colours::grey);      // Add depth with darker tones
    g.setGradientFill(backgroundGradient);
    g.fillAll();

    // --- Add Grain/Dust Overlay ---
    g.setColour(juce::Colours::white.withAlpha(0.05f)); // Light, translucent white for grains
    for (int i = 0; i < 500; ++i) // 500 grains for a subtle effect
    {
        int grainX = juce::Random::getSystemRandom().nextInt(getWidth());
        int grainY = juce::Random::getSystemRandom().nextInt(getHeight());
        g.fillRect(grainX, grainY, 1, 1); // Draw tiny 1x1 pixel grains
    }

    // --- Add Subtle Scratches ---
    g.setColour(juce::Colours::white.withAlpha(0.1f)); // Thin, faint scratches
    for (int i = 0; i < 20; ++i) // 20 scratches for a balanced effect
    {
        int startX = juce::Random::getSystemRandom().nextInt(getWidth());
        int endX = startX + juce::Random::getSystemRandom().nextInt(50) + 50; // Random scratch length
        int startY = juce::Random::getSystemRandom().nextInt(getHeight());
        g.drawLine(startX, startY, endX, startY, 0.5f); // Thin lines for scratches
    }

    // --- Text Above Knobs ---
    g.setFont(juce::Font(16.0f)); // Set font size
    g.setColour(juce::Colours::black.withAlpha(0.4f)); // Shadow color
    g.drawFittedText("SatGain", getWidth() / 4 - 50, getHeight() - 170, 100, 30, juce::Justification::centred, 1);
    g.drawFittedText("Harmonic Boost", 3 * getWidth() / 4 - 50, getHeight() - 170, 100, 30, juce::Justification::centred, 1);

    

}

void GainKnobAudioProcessorEditor::resized()
{
    // Define dimensions for the knobs
    int knobWidth = 125;       // Slightly increase knob width
    int knobHeight = 125;      // Slightly increase knob height
    int textBoxHeight = 20;    // Height of the text box
    int knobYOffset = -5;      // Adjust this to fine-tune the knob position
    int textBoxYOffset = 10;   // Offset to move the text box lower

    // Position the gain knob on the left
    gainSlider.setBounds(
        getWidth() / 4 - knobWidth / 2,
        getHeight() - knobHeight - 20 + knobYOffset, // Fine-tune knob position
        knobWidth,
        knobHeight);

    gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, knobWidth, textBoxHeight + textBoxYOffset); // Adjust text box size and position

    // Position the EQ knob on the right
    eqKnob.setBounds(
        3 * getWidth() / 4 - knobWidth / 2,
        getHeight() - knobHeight - 20 + knobYOffset, // Fine-tune knob position
        knobWidth,
        knobHeight);

    eqKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, knobWidth, textBoxHeight + textBoxYOffset); // Adjust text box size and position

    // Position the level meters in the middle
    int meterWidth = 55;    // Width of each meter
    int meterHeight = knobHeight - 30; // Shortened height of the meters
    int meterYOffset = getHeight() - knobHeight - textBoxHeight - textBoxYOffset - -10; // Move meters up


    levelMeters.setBounds(
        getWidth() / 2 - meterWidth,
        meterYOffset,
        2 * meterWidth,
        meterHeight);


    // Position the visualizer at the top
    visualizer.setBounds(0, 0, getWidth(), getHeight() - knobHeight - 60);
}
