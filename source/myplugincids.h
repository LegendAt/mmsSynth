//------------------------------------------------------------------------
// Copyright(c) 2023 JKU.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace MyCompanyName {
//------------------------------------------------------------------------
static const Steinberg::FUID kMMS_SynthProcessorUID (0xF9A6E051, 0xD4625A9D, 0x96958C38, 0xFFAC4D31);
static const Steinberg::FUID kMMS_SynthControllerUID (0x01F3DF32, 0x2AF05889, 0xB33809CF, 0x63A2A19C);

#define MMS_SynthVST3Category "Instrument"

//------------------------------------------------------------------------
} // namespace MyCompanyName

enum GainParams : Steinberg::Vst::ParamID {
	// Wave Generator
	kWaveType = 100,
	kWaveLevel = 101,
	// Level Envelope
	kLevelAttack = 201,
	kLevelDecay = 202,
	kLevelSustain = 203,
	kLevelRelease = 204,
	kLevelActive = 205,
	// Filter Envelope
	kFilterAttack = 301,
	kFilterDecay = 302,
	kFilterSustain = 303,
	kFilterRelease = 304,
	kFilterEnvelopeActive = 305,
	// Filter
	kFilterType = 401,
	kFilterLow = 402,
	kFilterHigh = 403,
	kFilterLevel = 404,
	kFilterActive = 405,
	// LFO
	kLfoFreq = 501,
	kLfoLevel = 502,
	kLfoActive = 503,

	// Frequency Analyser TODO

	kFaActive = 901,

};

// Default Values
// Wave Types 0 = Sine, 1 = Triangle, 2 = SawTooth, 3 = Rectangular 
#define default_WaveType 0
#define default_WaveLevel  1
// Level Envelope
#define default_LevelAttack 0.5
#define default_LevelDecay 0.5
#define default_LevelSustain 0.5
#define default_LevelRelease 0.5
// Filter Envelope
#define default_FilterAttack 0
#define default_FilterDecay 0
#define default_FilterSustain 1
#define default_FilterRelease 0
#define default_FilterActive 0
// Filter
#define default_FilterType 0
#define default_FilterLow 0
#define default_FilterHigh 1
// LFO
#define default_LfoFreq 0.2
#define default_LfoLevel 1
#define default_LfoActive true
// Frequency Analyser
#define default_FaActive = 0