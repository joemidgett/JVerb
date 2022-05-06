// JVerbSlider.cpp

#include "JVerbSlider.h"

JVerbSlider::JVerbSlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix)
{
	auto jVerbRotarySliderParams = getRotaryParameters();
	jVerbRotarySliderParams.startAngleRadians = juce::MathConstants<float>::pi * 1.2f;
	jVerbRotarySliderParams.endAngleRadians = juce::MathConstants<float>::pi * 2.8f;
	setRotaryParameters(jVerbRotarySliderParams);
	
	setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	setRange(0.0f, 100.0f);
	setValue(25.0f);
	setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 25);
	
	setColour(juce::Slider::ColourIds::rotarySliderFillColourId, juce::Colours::orange);
	setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::orange);
	setColour(juce::Slider::thumbColourId, juce::Colours::orange);
	setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, juce::Colours::black);
	setColour(juce::Slider::ColourIds::textBoxHighlightColourId, juce::Colours::darkgrey);
}
