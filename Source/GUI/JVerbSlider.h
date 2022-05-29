// JVerbSlider.h

#pragma once

#include <JuceHeader.h>

class JVerbSlider : public juce::Slider
{
public:
    JVerbSlider(juce::RangedAudioParameter& rap, const juce::String& unitSuffix);
};
