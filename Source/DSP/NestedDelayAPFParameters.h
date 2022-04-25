// NestedDelayAPFParameters.h

#pragma once

/**
\struct NestedDelayAPFParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the NestedDelayAPF object. Used for reverb algorithms in book.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct NestedDelayAPFParameters
{
    NestedDelayAPFParameters() {}
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    NestedDelayAPFParameters& operator=(const NestedDelayAPFParameters& params)	// need this override for collections to work
    {
        if (this == &params)
            return *this;

        outerAPFdelayTime_mSec = params.outerAPFdelayTime_mSec;
        innerAPFdelayTime_mSec = params.innerAPFdelayTime_mSec;
        outerAPF_g = params.outerAPF_g;
        innerAPF_g = params.innerAPF_g;

        // --- outer LFO
        enableLFO = params.enableLFO;
        lfoRate_Hz = params.lfoRate_Hz;
        lfoDepth = params.lfoDepth;
        lfoMaxModulation_mSec = params.lfoMaxModulation_mSec;

        return *this;
    }

    // --- individual parameters
    double outerAPFdelayTime_mSec = 0.0;	///< delay time for outer APF
    double innerAPFdelayTime_mSec = 0.0;	///< delay time for inner APF
    double outerAPF_g = 0.0;				///< g coefficient for outer APF
    double innerAPF_g = 0.0;				///< g coefficient for inner APF

    // --- this LFO belongs to the outer APF only
    bool enableLFO = false;					///< flag to enable the modulated delay
    double lfoRate_Hz = 0.0;				///< LFO rate in Hz (if enabled)
    double lfoDepth = 1.0;					///< LFO depth (not in %) (if enabled)
    double lfoMaxModulation_mSec = 0.0;		///< max modulation time if LFO is enabled
};