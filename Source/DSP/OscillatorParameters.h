// OscillatorParameters.h

#pragma once

#include "Utilities.h"

/**
\struct OscillatorParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the LFO and DFOscillator objects.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct OscillatorParameters
{
    OscillatorParameters() {}
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    OscillatorParameters& operator=(const OscillatorParameters& params)	// need this override for collections to work
    {
        if (this == &params)
            return *this;

        waveform = params.waveform;
        frequency_Hz = params.frequency_Hz;
        return *this;
    }

    // --- individual parameters
    generatorWaveform waveform = generatorWaveform::kTriangle; ///< the current waveform
    double frequency_Hz = 0.0;	///< oscillator frequency
};