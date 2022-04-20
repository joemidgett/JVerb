// Utilities.h

#pragma once

// --- constants & enumerations
//
// ---  by placing outside the class declaration, these will also be avaialble to anything
//      that #includes this file (sometimes needed)
const double kSmallestPositiveFloatValue = 1.175494351e-38;         /* min positive value */
const double kSmallestNegativeFloatValue = -1.175494351e-38;         /* min negative value */
const double kSqrtTwo = pow(2.0, 0.5);

// --- constants for reverb tank
const unsigned int NUM_BRANCHES = 4;
const unsigned int NUM_CHANNELS = 2; // stereo

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
@doLinearInterpolation
\ingroup FX-Functions

@brief performs linear interpolation of fractional x distance between two adjacent (x,y) points;
returns interpolated value

\param y1 - the y coordinate of the first point
\param y2 - the 2 coordinate of the second point
\param x - the interpolation location as a fractional distance between x1 and x2 (which are not needed)
\return the interpolated value or y2 if the interpolation is outside the x interval
*/
inline double doLinearInterpolation(double y1, double y2, double fractional_X)
{
	// --- check invalid condition
	if (fractional_X >= 1.0) return y2;

	// --- use weighted sum method of interpolating
	return fractional_X * y2 + (1.0 - fractional_X) * y1;
}

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
@unipolarToBipolar
\ingroup FX-Functions

@brief calculates the bipolar [-1.0, +1.0] value FROM a unipolar [0.0, +1.0] value

\param value - value to convert
\return the bipolar value
*/
inline double unipolarToBipolar(double value)
{
	return 2.0 * value - 1.0;
}

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
