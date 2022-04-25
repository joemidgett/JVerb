// SimpleLPF.h

#pragma once

/**
\class SimpleLPF
\ingroup FX-Objects
\brief
The SimpleLPF object implements a first order one-pole LPF using one coefficient "g" value.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use SimpleLPFParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class SimpleLPF : public IAudioSignalProcessor
{
public:
    SimpleLPF(void) {}	/* C-TOR */
    ~SimpleLPF(void) {}	/* D-TOR */

public:
    /** reset members to initialized state */
    virtual bool reset(double _sampleRate)
    {
        state = 0.0;
        return true;
    }

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return SimpleLPFParameters custom data structure
    */
    SimpleLPFParameters getParameters()
    {
        return simpleLPFParameters;
    }

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param SimpleLPFParameters custom data structure
    */
    void setParameters(const SimpleLPFParameters& params)
    {
        simpleLPFParameters = params;
    }

    /** process simple one pole FB back filter */
    /**
    \param xn input
    \return the processed sample
    */
    virtual double processAudioSample(double xn)
    {
        double g = simpleLPFParameters.g;
        double yn = (1.0 - g) * xn + g * state;
        state = yn;
        return yn;
    }

    /** return false: this object only processes samples */
    virtual bool canProcessAudioFrame() { return false; }

private:
    SimpleLPFParameters simpleLPFParameters;	///< object parameters
    double state = 0.0;							///< single state (z^-1) register
};