// SimpleDelay.h

#pragma once

#include "CircularBuffer.h"

/**
\class SimpleDelay
\ingroup FX-Objects
\brief
The SimpleDelay object implements a basic delay line without feedback.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use SimpleDelayParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class SimpleDelay : public IAudioSignalProcessor
{
public:
    SimpleDelay(void) {}	/* C-TOR */
    ~SimpleDelay(void) {}	/* D-TOR */

public:
    /** reset members to initialized state */
    virtual bool reset(double _sampleRate)
    {
        // --- if sample rate did not change
        if (sampleRate == _sampleRate)
        {
            // --- just flush buffer and return
            delayBuffer.flushBuffer();
            return true;
        }

        // --- create new buffer, will store sample rate and length(mSec)
        createDelayBuffer(_sampleRate, bufferLength_mSec);

        return true;
    }

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return SimpleDelayParameters custom data structure
    */
    SimpleDelayParameters getParameters()
    {
        return simpleDelayParameters;
    }

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param SimpleDelayParameters custom data structure
    */
    void setParameters(const SimpleDelayParameters& params)
    {
        simpleDelayParameters = params;
        simpleDelayParameters.delay_Samples = simpleDelayParameters.delayTime_mSec * (samplesPerMSec);
        delayBuffer.setInterpolate(simpleDelayParameters.interpolate);
    }

    /** process MONO audio delay */
    /**
    \param xn input
    \return the processed sample
    */
    virtual double processAudioSample(double xn)
    {
        // --- read delay
        if (simpleDelayParameters.delay_Samples == 0)
            return xn;

        double yn = delayBuffer.readBuffer(simpleDelayParameters.delay_Samples);

        // --- write to delay buffer
        delayBuffer.writeBuffer(xn);

        // --- done
        return yn;
    }

    /** reset members to initialized state */
    virtual bool canProcessAudioFrame() { return false; }

    /** create a new delay buffer */
    void createDelayBuffer(double _sampleRate, double _bufferLength_mSec)
    {
        // --- store for math
        bufferLength_mSec = _bufferLength_mSec;
        sampleRate = _sampleRate;
        samplesPerMSec = sampleRate / 1000.0;

        // --- total buffer length including fractional part
        bufferLength = (unsigned int)(bufferLength_mSec * (samplesPerMSec)) + 1; // +1 for fractional part

        // --- create new buffer
        delayBuffer.createCircularBuffer(bufferLength);
    }

    /** read delay at current location */
    double readDelay()
    {
        // --- simple read
        return delayBuffer.readBuffer(simpleDelayParameters.delay_Samples);
    }

    /** read delay at current location */
    double readDelayAtTime_mSec(double _delay_mSec)
    {
        // --- calculate total delay time in samples + fraction
        double _delay_Samples = _delay_mSec * (samplesPerMSec);

        // --- simple read
        return delayBuffer.readBuffer(_delay_Samples);
    }

    /** read delay at a percentage of total length */
    double readDelayAtPercentage(double delayPercent)
    {
        // --- simple read
        return delayBuffer.readBuffer((delayPercent / 100.0) * simpleDelayParameters.delay_Samples);
    }

    /** write a new value into the delay */
    void writeDelay(double xn)
    {
        // --- simple write
        delayBuffer.writeBuffer(xn);
    }

private:
    SimpleDelayParameters simpleDelayParameters; ///< object parameters

    double sampleRate = 0.0;		///< sample rate
    double samplesPerMSec = 0.0;	///< samples per millisecond (for arbitrary access)
    double bufferLength_mSec = 0.0; ///< total buffer lenth in mSec
    unsigned int bufferLength = 0;	///< buffer length in samples

    // --- delay buffer of doubles
    CircularBuffer<double> delayBuffer; ///< circular buffer for delay
};