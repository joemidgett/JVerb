// JVerbLookAndFeel.h

#pragma once

#include <JuceHeader.h>

class JVerbLookAndFeel : public juce::LookAndFeel_V4
{
public:

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                     float sliderPosProportional, float rotaryStartAngle, 
                     float rotaryEndAngle, juce::Slider&);
};
