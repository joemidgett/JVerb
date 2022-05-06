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

    void createJVerbLabel(const juce::String& name, juce::Label& label, JVerbSlider& slider);
    void modifyJVerbSliderColors(JVerbSlider& slider, std::vector<juce::String> colours);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JVerbAudioProcessor& audioProcessor;

    JVerbLookAndFeel jVerbLnf;

    JVerbSlider jVerbDrySlider, 
                jVerbLowGainSlider, 
                jVerbReverbTimeSlider, 
                jVerbHighGainSlider, 
                jVerbWetSlider;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    SliderAttachment jVerbDrySliderAttachment,
                     jVerbLowGainSliderAttachment, 
                     jVerbReverbTimeSliderAttachment,
                     jVerbHighGainSliderAttachment, 
                     jVerbWetSliderAttachment;

    juce::Label jVerbDryLabel, 
                jVerbLowGainLabel, 
                jVerbReverbTimeLabel, 
                jVerbHighGainLabel, 
                jVerbWetLabel;

    std::vector<juce::String> jVerbDryColorIds { "purple", "mediumpurple", "rebeccapurple", "rebeccapurple"},
                              jVerbLowGainColorIds { "deeppink", "pink", "hotpink", "deeppink" },
                              jVerbReverbColorIds { "darkblue", "lightblue", "cornflowerblue", "darkblue" },
                              jVerbHighGainColorIds { "darkgreen", "lightgreen", "lightseagreen", "darkgreen" },
                              jVerbWetColorIds { "yellow", "lightyellow", "lightgoldenrodyellow", "darkolivegreen" };

    juce::Colour defaultColor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JVerbAudioProcessorEditor)
};
