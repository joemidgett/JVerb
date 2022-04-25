// SignalGenData.h

#pragma once

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