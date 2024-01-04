/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/



#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
ArpAudioProcessor::ArpAudioProcessor()
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

}

ArpAudioProcessor::~ArpAudioProcessor()
{
}

//==============================================================================
const juce::String ArpAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ArpAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ArpAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ArpAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ArpAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ArpAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ArpAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ArpAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ArpAudioProcessor::getProgramName (int index)
{
    return {};
}

void ArpAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ArpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    juce::ignoreUnused (samplesPerBlock);

            notes.clear();
            currentNote = 0;
            lastNoteValue = -1;
            time = 0;
            rate = static_cast<float> (sampleRate);
        
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}



void ArpAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ArpAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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


void ArpAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    // A pure MIDI plugin shouldn't be provided any audio data
            jassert (buffer.getNumChannels() == 0);

            // however we use the buffer to get timing information
            auto numSamples = buffer.getNumSamples();

            // get note duration
            auto noteDuration = static_cast<int> (std::ceil (rate * 0.25f * (0.1f + (1.0f - (speed)))));

            for (const auto metadata : midiMessages) //nutki
            {
                const auto msg = metadata.getMessage(); //siedzi tu pojedyncza nutka
                if      (msg.isNoteOn())  notes.add (msg.getNoteNumber()); //zapisuje wysokosc dzwieku ktora nuta. Notes tablica ktora trzyma nuty i dodaje add
                else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber()); //odkliknieta no to usuwam
            }

        midiMessages.clear(); //czyszczony bufor
    
    if ((time + numSamples) >= noteDuration)
            {
                auto offset = fmax (0, fmin ((int) (noteDuration - time), numSamples - 1));

                if (lastNoteValue > 0)
                {
                    midiMessages.addEvent (juce::MidiMessage::noteOff (1, lastNoteValue), offset);
                    lastNoteValue = -1;
                }

                if (notes.size() > 0)
                {
                    currentNote = (currentNote + 1) % notes.size();
                    lastNoteValue = notes[currentNote];
                    midiMessages.addEvent (juce::MidiMessage::noteOn (1, lastNoteValue, (juce::uint8) 127), offset);
                }

            }

            time = (time + numSamples) % noteDuration;
        
}

//==============================================================================
bool ArpAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ArpAudioProcessor::createEditor()
{
    return new ArpAudioProcessorEditor (*this);
}

//==============================================================================
void ArpAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ArpAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    
    
    return new ArpAudioProcessor();
}
