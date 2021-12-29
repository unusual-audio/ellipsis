/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class EllipsisAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    EllipsisAudioProcessor();
    ~EllipsisAudioProcessor() override;
    
    juce::AudioParameterInt*    drive;
    juce::AudioParameterInt*    crossover;
    
    const float MIN_DRIVE =      1;
    const float MAX_DRIVE =     11;
    const float DEFAULT_DRIVE =  3;
    
    const float MIN_CROSSOVER =      60;
    const float MAX_CROSSOVER =     200;
    const float DEFAULT_CROSSOVER = 120;
    
    const float GAIN_COMPENSATION = juce::Decibels::decibelsToGain(12.0); // at max drive
    
    void notifyUpdate();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    
    juce::dsp::LinkwitzRileyFilter<float> lowPassFilter;
    juce::dsp::LinkwitzRileyFilter<float> highPassFilter;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EllipsisAudioProcessor)
};
