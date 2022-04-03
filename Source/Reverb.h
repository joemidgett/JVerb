// Reverb.h

#pragma once

#include <memory>
#include <time.h>

// --- constants & enumerations
//
// ---  by placing outside the class declaration, these will also be avaialble to anything
//      that #includes this file (sometimes needed)
const double kSmallestPositiveFloatValue = 1.175494351e-38;         /* min positive value */
const double kSmallestNegativeFloatValue = -1.175494351e-38;         /* min negative value */

// --- prevent accidental double inclusion
#ifndef _guiconstants_h
/**
	@kPi
	\ingroup Constants-Enums
	@brief pi to 80 decimal places
*/
const double kPi = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899;

#endif

/**
@convertIntToEnum
\ingroup ASPiK-GUI
\def convertIntToEnum
@brief convert an int to an enum, e.g. for passing to functions
\param INT - the int to compare with
\param ENUM - the typed enum to compare with
\return the int value properly cast as the enum type
*/
#define convertIntToEnum(INT,ENUM) static_cast<ENUM>(INT)

/**
@checkFloatUnderflow
\ingroup FX-Functions

@brief Perform underflow check; returns true if we did underflow (user may not care)

\param value - the value to check for underflow
\return true if overflowed, false otherwise
*/
inline bool checkFloatUnderflow(double& value)
{
	bool retValue = false;
	if (value > 0.0 && value < kSmallestPositiveFloatValue)
	{
		value = 0;
		retValue = true;
	}
	else if (value < 0.0 && value > kSmallestNegativeFloatValue)
	{
		value = 0;
		retValue = true;
	}
	return retValue;
}

/**
@boundValue
\ingroup FX-Functions

@brief  Bound a value to min and max limits

\param value - value to bound
\param minValue - lower bound limit
\param maxValue - upper bound limit
*/
inline void boundValue(double& value, double minValue, double maxValue)
{
	value = fmin(value, maxValue);
	value = fmax(value, minValue);
}

/**
@doUnipolarModulationFromMax
\ingroup FX-Functions

@brief Perform unipolar modulation from a max value down to a min value using a unipolar modulator value

\param unipolarModulatorValue - modulation value on range [0.0, +1.0]
\param minValue - lower modulation limit
\param maxValue - upper modulation limit
\return the modulated value
*/
inline double doUnipolarModulationFromMax(double unipolarModulatorValue, double minValue, double maxValue)
{
	// --- UNIPOLAR bound
	boundValue(unipolarModulatorValue, 0.0, 1.0);

	// --- modulate from maximum value downwards
	return maxValue - (1.0 - unipolarModulatorValue) * (maxValue - minValue);
}

/**
@bipolarToUnipolar
\ingroup FX-Functions

@brief calculates the unipolar [0.0, +1.0] value FROM a bipolar [-1.0, +1.0] value

\param value - value to convert
\return the unipolar value
*/
inline double bipolarToUnipolar(double value)
{
	return 0.5 * value + 0.5;
}

/**
\class IAudioSignalProcessor
\ingroup Interfaces
\brief
Use this interface for objects that process audio input samples to produce audio output samples. A derived class must implement the three abstract methods. The others are optional.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class IAudioSignalProcessor
{
public:
	// --- pure virtual, derived classes must implement or will not compile
	//     also means this is a pure abstract base class and is incomplete,
	//     so it can only be used as a base class
	//
	/** initialize the object with the new sample rate */
	virtual bool reset(double _sampleRate) = 0;

	/** process one sample in and out */
	virtual double processAudioSample(double xn) = 0;

	/** return true if the derived object can process a frame, false otherwise */
	virtual bool canProcessAudioFrame() = 0;

	/** set or change the sample rate; normally this is done during reset( ) but may be needed outside of initialzation */
	virtual void setSampleRate(double _sampleRate) {}

	/** switch to enable/disable the aux input */
	virtual void enableAuxInput(bool enableAuxInput) {}

	/** for processing objects with a sidechain input or other necessary aux input
			the return value is optional and will depend on the subclassed object */
	virtual double processAuxInputAudioSample(double xn)
	{
		// --- do nothing
		return xn;
	}

	/** for processing objects with a sidechain input or other necessary aux input
	--- optional processing function
		e.g. does not make sense for some objects to implement this such as inherently mono objects like Biquad
			 BUT a processor that must use both left and right channels (ping-pong delay) would require it */
	virtual bool processAudioFrame(const float* inputFrame,		/* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
		float* outputFrame,
		uint32_t inputChannels,
		uint32_t outputChannels)
	{
		// --- do nothing
		return false; // NOT handled
	}
};

/**
\struct SignalGenData
\ingroup Structures
\brief
This is the output structure for audio generator objects that can render up to four outputs.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
// --- structure to send output data from signal gen; you can add more outputs here
struct SignalGenData
{
	SignalGenData() {}

	double normalOutput = 0.0;			///< normal
	double invertedOutput = 0.0;		///< inverted
	double quadPhaseOutput_pos = 0.0;	///< 90 degrees out
	double quadPhaseOutput_neg = 0.0;	///< -90 degrees out
};

/**
\class IAudioSignalGenerator
\ingroup Interfaces
\brief
Use this interface for objects that render an output without an input, such as oscillators. May also be used for envelope generators whose input is a note-on or other switchable event.

Outpput I/F:
- Use SignalGenData structure for output.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class IAudioSignalGenerator
{
public:
	// --- pure virtual, derived classes must implement or will not compile
	//     also means this is a pure abstract base class and is incomplete,
	//     so it can only be used as a base class
	//
	/** Sample rate may or may not be required, but usually is */
	virtual bool reset(double _sampleRate) = 0;

	/** render the generator output */
	virtual const SignalGenData renderAudioOutput() = 0;
};

/**
\enum stateReg
\ingroup Constants-Enums
\brief
Use this enum to easily access z^-1 state values inside of arrays. For some structures, fewer storage units are needed. They are divided as follows:

- Direct Forms: we will allow max of 2 for X (feedforward) and 2 for Y (feedback) data
- Transpose Forms: we will use ONLY the x_z1 and x_z2 registers for the 2 required delays
- enum stateReg { x_z1, x_z2, y_z1, y_z2, numStates };

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/

// --- state array index values
//     z^-1 registers;
//        Direct Forms: we will allow max of 2 for X (feedforward) and 2 for Y (feedback) data
//        Transpose Forms: we will use ONLY the x_z1 and x_z2 registers for the 2 required delays
enum stateReg { x_z1, x_z2, y_z1, y_z2, numStates };

/**
\enum filterCoeff
\ingroup Constants-Enums
\brief
Use this enum to easily access coefficents inside of arrays.

- enum filterCoeff { a0, a1, a2, b1, b2, c0, d0, numCoeffs };

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
enum filterCoeff { a0, a1, a2, b1, b2, c0, d0, numCoeffs };

/**
\enum biquadAlgorithm
\ingroup Constants-Enums
\brief
Use this strongly typed enum to easily set the biquad calculation type

- enum class biquadAlgorithm { kDirect, kCanonical, kTransposeDirect, kTransposeCanonical }; //  4 types of biquad calculations, constants (k)

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/

// --- type of calculation (algorithm)
enum class biquadAlgorithm { kDirect, kCanonical, kTransposeDirect, kTransposeCanonical }; //  4 types of biquad calculations, constants (k)

/**
\struct BiquadParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the Biquad object. Default version defines the biquad structure used in the calculation.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct BiquadParameters
{
	BiquadParameters() {}

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	BiquadParameters& operator=(const BiquadParameters& params)
	{
		if (this == &params)
			return *this;

		biquadCalcType = params.biquadCalcType;
		return *this;
	}

	biquadAlgorithm biquadCalcType = biquadAlgorithm::kDirect; ///< biquad structure to use
};

/**
\class Biquad
\ingroup FX-Objects
\brief
The Biquad object implements a first or second order H(z) transfer function using one of four standard structures: Direct, Canonical, Transpose Direct, Transpose Canonical.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use BiquadParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class Biquad : public IAudioSignalProcessor
{
public:
	Biquad() {}		/* C-TOR */
	~Biquad() {}	/* D-TOR */

	// --- IAudioSignalProcessor FUNCTIONS --- //
	//
	/** reset: clear out the state array (flush delays); can safely ignore sampleRate argument - we don't need/use it */
	virtual bool reset(double _sampleRate)
	{
		memset(&stateArray[0], 0, sizeof(double) * numStates);
		return true;  // handled = true
	}

	/** return false: this object only processes samples */
	virtual bool canProcessAudioFrame() { return false; }

	/** process input x(n) through biquad to produce return value y(n) */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn);

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return BiquadParameters custom data structure
	*/
	BiquadParameters getParameters() { return parameters; }

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param BiquadParameters custom data structure
	*/
	void setParameters(const BiquadParameters& _parameters) { parameters = _parameters; }

	// --- MUTATORS & ACCESSORS --- //
	/** set the coefficient array NOTE: passing by pointer to array; allows us to use "array notation" with pointers i.e. [ ] */
	void setCoefficients(double* coeffs) {
		// --- fast block memory copy:
		memcpy(&coeffArray[0], &coeffs[0], sizeof(double) * numCoeffs);
	}

	/** get the coefficient array for read/write access to the array (not used in current objects) */
	double* getCoefficients()
	{
		// --- read/write access to the array (not used)
		return &coeffArray[0];
	}

	/** get the state array for read/write access to the array (used only in direct form oscillator) */
	double* getStateArray()
	{
		// --- read/write access to the array (used only in direct form oscillator)
		return &stateArray[0];
	}

	/** get the structure G (gain) value for Harma filters; see 2nd Ed FX book */
	double getG_value() { return coeffArray[a0]; }

	/** get the structure S (storage) value for Harma filters; see 2nd Ed FX book */
	double getS_value();// { return storageComponent; }

protected:
	/** array of coefficients */
	double coeffArray[numCoeffs] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

	/** array of state (z^-1) registers */
	double stateArray[numStates] = { 0.0, 0.0, 0.0, 0.0 };

	/** type of calculation (algorithm  structure) */
	BiquadParameters parameters;

	/** for Harma loop resolution */
	double storageComponent = 0.0;
};

/**
\enum filterAlgorithm
\ingroup Constants-Enums
\brief
Use this strongly typed enum to easily set the filter algorithm for the AudioFilter object or any other object that requires filter definitions.

- filterAlgorithm { kLPF1P, kLPF1, kHPF1, kLPF2, kHPF2, kBPF2, kBSF2, kButterLPF2, kButterHPF2, kButterBPF2, kButterBSF2, kMMALPF2, kMMALPF2B, kLowShelf, kHiShelf, kNCQParaEQ, kCQParaEQ, kLWRLPF2, kLWRHPF2, kAPF1, kAPF2, kResonA, kResonB, kMatchLP2A, kMatchLP2B, kMatchBP2A, kMatchBP2B, kImpInvLP1, kImpInvLP2 };

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
enum class filterAlgorithm {
	kLPF1P, kLPF1, kHPF1, kLPF2, kHPF2, kBPF2, kBSF2, kButterLPF2, kButterHPF2, kButterBPF2,
	kButterBSF2, kMMALPF2, kMMALPF2B, kLowShelf, kHiShelf, kNCQParaEQ, kCQParaEQ, kLWRLPF2, kLWRHPF2,
	kAPF1, kAPF2, kResonA, kResonB, kMatchLP2A, kMatchLP2B, kMatchBP2A, kMatchBP2B,
	kImpInvLP1, kImpInvLP2
}; // --- you will add more here...

/**
\struct AudioFilterParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the AudioFilter object.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct AudioFilterParameters
{
	AudioFilterParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	AudioFilterParameters& operator=(const AudioFilterParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;
		algorithm = params.algorithm;
		fc = params.fc;
		Q = params.Q;
		boostCut_dB = params.boostCut_dB;

		return *this;
	}

	// --- individual parameters
	filterAlgorithm algorithm = filterAlgorithm::kLPF1; ///< filter algorithm
	double fc = 100.0; ///< filter cutoff or center frequency (Hz)
	double Q = 0.707; ///< filter Q
	double boostCut_dB = 0.0; ///< filter gain; note not used in all types
};

/**
\class AudioFilter
\ingroup FX-Objects
\brief
The AudioFilter object implements all filters in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use AudioFilterParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class AudioFilter : public IAudioSignalProcessor
{
public:
	AudioFilter() {}		/* C-TOR */
	~AudioFilter() {}		/* D-TOR */

	// --- IAudioSignalProcessor
	/** --- set sample rate, then update coeffs */
	virtual bool reset(double _sampleRate)
	{
		BiquadParameters bqp = biquad.getParameters();

		// --- you can try both forms - do you hear a difference?
		bqp.biquadCalcType = biquadAlgorithm::kTransposeCanonical; //<- this is the default operation
	//	bqp.biquadCalcType = biquadAlgorithm::kDirect;
		biquad.setParameters(bqp);

		sampleRate = _sampleRate;
		return biquad.reset(_sampleRate);
	}

	/** return false: this object only processes samples */
	virtual bool canProcessAudioFrame() { return false; }

	/** process input x(n) through the filter to produce return value y(n) */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn);

	/** --- sample rate change necessarily requires recalculation */
	virtual void setSampleRate(double _sampleRate)
	{
		sampleRate = _sampleRate;
		calculateFilterCoeffs();
	}

	/** --- get parameters */
	AudioFilterParameters getParameters() { return audioFilterParameters; }

	/** --- set parameters */
	void setParameters(const AudioFilterParameters& parameters)
	{
		if (audioFilterParameters.algorithm != parameters.algorithm ||
			audioFilterParameters.boostCut_dB != parameters.boostCut_dB ||
			audioFilterParameters.fc != parameters.fc ||
			audioFilterParameters.Q != parameters.Q)
		{
			// --- save new params
			audioFilterParameters = parameters;
		}
		else
			return;

		// --- don't allow 0 or (-) values for Q
		if (audioFilterParameters.Q <= 0)
			audioFilterParameters.Q = 0.707;

		// --- update coeffs
		calculateFilterCoeffs();
	}

	/** --- helper for Harma filters (phaser) */
	double getG_value() { return biquad.getG_value(); }

	/** --- helper for Harma filters (phaser) */
	double getS_value() { return biquad.getS_value(); }

protected:
	// --- our calculator
	Biquad biquad; ///< the biquad object

	// --- array to hold coeffs (we need them too)
	double coeffArray[numCoeffs] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; ///< our local copy of biquad coeffs

	// --- object parameters
	AudioFilterParameters audioFilterParameters; ///< parameters
	double sampleRate = 44100.0; ///< current sample rate

	/** --- function to recalculate coefficients due to a change in filter parameters */
	bool calculateFilterCoeffs();
};

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

/**
\enum generatorWaveform
\ingroup Constants-Enums
\brief
Use this strongly typed enum to easily set the oscillator waveform

- enum  generatorWaveform { kTriangle, kSin, kSaw };

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
enum class generatorWaveform { kTriangle, kSin, kSaw };

/**
\struct OscillatorParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the LFO and DFOscillator objects.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct OscillatorParameters
{
	OscillatorParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	OscillatorParameters& operator=(const OscillatorParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		waveform = params.waveform;
		frequency_Hz = params.frequency_Hz;
		return *this;
	}

	// --- individual parameters
	generatorWaveform waveform = generatorWaveform::kTriangle; ///< the current waveform
	double frequency_Hz = 0.0;	///< oscillator frequency
};

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

/**
\class CircularBuffer
\ingroup FX-Objects
\brief
The CircularBuffer object implements a simple circular buffer. It uses a wrap mask to wrap the read or write index quickly.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
/** A simple cyclic buffer: NOTE - this is NOT an IAudioSignalProcessor or IAudioSignalGenerator
	S must be a power of 2.
*/
template <typename T>
class CircularBuffer
{
public:
	CircularBuffer() {}		/* C-TOR */
	~CircularBuffer() {}	/* D-TOR */

							/** flush buffer by resetting all values to 0.0 */
	void flushBuffer() { memset(&buffer[0], 0, bufferLength * sizeof(T)); }

	/** Create a buffer based on a target maximum in SAMPLES
	//	   do NOT call from realtime audio thread; do this prior to any processing */
	void createCircularBuffer(unsigned int _bufferLength)
	{
		// --- find nearest power of 2 for buffer, and create
		createCircularBufferPowerOfTwo((unsigned int)(pow(2, ceil(log(_bufferLength) / log(2)))));
	}

	/** Create a buffer based on a target maximum in SAMPLESwhere the size is
		pre-calculated as a power of two */
	void createCircularBufferPowerOfTwo(unsigned int _bufferLengthPowerOfTwo)
	{
		// --- reset to top
		writeIndex = 0;

		// --- find nearest power of 2 for buffer, save it as bufferLength
		bufferLength = _bufferLengthPowerOfTwo;

		// --- save (bufferLength - 1) for use as wrapping mask
		wrapMask = bufferLength - 1;

		// --- create new buffer
		buffer.reset(new T[bufferLength]);

		// --- flush buffer
		flushBuffer();
	}

	/** write a value into the buffer; this overwrites the previous oldest value in the buffer */
	void writeBuffer(T input)
	{
		// --- write and increment index counter
		buffer[writeIndex++] = input;

		// --- wrap if index > bufferlength - 1
		writeIndex &= wrapMask;
	}

	/** read an arbitrary location that is delayInSamples old */
	T readBuffer(int delayInSamples)//, bool readBeforeWrite = true)
	{
		// --- subtract to make read index
		//     note: -1 here is because we read-before-write,
		//           so the *last* write location is what we use for the calculation
		int readIndex = (writeIndex - 1) - delayInSamples;

		// --- autowrap index
		readIndex &= wrapMask;

		// --- read it
		return buffer[readIndex];
	}

	/** read an arbitrary location that includes a fractional sample */
	T readBuffer(double delayInFractionalSamples)
	{
		// --- truncate delayInFractionalSamples and read the int part
		T y1 = readBuffer((int)delayInFractionalSamples);

		// --- if no interpolation, just return value
		if (!interpolate) return y1;

		// --- else do interpolation
		//
		// --- read the sample at n+1 (one sample OLDER)
		T y2 = readBuffer((int)delayInFractionalSamples + 1);

		// --- get fractional part
		double fraction = delayInFractionalSamples - (int)delayInFractionalSamples;

		// --- do the interpolation (you could try different types here)
		return doLinearInterpolation(y1, y2, fraction);
	}

	/** enable or disable interpolation; usually used for diagnostics or in algorithms that require strict integer samples times */
	void setInterpolate(bool b) { interpolate = b; }

private:
	std::unique_ptr<T[]> buffer = nullptr;	///< smart pointer will auto-delete
	unsigned int writeIndex = 0;		///> write index
	unsigned int bufferLength = 1024;	///< must be nearest power of 2
	unsigned int wrapMask = 1023;		///< must be (bufferLength - 1)
	bool interpolate = true;			///< interpolation (default is ON)
};

/**
\struct SimpleDelayParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the SimpleDelay object. Used for reverb algorithms in book.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct SimpleDelayParameters
{
	SimpleDelayParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	SimpleDelayParameters& operator=(const SimpleDelayParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		delayTime_mSec = params.delayTime_mSec;
		interpolate = params.interpolate;
		delay_Samples = params.delay_Samples;
		return *this;
	}

	// --- individual parameters
	double delayTime_mSec = 0.0;	///< delay tine in mSec
	bool interpolate = false;		///< interpolation flag (diagnostics usually)

	// --- outbound parameters
	double delay_Samples = 0.0;		///< current delay in samples; other objects may need to access this information
};

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

/**
\struct NestedDelayAPFParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the NestedDelayAPF object. Used for reverb algorithms in book.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct NestedDelayAPFParameters
{
	NestedDelayAPFParameters() {}
	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	NestedDelayAPFParameters& operator=(const NestedDelayAPFParameters& params)	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		outerAPFdelayTime_mSec = params.outerAPFdelayTime_mSec;
		innerAPFdelayTime_mSec = params.innerAPFdelayTime_mSec;
		outerAPF_g = params.outerAPF_g;
		innerAPF_g = params.innerAPF_g;

		// --- outer LFO
		enableLFO = params.enableLFO;
		lfoRate_Hz = params.lfoRate_Hz;
		lfoDepth = params.lfoDepth;
		lfoMaxModulation_mSec = params.lfoMaxModulation_mSec;

		return *this;
	}

	// --- individual parameters
	double outerAPFdelayTime_mSec = 0.0;	///< delay time for outer APF
	double innerAPFdelayTime_mSec = 0.0;	///< delay time for inner APF
	double outerAPF_g = 0.0;				///< g coefficient for outer APF
	double innerAPF_g = 0.0;				///< g coefficient for inner APF

	// --- this LFO belongs to the outer APF only
	bool enableLFO = false;					///< flag to enable the modulated delay
	double lfoRate_Hz = 0.0;				///< LFO rate in Hz (if enabled)
	double lfoDepth = 1.0;					///< LFO depth (not in %) (if enabled)
	double lfoMaxModulation_mSec = 0.0;		///< max modulation time if LFO is enabled

};

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

/**
\enum reverbDensity
\ingroup Constants-Enums
\brief
Use this strongly typed enum to easily set the density in the reverb object.

- enum class reverbDensity { kThick, kSparse };

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
enum class reverbDensity { kThick, kSparse };

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

// --- constants for reverb tank
const unsigned int NUM_BRANCHES = 4;
const unsigned int NUM_CHANNELS = 2; // stereo

/**
\class ReverbTank
\ingroup FX-Objects
\brief
The ReverbTank object implements the cyclic reverb tank in the FX book listed below.

Audio I/O:
- Processes mono input to mono OR stereo output.

Control I/F:
- Use ReverbTankParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class ReverbTank : public IAudioSignalProcessor
{
public:
	ReverbTank() {}		/* C-TOR */
	~ReverbTank() {}	/* D-TOR */

	/** reset members to initialized state */
	virtual bool reset(double _sampleRate)
	{
		// ---store
		sampleRate = _sampleRate;

		// ---set up preDelay
		preDelay.reset(_sampleRate);
		preDelay.createDelayBuffer(_sampleRate, 100.0);

		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			branchDelays[i].reset(_sampleRate);
			branchDelays[i].createDelayBuffer(_sampleRate, 100.0);

			branchNestedAPFs[i].reset(_sampleRate);
			branchNestedAPFs[i].createDelayBuffers(_sampleRate, 100.0, 100.0);

			branchLPFs[i].reset(_sampleRate);
		}
		for (int i = 0; i < NUM_CHANNELS; i++)
		{
			shelvingFilters[i].reset(_sampleRate);
		}

		return true;
	}

	/** return true: this object can process frames */
	virtual bool canProcessAudioFrame() { return true; }

	/** process mono reverb tank */
	/**
	\param xn input
	\return the processed sample
	*/
	virtual double processAudioSample(double xn)
	{
		float inputs[2] = { 0.0, 0.0 };
		float outputs[2] = { 0.0, 0.0 };
		processAudioFrame(inputs, outputs, 1, 1);
		return outputs[0];
	}

	/** process stereo reverb tank */
	virtual bool processAudioFrame(const float* inputFrame,
		float* outputFrame,
		uint32_t inputChannels,
		uint32_t outputChannels)
	{
		// --- global feedback from delay in last branch
		double globFB = branchDelays[NUM_BRANCHES - 1].readDelay();

		// --- feedback value
		double fb = parameters.kRT * (globFB);

		// --- mono-ized input signal
		double xnL = inputFrame[0];
		double xnR = inputChannels > 1 ? inputFrame[1] : 0.0;
		double monoXn = double(1.0 / inputChannels) * xnL + double(1.0 / inputChannels) * xnR;

		// --- pre delay output
		double preDelayOut = preDelay.processAudioSample(monoXn);

		// --- input to first branch = preDalay + globFB
		double input = preDelayOut + fb;
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			double apfOut = branchNestedAPFs[i].processAudioSample(input);
			double lpfOut = branchLPFs[i].processAudioSample(apfOut);
			double delayOut = parameters.kRT * branchDelays[i].processAudioSample(lpfOut);
			input = delayOut + preDelayOut;
		}
		// --- gather outputs
		/*
		There are 25 prime numbers between 1 and 100.
		They are 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41,
		43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, and 97

		we want 16 of them: 23, 29, 31, 37, 41,
		43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, and 97
		*/

		double weight = 0.707;

		double outL = 0.0;
		outL += weight * branchDelays[0].readDelayAtPercentage(23.0);
		outL -= weight * branchDelays[1].readDelayAtPercentage(41.0);
		outL += weight * branchDelays[2].readDelayAtPercentage(59.0);
		outL -= weight * branchDelays[3].readDelayAtPercentage(73.0);

		double outR = 0.0;
		outR -= weight * branchDelays[0].readDelayAtPercentage(29.0);
		outR += weight * branchDelays[1].readDelayAtPercentage(43.0);
		outR -= weight * branchDelays[2].readDelayAtPercentage(61.0);
		outR += weight * branchDelays[3].readDelayAtPercentage(79.0);

		if (parameters.density == reverbDensity::kThick)
		{
			outL += weight * branchDelays[0].readDelayAtPercentage(31.0);
			outL -= weight * branchDelays[1].readDelayAtPercentage(47.0);
			outL += weight * branchDelays[2].readDelayAtPercentage(67.0);
			outL -= weight * branchDelays[3].readDelayAtPercentage(83.0);

			outR -= weight * branchDelays[0].readDelayAtPercentage(37.0);
			outR += weight * branchDelays[1].readDelayAtPercentage(53.0);
			outR -= weight * branchDelays[2].readDelayAtPercentage(71.0);
			outR += weight * branchDelays[3].readDelayAtPercentage(89.0);
		}

		// ---  filter
		double tankOutL = shelvingFilters[0].processAudioSample(outL);
		double tankOutR = shelvingFilters[1].processAudioSample(outR);

		// --- sum with dry
		double dry = pow(10.0, parameters.dryLevel_dB / 20.0);
		double wet = pow(10.0, parameters.wetLevel_dB / 20.0);

		if (outputChannels == 1)
			outputFrame[0] = dry * xnL + wet * (0.5 * tankOutL + 0.5 * tankOutR);
		else
		{
			outputFrame[0] = dry * xnL + wet * tankOutL;
			outputFrame[1] = dry * xnR + wet * tankOutR;
		}

		return true;
	}

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return ReverbTankParameters custom data structure
	*/
	ReverbTankParameters getParameters() { return parameters; }

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param ReverbTankParameters custom data structure
	*/
	void setParameters(const ReverbTankParameters& params)
	{
		// --- do the updates here, the sub-components will only update themselves if
		//     their parameters changed, so we let those object handle that chore
		TwoBandShelvingFilterParameters filterParams = shelvingFilters[0].getParameters();
		filterParams.highShelf_fc = params.highShelf_fc;
		filterParams.highShelfBoostCut_dB = params.highShelfBoostCut_dB;
		filterParams.lowShelf_fc = params.lowShelf_fc;
		filterParams.lowShelfBoostCut_dB = params.lowShelfBoostCut_dB;

		// --- copy to both channels
		shelvingFilters[0].setParameters(filterParams);
		shelvingFilters[1].setParameters(filterParams);

		SimpleLPFParameters  lpfParams = branchLPFs[0].getParameters();
		lpfParams.g = params.lpf_g;

		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			branchLPFs[i].setParameters(lpfParams);
		}

		// --- update pre delay
		SimpleDelayParameters delayParams = preDelay.getParameters();
		delayParams.delayTime_mSec = params.preDelayTime_mSec;
		preDelay.setParameters(delayParams);

		// --- set apf and delay parameters
		int m = 0;
		NestedDelayAPFParameters apfParams = branchNestedAPFs[0].getParameters();
		delayParams = branchDelays[0].getParameters();

		// --- global max Delay times
		double globalAPFMaxDelay = (parameters.apfDelayWeight_Pct / 100.0) * parameters.apfDelayMax_mSec;
		double globalFixedMaxDelay = (parameters.fixeDelayWeight_Pct / 100.0) * parameters.fixeDelayMax_mSec;

		// --- lfo
		apfParams.enableLFO = true;
		apfParams.lfoMaxModulation_mSec = 0.3;
		apfParams.lfoDepth = 1.0;

		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			// --- setup APFs
			apfParams.outerAPFdelayTime_mSec = globalAPFMaxDelay * apfDelayWeight[m++];
			apfParams.innerAPFdelayTime_mSec = globalAPFMaxDelay * apfDelayWeight[m++];
			apfParams.innerAPF_g = -0.5;
			apfParams.outerAPF_g = 0.5;
			if (i == 0)
				apfParams.lfoRate_Hz = 0.15;
			else if (i == 1)
				apfParams.lfoRate_Hz = 0.33;
			else if (i == 2)
				apfParams.lfoRate_Hz = 0.57;
			else if (i == 3)
				apfParams.lfoRate_Hz = 0.73;

			branchNestedAPFs[i].setParameters(apfParams);

			// --- fixedDelayWeight
			delayParams.delayTime_mSec = globalFixedMaxDelay * fixedDelayWeight[i];
			branchDelays[i].setParameters(delayParams);
		}

		// --- save our copy
		parameters = params;
	}

private:
	ReverbTankParameters parameters;				///< object parameters

	SimpleDelay  preDelay;							///< pre delay object
	SimpleDelay  branchDelays[NUM_BRANCHES];		///< branch delay objects
	NestedDelayAPF branchNestedAPFs[NUM_BRANCHES];	///< nested APFs for each branch
	SimpleLPF  branchLPFs[NUM_BRANCHES];			///< LPFs in each branch

	TwoBandShelvingFilter shelvingFilters[NUM_CHANNELS]; ///< shelving filters 0 = left; 1 = right

	// --- weighting values to make various and low-correlated APF delay values easily
	double apfDelayWeight[NUM_BRANCHES * 2] = { 0.317, 0.873, 0.477, 0.291, 0.993, 0.757, 0.179, 0.575 };///< weighting values to make various and low-correlated APF delay values easily
	double fixedDelayWeight[NUM_BRANCHES] = { 1.0, 0.873, 0.707, 0.667 };	///< weighting values to make various and fixed delay values easily
	double sampleRate = 0.0;	///< current sample rate
};