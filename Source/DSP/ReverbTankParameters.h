// ReverbTankParameters.h

#pragma once

/**
\struct ReverbTankParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the ReverbTank object.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct ReverbTankParameters
{
	ReverbTankParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	ReverbTankParameters& operator=(const ReverbTankParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		density = params.density;

		// --- tweaker variables
		apfDelayMax_mSec = params.apfDelayMax_mSec;
		apfDelayWeight_Pct = params.apfDelayWeight_Pct;
		fixeDelayMax_mSec = params.fixeDelayMax_mSec;
		fixeDelayWeight_Pct = params.fixeDelayWeight_Pct;
		preDelayTime_mSec = params.preDelayTime_mSec;

		lpf_g = params.lpf_g;
		kRT = params.kRT;

		lowShelf_fc = params.lowShelf_fc;
		lowShelfBoostCut_dB = params.lowShelfBoostCut_dB;
		highShelf_fc = params.highShelf_fc;
		highShelfBoostCut_dB = params.highShelfBoostCut_dB;

		wetLevel_dB = params.wetLevel_dB;
		dryLevel_dB = params.dryLevel_dB;
		return *this;
	}

	// --- individual parameters
	reverbDensity density = reverbDensity::kThick;	///< density setting thick or thin

	// --- tweaking parameters - you may not want to expose these
	//     in the final plugin!
	// --- See the book for all the details on how these tweakers work!!
	double apfDelayMax_mSec = 5.0;					///< APF max delay time
	double apfDelayWeight_Pct = 100.0;				///< APF max delay weighying
	double fixeDelayMax_mSec = 50.0;				///< fixed delay max time
	double fixeDelayWeight_Pct = 100.0;				///< fixed delay max weighying

	// --- direct control parameters
	double preDelayTime_mSec = 0.0;					///< pre-delay time in mSec
	double lpf_g = 0.0;								///< LPF g coefficient
	double kRT = 0.0;								///< reverb time, 0 to 1

	double lowShelf_fc = 0.0;						///< low shelf fc
	double lowShelfBoostCut_dB = 0.0;				///< low shelf gain
	double highShelf_fc = 0.0;						///< high shelf fc
	double highShelfBoostCut_dB = 0.0;				///< high shelf gain

	double wetLevel_dB = -3.0;						///< wet output level in dB
	double dryLevel_dB = -3.0;						///< dry output level in dB
};