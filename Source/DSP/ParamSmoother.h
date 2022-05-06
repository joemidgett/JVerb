// ParamSmoother.h

#pragma once

#include <JuceHeader.h>

class ParamSmoother
{
public:
    void initializeSmoothing(float smoothingTimeInMs, float samplingRate);
    float processSmoothing(float inputValue);

private:
    float a;
    float b;
    float outputValue;
};
