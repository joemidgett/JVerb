/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/JVerbLookAndFeel.h"
#include "GUI/JVerbSlider.h"

//==============================================================================
/**
*/
class JVerbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JVerbAudioProcessorEditor (JVerbAudioProcessor&);
    ~JVerbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JVerbAudioProcessor& audioProcessor;
    JVerbLookAndFeel jVerbLnf;

    JVerbSlider jVerbDrySlider, jVerbLowGainSlider, jVerbReverbTimeSlider, jVerbHighGainSlider, jVerbWetSlider;

    juce::AudioProcessorValueTreeState::SliderAttachment jVerbDrySliderAttachment,
        jVerbLowGainSliderAttachment, jVerbReverbTimeSliderAttachment,
        jVerbHighGainSliderAttachment, jVerbWetSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JVerbAudioProcessorEditor)
};
