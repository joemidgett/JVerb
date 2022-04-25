/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JVerbAudioProcessorEditor::JVerbAudioProcessorEditor (JVerbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&jVerbLnf);
    
  
    addAndMakeVisible(jVerbSlider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

JVerbAudioProcessorEditor::~JVerbAudioProcessorEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

//==============================================================================
void JVerbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void JVerbAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    jVerbDrySlider.setBounds(0, 30, 130, 150);
    jVerbLowGainSlider.setBounds(jVerbDrySlider.getBounds().withX(jVerbDrySlider.getRight()));
    jVerbReverbTimeSlider.setBounds(jVerbLowGainSlider.getBounds().withX(jVerbLowGainSlider.getRight()));
    jVerbHighGainSlider.setBounds(jVerbReverbTimeSlider.getBounds().withX(jVerbReverbTimeSlider.getRight()));
    jVerbWetSlider.setBounds(jVerbHighGainSlider.getBounds().withX(jVerbHighGainSlider.getRight()));
}
