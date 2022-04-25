// LFO.cpp

#include "LFO.h"

/**
\brief generates the oscillator output for one sample interval; note that there are multiple outputs.
*/
const SignalGenData LFO::renderAudioOutput()
{
    // --- always first!
    checkAndWrapModulo(modCounter, phaseInc);

    // --- QP output always follows location of current modulo; first set equal
    modCounterQP = modCounter;

    // --- then, advance modulo by quadPhaseInc = 0.25 = 90 degrees, AND wrap if needed
    advanceAndCheckWrapModulo(modCounterQP, 0.25);

    SignalGenData output;
    generatorWaveform waveform = lfoParameters.waveform;

    // --- calculate the oscillator value
    if (waveform == generatorWaveform::kSin)
    {
        // --- calculate normal angle
        double angle = modCounter * 2.0 * kPi - kPi;

        // --- norm output with parabolicSine approximation
        output.normalOutput = parabolicSine(-angle);

        // --- calculate QP angle
        angle = modCounterQP * 2.0 * kPi - kPi;

        // --- calc QP output
        output.quadPhaseOutput_pos = parabolicSine(-angle);
    }
    else if (waveform == generatorWaveform::kTriangle)
    {
        // triv saw
        output.normalOutput = unipolarToBipolar(modCounter);

        // bipolar triagle
        output.normalOutput = 2.0 * fabs(output.normalOutput) - 1.0;

        // -- quad phase
        output.quadPhaseOutput_pos = unipolarToBipolar(modCounterQP);

        // bipolar triagle
        output.quadPhaseOutput_pos = 2.0 * fabs(output.quadPhaseOutput_pos) - 1.0;
    }
    else if (waveform == generatorWaveform::kSaw)
    {
        output.normalOutput = unipolarToBipolar(modCounter);
        output.quadPhaseOutput_pos = unipolarToBipolar(modCounterQP);
    }

    // --- invert two main outputs to make the opposite versions
    output.quadPhaseOutput_neg = -output.quadPhaseOutput_pos;
    output.invertedOutput = -output.normalOutput;

    // --- setup for next sample period
    advanceModulo(modCounter, phaseInc);

    return output;
}
