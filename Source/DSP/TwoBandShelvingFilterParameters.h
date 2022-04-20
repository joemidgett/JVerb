// TwoBandShelvingFilterParameters.h

#pragma once

/**
\struct TwoBandShelvingFilterParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the TwoBandShelvingFilter object. Used for reverb algorithms in book.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct TwoBandShelvingFilterParameters
{
	TwoBandShelvingFilterParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	TwoBandShelvingFilterParameters& operator=(const TwoBandShelvingFilterParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		lowShelf_fc = params.lowShelf_fc;
		lowShelfBoostCut_dB = params.lowShelfBoostCut_dB;
		highShelf_fc = params.highShelf_fc;
		highShelfBoostCut_dB = params.highShelfBoostCut_dB;
		return *this;
	}

	// --- individual parameters
	double lowShelf_fc = 0.0;			///< fc for low shelf
	double lowShelfBoostCut_dB = 0.0;	///< low shelf gain
	double highShelf_fc = 0.0;			///< fc for high shelf
	double highShelfBoostCut_dB = 0.0;	///< high shelf gain
};