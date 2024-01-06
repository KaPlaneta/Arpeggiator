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
                       ),
parameters(*this, nullptr, juce::Identifier("ImageSonification"),
    {
        std::make_unique<juce::AudioParameterInt>("algorithm",            // parameterID
                                                     "Algorithm",            // parameter name
                                                     0,              // minimum value
                                                     100,              // maximum value
                                                     0)        // default value
    })
#endif
{
    currentOctave = 1;
    counterNotes = 0;
    algorithmParam = parameters.getRawParameterValue("algorithm");

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
                if (msg.isNoteOn())  notes.add (msg.getNoteNumber()); //zapisuje wysokosc dzwieku ktora nuta. Notes tablica ktora trzyma nuty i dodaje add
                else if (msg.isNoteOff()) notes.removeValue (msg.getNoteNumber()); //odkliknieta no to usuwam
            }

        midiMessages.clear(); //czyszczony bufor
    
    if (notesSize != notes.size()) {
        notesSize = notes.size();
        currentOctave = 1;
        counterNotes = 0;
        if (*algorithmParam == static_cast<float>(DownToUp)) {
            currentNote = -1;
        }
        else if (*algorithmParam == static_cast<float>(UpToDown)) {
            currentNote = notesSize;
        }
    }
    
    if ((time + numSamples) >= noteDuration)
            {
                auto offset = fmax (0, fmin ((int) (noteDuration - time), numSamples - 1));

                if (lastNoteValue > 0)
                {
                    midiMessages.addEvent (juce::MidiMessage::noteOff (1, newNotes), offset);
                    lastNoteValue = -1;
                }

                if (notesSize > 0)
                {
                    if (*algorithmParam == static_cast<float>(DownToUp)) {
                        
                        currentNote = (currentNote + 1) % notesSize;
                        
                    }
                    else if (*algorithmParam == static_cast<float>(UpToDown)) {
                        if(currentNote == 0){
                            
                            currentNote = notesSize -1;

                        } else{
                            currentNote = currentNote - 1;
                        }
                    }
                    else if (*algorithmParam == static_cast<float>(Random)) {
                        
                        auto randomInt = juce::Random::getSystemRandom().nextInt(notesSize);
                        currentNote = randomInt;
                        
                    }
                    lastNoteValue = notes[currentNote];
                    newNotes = lastNoteValue + ((currentOctave-1)*semitonesInOctave);
                    
                    midiMessages.addEvent (juce::MidiMessage::noteOn (1, newNotes, (juce::uint8) 127), offset);
                    
                    counterNotes += 1;

                    if(counterNotes >= notesSize){
                        counterNotes = 0;
                        currentOctave = currentOctave + 1;

                        if(currentOctave >= octaves+1){
                            
                            
                            currentOctave = 1;
                            
                        }
                        if (*algorithmParam == static_cast<float>(DownToUp)) {
                            currentNote = -1;
                        }
                        else if (*algorithmParam == static_cast<float>(UpToDown)) {
                            currentNote = notesSize;
                        }
                        
                    }
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
    
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ArpAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));

    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    
    
    return new ArpAudioProcessor();
}
