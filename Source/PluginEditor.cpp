/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

enum RadioButtonIds
{
    SynthAlgorithmID = 1001,
//    CrawlingDirection = 1002
};

//==============================================================================
ArpAudioProcessorEditor::ArpAudioProcessorEditor (ArpAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    // BUTTONS FOR ALGORITHM CHOOSING
    for (auto button: buttons) {
        button.button->setRadioGroupId(SynthAlgorithmID);
        auto alg = button.alg;
        button.button->onClick = [this, alg]() {
            *audioProcessor.algorithmParam = static_cast<float>(alg);
        };

        button.button->setToggleState(*audioProcessor.algorithmParam == static_cast<float>(alg), juce::NotificationType::dontSendNotification);
        button.button->setButtonText(button.name);
        addAndMakeVisible(button.button);
    }

    
    
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

    int it = 40;
    for (auto button : buttons) {
        button.button->setBounds(10, it, 150, 25);
        it += 25;
    }
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
