// DelayAPF.h

#pragma once

#include "LFO.h"
#include "DelayAPFParameters.h"

/**
\class DelayAPF
\ingroup FX-Objects
\brief
The DelayAPF object implements a delaying APF with optional LPF and optional modulated delay time with LFO.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use DelayAPFParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class DelayAPF : public IAudioSignalProcessor
{
public:
	DelayAPF(void) {}	/* C-TOR */
	~DelayAPF(void) {}	/* D-TOR */

public:
	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// --- reset children
		modLFO.reset(_sampleRate);

		// --- flush
		lpf_state = 0.0;

		// --- create new buffer, will store sample rate and length(mSec)
		createDelayBuffer(sampleRate, bufferLength_mSec);

		return true;
	}

	/** process one input sample through object */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		SimpleDelayParameters delayParams = delay.getParameters();
		if (delayParams.delay_Samples == 0)
			return xn;

		// --- delay line output
		double wnD = 0.0;
		double apf_g = delayAPFParameters.apf_g;
		double lpf_g = delayAPFParameters.lpf_g;
		double lfoDepth = delayAPFParameters.lfoDepth;

		// --- for modulated APFs
		if (delayAPFParameters.enableLFO)
		{
			SignalGenData lfoOutput = modLFO.renderAudioOutput();
			double maxDelay = delayParams.delayTime_mSec;
			double minDelay = maxDelay - delayAPFParameters.lfoMaxModulation_mSec;
			minDelay = fmax(0.0, minDelay); // bound minDelay to 0 as minimum

			// --- calc max-down modulated value with unipolar converted LFO output
			//     NOTE: LFO output is scaled by lfoDepth
			double modDelay_mSec = doUnipolarModulationFromMax(bipolarToUnipolar(lfoDepth * lfoOutput.normalOutput),
				minDelay, maxDelay);

			// --- read modulated value to get w(n-D);
			wnD = delay.readDelayAtTime_mSec(modDelay_mSec);
		}
		else
			// --- read the delay line to get w(n-D)
			wnD = delay.readDelay();

		if (delayAPFParameters.enableLPF)
		{
			// --- apply simple 1st order pole LPF, overwrite wnD
			wnD = wnD * (1.0 - lpf_g) + lpf_g * lpf_state;
			lpf_state = wnD;
		}

		// form w(n) = x(n) + gw(n-D)
		double wn = xn + apf_g * wnD;

		// form y(n) = -gw(n) + w(n-D)
		double yn = -apf_g * wn + wnD;

		// underflow check
		checkFloatUnderflow(yn);

		// write delay line
		delay.writeDelay(wn);

		return yn;
	}

	/** return false: this object only processes samples */
	virtual bool canProcessAudioFrame() { return false; }

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return DelayAPFParameters custom data structure
	*/
	DelayAPFParameters getParameters()
	{
		return delayAPFParameters;
	}

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param DelayAPFParameters custom data structure
	*/
	void setParameters(const DelayAPFParameters& params)
	{
		delayAPFParameters = params;

		// --- update delay line
		SimpleDelayParameters delayParams = delay.getParameters();
		delayParams.delayTime_mSec = delayAPFParameters.delayTime_mSec;
		delay.setParameters(delayParams);
	}

	/** create the delay buffer in mSec */
	void createDelayBuffer(double _sampleRate, double delay_mSec)
	{
		sampleRate = _sampleRate;
		bufferLength_mSec = delay_mSec;

		// --- create new buffer, will store sample rate and length(mSec)
		delay.createDelayBuffer(_sampleRate, delay_mSec);
	}

protected:
	// --- component parameters
	DelayAPFParameters delayAPFParameters;	///< obeject parameters
	double sampleRate = 0.0;				///< current sample rate
	double bufferLength_mSec = 0.0;			///< total buffer length in mSec

	// --- delay buffer of doubles
	SimpleDelay delay;						///< delay

	// --- optional LFO
	LFO modLFO;								///< LFO

	// --- LPF support
	double lpf_state = 0.0;					///< LPF state register (z^-1)
};