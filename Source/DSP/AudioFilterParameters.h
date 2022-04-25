// AudioFilterParameters.h

#pragma once

/**
\struct AudioFilterParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the AudioFilter object.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct AudioFilterParameters
{
    AudioFilterParameters() {}
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    AudioFilterParameters& operator=(const AudioFilterParameters& params)	// need this override for collections to work
    {
        if (this == &params)
            return *this;
        algorithm = params.algorithm;
        fc = params.fc;
        Q = params.Q;
        boostCut_dB = params.boostCut_dB;

        return *this;
    }

    // --- individual parameters
    filterAlgorithm algorithm = filterAlgorithm::kLPF1; ///< filter algorithm
    double fc = 100.0; ///< filter cutoff or center frequency (Hz)
    double Q = 0.707; ///< filter Q
    double boostCut_dB = 0.0; ///< filter gain; note not used in all types
};