/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EllipsisAudioProcessor::EllipsisAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
    addParameter (drive = new juce::AudioParameterInt ({"drive", 1}, "Drive", MIN_DRIVE, MAX_DRIVE, DEFAULT_DRIVE));
    addParameter (crossover = new juce::AudioParameterInt ({"crossover", 1}, "Crossover", MIN_CROSSOVER, MAX_CROSSOVER, DEFAULT_CROSSOVER));
}

EllipsisAudioProcessor::~EllipsisAudioProcessor()
{
}

//==============================================================================
const juce::String EllipsisAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EllipsisAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool EllipsisAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool EllipsisAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double EllipsisAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EllipsisAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EllipsisAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EllipsisAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String EllipsisAudioProcessor::getProgramName (int index)
{
    return {};
}

void EllipsisAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void EllipsisAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.setCutoffFrequency(*crossover);
    lowPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    lowPassFilter.prepare(spec);
    lowPassFilter.reset();

    highPassFilter.setCutoffFrequency(*crossover);
    highPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    highPassFilter.prepare(spec);
    highPassFilter.reset();
}

void EllipsisAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EllipsisAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void EllipsisAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    lowPassFilter.setCutoffFrequency(*crossover);
    highPassFilter.setCutoffFrequency(*crossover);
    
    juce::dsp::AudioBlock<float> block (buffer);

    juce::AudioBuffer<float> lowBandBuffer;
    lowBandBuffer.makeCopyOf(buffer);
    juce::dsp::AudioBlock<float> lowBandBlock (lowBandBuffer);
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float> (lowBandBlock));

    juce::AudioBuffer<float> highBandBuffer;
    highBandBuffer.makeCopyOf(buffer);
    juce::dsp::AudioBlock<float> highBandBlock (highBandBuffer);
    highPassFilter.process(juce::dsp::ProcessContextReplacing<float> (highBandBlock));

    for (int i = 0; i < lowBandBuffer.getNumChannels(); ++i)
        for (int sample = 0; sample < lowBandBuffer.getNumSamples(); ++sample)
        {
            float s = lowBandBuffer.getSample(i, sample);
            int iterations = 1 + (int) pow(*drive, 2.5f) / 4;
            for (int j = 0; j < iterations; ++j)
                s = tanh(s); // vibe af
            lowBandBuffer.setSample(i, sample, s);
        }

    // Compensate for the loss and add a little bit extra
    lowBandBuffer.applyGain(1 + (*drive * (GAIN_COMPENSATION / MAX_DRIVE)));

    buffer.clear();
    for (int i = 0; i < buffer.getNumChannels(); ++i)
    {
        buffer.addFrom(i, 0, lowBandBuffer, i, 0, buffer.getNumSamples());
        buffer.addFrom(i, 0, highBandBuffer, i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool EllipsisAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EllipsisAudioProcessor::createEditor()
{
    return new EllipsisAudioProcessorEditor (*this);
}

//==============================================================================
void EllipsisAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    std::unique_ptr<juce::XmlElement> xml (new juce::XmlElement ("EllipsisParams"));
    xml->setAttribute ("drive", (double) *drive);
    xml->setAttribute ("crossover", (double) *crossover);
    copyXmlToBinary (*xml, destData);
}

void EllipsisAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName ("EllipsisParams"))
        {
            *drive = (float) xmlState->getDoubleAttribute ("drive", *drive);
            *crossover = (float) xmlState->getDoubleAttribute ("crossover", *crossover);
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EllipsisAudioProcessor();
}
