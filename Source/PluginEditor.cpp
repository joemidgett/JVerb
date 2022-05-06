/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JVerbAudioProcessorEditor::JVerbAudioProcessorEditor (JVerbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    jVerbDrySlider(*audioProcessor.apvts.getParameter("dryLevel_dB"), "dB"),
    jVerbLowGainSlider(*audioProcessor.apvts.getParameter("lowShelfBoostCut_dB"), "dB"),
    jVerbReverbTimeSlider(*audioProcessor.apvts.getParameter("kRT"), "Sec"),
    jVerbHighGainSlider(*audioProcessor.apvts.getParameter("highShelfBoostCut_dB"), "dB"),
    jVerbWetSlider(*audioProcessor.apvts.getParameter("wetLevel_dB"), "dB"),

    jVerbDrySliderAttachment(audioProcessor.apvts, "dryLevel_dB", jVerbDrySlider),
    jVerbLowGainSliderAttachment(audioProcessor.apvts, "lowShelfBoostCut_dB", jVerbLowGainSlider),
    jVerbReverbTimeSliderAttachment(audioProcessor.apvts, "kRT", jVerbReverbTimeSlider),
    jVerbHighGainSliderAttachment(audioProcessor.apvts, "highShelfBoostCut_dB", jVerbHighGainSlider),
    jVerbWetSliderAttachment(audioProcessor.apvts, "wetLevel_dB", jVerbWetSlider)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&jVerbLnf);

    addAndMakeVisible(jVerbDrySlider);
    addAndMakeVisible(jVerbLowGainSlider);
    addAndMakeVisible(jVerbReverbTimeSlider);
    addAndMakeVisible(jVerbHighGainSlider);
    addAndMakeVisible(jVerbWetSlider);

    //jVerbDrySlider.setColour()
    jVerbDrySlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::purple);
    jVerbDrySlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::mediumpurple);
    jVerbDrySlider.setColour(juce::Slider::thumbColourId, juce::Colours::rebeccapurple);
    jVerbDrySlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, juce::Colours::rebeccapurple);

    jVerbLowGainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::deeppink);
    jVerbLowGainSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::pink);
    jVerbLowGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::hotpink);
    jVerbLowGainSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, juce::Colours::deeppink);

    jVerbReverbTimeSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkblue);
    jVerbReverbTimeSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::lightblue);
    jVerbReverbTimeSlider.setColour(juce::Slider::thumbColourId, juce::Colours::cornflowerblue);
    jVerbReverbTimeSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, juce::Colours::darkblue);

    jVerbHighGainSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgreen);
    jVerbHighGainSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::lightgreen);
    jVerbHighGainSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightseagreen);
    jVerbHighGainSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, juce::Colours::darkgreen);

    jVerbWetSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::yellow);
    jVerbWetSlider.setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::lightyellow);
    jVerbWetSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightgoldenrodyellow);
    jVerbWetSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, juce::Colours::darkolivegreen);

    createJVerbLabel("Dry Gain - dB", jVerbDryLabel, jVerbDrySlider);
    createJVerbLabel("Low Gain - dB", jVerbLowGainLabel, jVerbLowGainSlider);
    createJVerbLabel("Reverb Time", jVerbReverbTimeLabel, jVerbReverbTimeSlider);
    createJVerbLabel("High Gain - dB", jVerbHighGainLabel, jVerbHighGainSlider);
    createJVerbLabel("Wet Gain - dB", jVerbWetLabel, jVerbWetSlider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (650, 195);
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

void JVerbAudioProcessorEditor::createJVerbLabel(const juce::String& name, juce::Label& label, JVerbSlider& slider)
{
    label.setText(name, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.attachToComponent(&slider, false);
    addAndMakeVisible(label);
}
