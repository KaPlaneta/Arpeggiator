/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

template <typename T>
struct ToggleButtonInfo {
    ToggleButtonInfo(std::string name, T alg, juce::ToggleButton* button) : name(name), alg(alg), button(button) { }
    std::string name;
    T alg;
    juce::ToggleButton* button;

    ToggleButtonInfo(const ToggleButtonInfo& other) {
        this->name = other.name;
        this->alg = other.alg;
        this->button = other.button;
    }
};


enum SythesiserAlgorithm {
    UpToDown,
    DownToUp,
    Random
};

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

    juce::ToggleButton upToDown;
    juce::ToggleButton downToUp;
    juce::ToggleButton random;
//    juce::ToggleButton terrainToggle;

    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ArpAudioProcessor& audioProcessor;
    
    juce::Slider speed;
    juce::Slider octaves;

    
    ToggleButtonInfo<enum SythesiserAlgorithm> buttons[3] = {
        { "Up to down", UpToDown, &upToDown},
        { "Down to up", DownToUp, &downToUp},
        { "Random", Random, &random},
//        { "Terrain algorithm", Terrain, &terrainToggle}
    };
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ArpAudioProcessorEditor)
};
