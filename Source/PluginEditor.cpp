/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ArpAudioProcessorEditor::ArpAudioProcessorEditor (ArpAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    speed.addListener (this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    speed.setSliderStyle (juce::Slider::LinearBar);
    speed.setRange(0, 1, 0.01); // Zakres od 1 do 100 z krokiem 1
    speed.setPopupDisplayEnabled (true, false, this);
//    windowSize.setTextValueSuffix ("Window size");
    speed.setValue(0.5);

    addAndMakeVisible (&speed);

}

void ArpAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
audioProcessor.speed = speed.getValue();
}


ArpAudioProcessorEditor::~ArpAudioProcessorEditor()
{
}

//==============================================================================
void ArpAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void ArpAudioProcessorEditor::resized()
{
    
    speed.setBounds (40, 110, getWidth()-60, 20);

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
