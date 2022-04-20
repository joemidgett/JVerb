// LFO.h

#pragma once

#include <JuceHeader.h>
#include "IAudioSignalGenerator.h"
#include "OscillatorParameters.h"
#include "SignalGenData.h"
#include "Utilities.h"

/**
\class LFO
\ingroup FX-Objects
\brief
The LFO object implements a mathematically perfect LFO generator for modulation uses only. It should not be used for
audio frequencies except for the sinusoidal output which, though an approximation, has very low TDH.

Audio I/O:
- Output only object: low frequency generator.

Control I/F:
- Use OscillatorParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class LFO : public IAudioSignalGenerator
{
public:
	LFO() { srand(time(NULL)); }	/* C-TOR */
	virtual ~LFO() {}				/* D-TOR */

	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		sampleRate = _sampleRate;
		phaseInc = lfoParameters.frequency_Hz / sampleRate;

		// --- timebase variables
		modCounter = 0.0;			///< modulo counter [0.0, +1.0]
		modCounterQP = 0.25;		///<Quad Phase modulo counter [0.0, +1.0]

		return true;
	}

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return OscillatorParameters custom data structure
	*/
	OscillatorParameters getParameters() { return lfoParameters; }

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param OscillatorParameters custom data structure
	*/
	void setParameters(const OscillatorParameters& params)
	{
		if (params.frequency_Hz != lfoParameters.frequency_Hz)
			// --- update phase inc based on osc freq and fs
			phaseInc = params.frequency_Hz / sampleRate;

		lfoParameters = params;
	}

	/** render a new audio output structure */
	virtual const SignalGenData renderAudioOutput();

protected:
	// --- parameters
	OscillatorParameters lfoParameters; ///< obejcgt parameters

	// --- sample rate
	double sampleRate = 0.0;			///< sample rate

	// --- timebase variables
	double modCounter = 0.0;			///< modulo counter [0.0, +1.0]
	double phaseInc = 0.0;				///< phase inc = fo/fs
	double modCounterQP = 0.25;			///<Quad Phase modulo counter [0.0, +1.0]

	/** check the modulo counter and wrap if needed */
	inline bool checkAndWrapModulo(double& moduloCounter, double phaseInc)
	{
		// --- for positive frequencies
		if (phaseInc > 0 && moduloCounter >= 1.0)
		{
			moduloCounter -= 1.0;
			return true;
		}

		// --- for negative frequencies
		if (phaseInc < 0 && moduloCounter <= 0.0)
		{
			moduloCounter += 1.0;
			return true;
		}

		return false;
	}

	/** advanvce the modulo counter, then check the modulo counter and wrap if needed */
	inline bool advanceAndCheckWrapModulo(double& moduloCounter, double phaseInc)
	{
		// --- advance counter
		moduloCounter += phaseInc;

		// --- for positive frequencies
		if (phaseInc > 0 && moduloCounter >= 1.0)
		{
			moduloCounter -= 1.0;
			return true;
		}

		// --- for negative frequencies
		if (phaseInc < 0 && moduloCounter <= 0.0)
		{
			moduloCounter += 1.0;
			return true;
		}

		return false;
	}

	/** advanvce the modulo counter */
	inline void advanceModulo(double& moduloCounter, double phaseInc) { moduloCounter += phaseInc; }

	const double B = 4.0 / kPi;
	const double C = -4.0 / (kPi * kPi);
	const double P = 0.225;
	/** parabolic sinusoidal calcualtion; NOTE: input is -pi to +pi http://devmaster.net/posts/9648/fast-and-accurate-sine-cosine */
	inline double parabolicSine(double angle)
	{
		double y = B * angle + C * angle * fabs(angle);
		y = P * (y * fabs(y) - y) + y;
		return y;
	}
};