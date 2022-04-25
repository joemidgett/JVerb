// NestedDelayAPF.h

#pragma once

#include "DelayAPF.h"
#include "SignalGenData.h"
#include "DelayAPFParameters.h"

/**
\class NestedDelayAPF
\ingroup FX-Objects
\brief
The NestedDelayAPF object implements a pair of nested Delaying APF structures. These are labled the
outer and inner APFs. The outer APF's LPF and LFO may be optionally enabled. You might want
to extend this object to enable and use the inner LPF and LFO as well.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use BiquadParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class NestedDelayAPF : public DelayAPF
{
public:
    NestedDelayAPF(void) { }	/* C-TOR */
    ~NestedDelayAPF(void) { }	/* D-TOR */

public:
    /** reset members to initialized state */
    virtual bool reset(double _sampleRate)
    {
        // --- call base class reset first
        DelayAPF::reset(_sampleRate);

        // --- then do our stuff
        nestedAPF.reset(_sampleRate);

        return true;
    }

    /** process mono audio input */
    /**
    \param xn input
    \return the processed sample
    */
    virtual double processAudioSample(double xn)
    {
        // --- delay line output
        double wnD = 0.0;

        SimpleDelayParameters delayParams = delay.getParameters();
        if (delayParams.delay_Samples == 0)
            return xn;

        double apf_g = delayAPFParameters.apf_g;
        double lpf_g = delayAPFParameters.lpf_g;

        // --- for modulated APFs
        if (delayAPFParameters.enableLFO)
        {
            SignalGenData lfoOutput = modLFO.renderAudioOutput();
            double maxDelay = delayParams.delayTime_mSec;
            double minDelay = maxDelay - delayAPFParameters.lfoMaxModulation_mSec;
            minDelay = fmax(0.0, minDelay); // bound minDelay to 0 as minimum
            double lfoDepth = delayAPFParameters.lfoDepth;

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

        // --- form w(n) = x(n) + gw(n-D)
        double wn = xn + apf_g * wnD;

        // --- process wn through inner APF
        double ynInner = nestedAPF.processAudioSample(wn);

        // --- form y(n) = -gw(n) + w(n-D)
        double yn = -apf_g * wn + wnD;

        // --- underflow check
        checkFloatUnderflow(yn);

        // --- write delay line
        delay.writeDelay(ynInner);

        return yn;
    }

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return BiquadParameters custom data structure
    */
    NestedDelayAPFParameters getParameters() { return nestedAPFParameters; }

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param BiquadParameters custom data structure
    */
    void setParameters(const NestedDelayAPFParameters& params)
    {
        nestedAPFParameters = params;

        DelayAPFParameters outerAPFParameters = DelayAPF::getParameters();
        DelayAPFParameters innerAPFParameters = nestedAPF.getParameters();

        // --- outer APF
        outerAPFParameters.apf_g = nestedAPFParameters.outerAPF_g;
        outerAPFParameters.delayTime_mSec = nestedAPFParameters.outerAPFdelayTime_mSec;

        // --- LFO support
        outerAPFParameters.enableLFO = nestedAPFParameters.enableLFO;
        outerAPFParameters.lfoDepth = nestedAPFParameters.lfoDepth;
        outerAPFParameters.lfoRate_Hz = nestedAPFParameters.lfoRate_Hz;
        outerAPFParameters.lfoMaxModulation_mSec = nestedAPFParameters.lfoMaxModulation_mSec;

        // --- inner APF
        innerAPFParameters.apf_g = nestedAPFParameters.innerAPF_g;
        innerAPFParameters.delayTime_mSec = nestedAPFParameters.innerAPFdelayTime_mSec;

        DelayAPF::setParameters(outerAPFParameters);
        nestedAPF.setParameters(innerAPFParameters);
    }

    /** createDelayBuffers -- note there are two delay times here for inner and outer APFs*/
    void createDelayBuffers(double _sampleRate, double delay_mSec, double nestedAPFDelay_mSec)
    {
        // --- base class
        DelayAPF::createDelayBuffer(_sampleRate, delay_mSec);

        // --- then our stuff
        nestedAPF.createDelayBuffer(_sampleRate, nestedAPFDelay_mSec);
    }

private:
    NestedDelayAPFParameters nestedAPFParameters; ///< object parameters
    DelayAPF nestedAPF;	///< nested APF object
};