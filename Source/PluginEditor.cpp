/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EllipsisAudioProcessorEditor::EllipsisAudioProcessorEditor (EllipsisAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
        driveSliderAttachment(*p.drive, driveSlider),
        crossoverSliderAttachment(*p.crossover, crossoverSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 280);

    addAndMakeVisible (driveSlider);
    
    driveSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    driveSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    driveSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    driveSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    driveSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    driveSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    driveSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
    addAndMakeVisible (crossoverSlider);
    crossoverSlider.setTextValueSuffix (" Hz");
    
    crossoverSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    crossoverSlider.setColour(juce::Slider::ColourIds::thumbColourId, yellow);
    crossoverSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    crossoverSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    crossoverSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    crossoverSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, yellow);
    crossoverSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);
    
}

EllipsisAudioProcessorEditor::~EllipsisAudioProcessorEditor()
{
}

//==============================================================================
void EllipsisAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (black);
    g.setColour(white);
    g.setFont(18);
    g.drawFittedText("Ellipsis",        40,  30, 320, 30, juce::Justification::left, 1);
    
    g.setColour(red);
    g.setFont(15);
    g.drawFittedText("Drive",           40,  90, 320, 30, juce::Justification::left, 1);
    
    g.setColour(yellow);
    g.setFont(15);
    g.drawFittedText("Crossover",       40, 150, 320, 30, juce::Justification::left, 1);
    
    g.setColour(grey);
    g.setFont(12);
    g.drawFittedText("Unusual Audio",   40, getHeight() - 60, 320, 30, juce::Justification::left, 1);
}

void EllipsisAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto sliderLeft = 110;
    driveSlider     .setBounds (sliderLeft,  95, getWidth() - sliderLeft - 60, 20);
    crossoverSlider .setBounds (sliderLeft, 155, getWidth() - sliderLeft - 60, 20);
}
