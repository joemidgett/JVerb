// ParamSmoother.cpp

#include "ParamSmoother.h"

void ParamSmoother::initializeSmoothing(float smoothingTimeInMs, float samplingRate)
{
    const float twoPi = juce::MathConstants<float>::twoPi;

    a = exp(-twoPi / (smoothingTimeInMs * 0.001f * samplingRate));
    b = 1.0f - a;
    outputValue = 0.0f;
}

float ParamSmoother::processSmoothing(float inputValue)
{
    outputValue = (inputValue * b) + (outputValue * a);
    return outputValue;
}
