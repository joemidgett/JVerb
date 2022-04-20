// SimpleLPFParameters.h

#pragma once

/**
\struct SimpleLPFParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the SimpleLPFP object. Used for reverb algorithms in book.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct SimpleLPFParameters
{
	SimpleLPFParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	SimpleLPFParameters& operator=(const SimpleLPFParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		g = params.g;
		return *this;
	}

	// --- individual parameters
	double g = 0.0; ///< simple LPF g value
};