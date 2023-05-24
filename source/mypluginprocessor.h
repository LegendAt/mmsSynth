//------------------------------------------------------------------------
// Copyright(c) 2023 JKU.
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"
#include "myplugincids.h"

namespace MyCompanyName {

//------------------------------------------------------------------------
//  MMS_SynthProcessor
//------------------------------------------------------------------------
class MMS_SynthProcessor : public Steinberg::Vst::AudioEffect
{
public:
	MMS_SynthProcessor ();
	~MMS_SynthProcessor () SMTG_OVERRIDE;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/) 
	{ 
		return (Steinberg::Vst::IAudioProcessor*)new MMS_SynthProcessor; 
	}

	//--- ---------------------------------------------------------------------
	// AudioEffect overrides:
	//--- ---------------------------------------------------------------------
	/** Called at first after constructor */
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	
	/** Called at the end before destructor */
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	
	/** Switch the Plug-in on/off */
	Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;

	/** Will be called before any process call */
	Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
	
	/** Asks if a given sample size is supported see SymbolicSampleSizes. */
	Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

	/** Here we go...the process call */
	Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
		
	/** For persistence */
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

//------------------------------------------------------------------------
protected:

	// Master Volume
	float fVolume = 0.f;
	// Wave Generation
	float fWaveType = default_WaveType;
	float fWaveLevel = default_WaveLevel * 0.8f;
	float fWavePhase = 0.f;
	float fFrequency = 0.f;
	float fDeltaAngle = 0.f;
	// LFO
	float fLfoLevel = default_LfoLevel * 0.2f;
	float fLfoFreq = default_LfoFreq * 20.0f;
	float fLfoAngle = 0.0f;
	float fLfoPhase = 0.0f;
	bool bLfoActive = default_LfoActive;
	// Timer
	float fTimer = 0.0f;
	float fTimestep = 0.0f;
	// LevelEnvelope
	float fLevelAttack = default_LevelAttack;
};
#define PI2  (3.14159265f*2.f)

//------------------------------------------------------------------------
} // namespace MyCompanyName



// Waveform generation functions
float generate(float phase);
float rect(float phase);
float tri(float phase);
float saw(float phase);