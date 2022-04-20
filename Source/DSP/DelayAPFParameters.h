// DelayAPFParameters.h

#pragma once

/**
\struct DelayAPFParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the DelayAPF object. Used for reverb algorithms in book.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct DelayAPFParameters
{
	DelayAPFParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	DelayAPFParameters& operator=(const DelayAPFParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		delayTime_mSec = params.delayTime_mSec;
		apf_g = params.apf_g;
		enableLPF = params.enableLPF;
		lpf_g = params.lpf_g;
		interpolate = params.interpolate;
		enableLFO = params.enableLFO;
		lfoRate_Hz = params.lfoRate_Hz;
		lfoDepth = params.lfoDepth;
		lfoMaxModulation_mSec = params.lfoMaxModulation_mSec;
		return *this;
	}

	// --- individual parameters
	double delayTime_mSec = 0.0;	///< APF delay time
	double apf_g = 0.0;				///< APF g coefficient
	bool enableLPF = false;			///< flag to enable LPF in structure
	double lpf_g = 0.0;				///< LPF g coefficient (if enabled)
	bool interpolate = false;		///< interpolate flag (diagnostics)
	bool enableLFO = false;			///< flag to enable LFO
	double lfoRate_Hz = 0.0;		///< LFO rate in Hz, if enabled
	double lfoDepth = 0.0;			///< LFO deoth (not in %) if enabled
	double lfoMaxModulation_mSec = 0.0;	///< LFO maximum modulation time in mSec
};