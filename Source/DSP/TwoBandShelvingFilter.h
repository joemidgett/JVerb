// TwoBandShelvingFilter.h

#pragma once

#include "AudioFilter.h"

/**
\class TwoBandShelvingFilter
\ingroup FX-Objects
\brief
The TwoBandShelvingFilter object implements two shelving filters in series in the standard "Bass and Treble" configuration.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use TwoBandShelvingFilterParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class TwoBandShelvingFilter : public IAudioSignalProcessor
{
public:
	TwoBandShelvingFilter()
	{
		AudioFilterParameters params = lowShelfFilter.getParameters();
		params.algorithm = filterAlgorithm::kLowShelf;
		lowShelfFilter.setParameters(params);

		params = highShelfFilter.getParameters();
		params.algorithm = filterAlgorithm::kHiShelf;
		highShelfFilter.setParameters(params);
	}		/* C-TOR */

	~TwoBandShelvingFilter() {}		/* D-TOR */

	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		lowShelfFilter.reset(_sampleRate);
		highShelfFilter.reset(_sampleRate);
		return true;
	}

	/** return false: this object only processes samples */
	virtual bool canProcessAudioFrame() { return false; }

	/** process a single input through the two filters in series */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		// --- all modes do Full Wave Rectification
		double filteredSignal = lowShelfFilter.processAudioSample(xn);
		filteredSignal = highShelfFilter.processAudioSample(filteredSignal);

		return filteredSignal;
	}

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return TwoBandShelvingFilterParameters custom data structure
	*/
	TwoBandShelvingFilterParameters getParameters()
	{
		return parameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param TwoBandShelvingFilterParameters custom data structure
	*/
	void setParameters(const TwoBandShelvingFilterParameters& params)
	{
		parameters = params;
		AudioFilterParameters filterParams = lowShelfFilter.getParameters();
		filterParams.fc = parameters.lowShelf_fc;
		filterParams.boostCut_dB = parameters.lowShelfBoostCut_dB;
		lowShelfFilter.setParameters(filterParams);

		filterParams = highShelfFilter.getParameters();
		filterParams.fc = parameters.highShelf_fc;
		filterParams.boostCut_dB = parameters.highShelfBoostCut_dB;
		highShelfFilter.setParameters(filterParams);
	}

private:
	TwoBandShelvingFilterParameters parameters; ///< object parameters
	AudioFilter lowShelfFilter;					///< filter for low shelf
	AudioFilter highShelfFilter;				///< filter for high shelf
};