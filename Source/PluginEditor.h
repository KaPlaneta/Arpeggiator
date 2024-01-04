/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ArpAudioProcessorEditor  : public juce::AudioProcessorEditor,  private juce::Slider::Listener
{
public:
    ArpAudioProcessorEditor (ArpAudioProcessor&);
    ~ArpAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void sliderValueChanged (juce::Slider* slider) override; // MOD 11 domyslna funkcja callback

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ArpAudioProcessor& audioProcessor;
    
    juce::Slider speed;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArpAudioProcessorEditor)
};
